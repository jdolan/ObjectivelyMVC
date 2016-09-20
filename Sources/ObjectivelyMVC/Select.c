/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>

#include <Objectively/Array.h>

#include <ObjectivelyMVC/Text.h>
#include <ObjectivelyMVC/Select.h>

#define _Class _Select

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	Select *this = (Select *) self;
	
	release(this->options);
	
	release(this->stackView);
	
	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((Select *) self, initWithFrame, NULL, ControlStyleDefault);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {
	
	Select *this = (Select *) self;

	const Array *options = (Array *) this->options;
	for (size_t i = 0; i < options->count; i++) {
		
		View *option = $(options, objectAtIndex, i);
		if ((Option *) option == this->selectedOption) {
			option->hidden = false;
		} else {
			option->hidden = this->control.state != ControlStateHighlighted;
		}
		
		if (option->hidden == false) {

			if (this->control.state == ControlStateHighlighted) {
				if ((Option *) option == this->selectedOption) {
					option->backgroundColor = Colors.SelectedColor;
				} else {
					option->backgroundColor = Colors.HighlightedColor;
				}
			} else {
				option->backgroundColor = Colors.Clear;
			}
		}
	}

	if (this->control.state == ControlStateHighlighted) {
		this->stackView->view.borderWidth = 1;
	} else {
		this->stackView->view.borderWidth = 0;
	}

	$((View *) this->stackView, sizeToFit);

	super(View, self, layoutSubviews);
}

/**
 * @see View::sizeThatFits(const View *)
 */
static SDL_Size sizeThatFits(const View *self) {

	const Select *this = (Select *) self;

	SDL_Size size = super(View, self, sizeThatFits);

	if (((Control *) self)->style == ControlStyleDefault) {
		size.w = DEFAULT_SELECT_WIDTH;
	} else {
		size.w = 0;
	}

	const Array *options = (Array *) this->options;
	for (size_t i = 0; i < options->count; i++) {

		const View *option = (View *) $(options, objectAtIndex, i);
		const SDL_Size optionSize = $(option, sizeThatFits);

		size.w = max(size.w, optionSize.w + self->padding.left + self->padding.right);
	}

	return size;
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {
	
	Select *this = (Select *) self;
	
	if (event->type == SDL_MOUSEBUTTONUP) {
		if (self->state == ControlStateHighlighted) {
			self->state &= ~ControlStateHighlighted;

			const Array *options = (Array *) this->options;
			for (size_t i = 0; i < options->count; i++) {
				
				Option *option = $(options, objectAtIndex, i);
				if ($((View *) option, didReceiveEvent, event)) {
					this->selectedOption = option;
					if (this->delegate.didSelectOption) {
						this->delegate.didSelectOption(this, option);
					}
					return true;
				}
			}
		} else if ($((View *) self, didReceiveEvent, event)) {
			self->state |= ControlStateHighlighted;
			return true;
		}
	}
	
	else if (event->type == SDL_MOUSEMOTION) {
		if (this->control.state == ControlStateHighlighted) {
			
			const Array *options = (Array *) this->options;
			for (size_t i = 0; i < options->count; i++) {
				
				View *option = $(options, objectAtIndex, i);
				if ($(option, didReceiveEvent, event)) {
					option->backgroundColor = Colors.SelectedColor;
				} else {
					option->backgroundColor = Colors.HighlightedColor;
				}
			}
		}
	}

	return super(Control, self, captureEvent, event);
}

/**
 * @fn void Control::stateDidChange(Control *self)
 *
 * @memberof Control
 */
static void stateDidChange(Control *self) {
	
	((View *) self)->needsLayout = true;

	if (self->state & ControlStateHighlighted) {
		self->view.zIndex = 4;
	} else {
		self->view.zIndex = 0;
	}

	super(Control, self, stateDidChange);
}

#pragma mark - Select

/**
 * @brief ArrayEnumerator to remove Options from the Select's StackView.
 */
static _Bool addOption_removeOptions(const Array *array, ident obj, ident data) {
	$((View *) data, removeSubview, (View *) obj); return false;
}

/**
 * @brief ArrayEnumerator to add Options to the Select's StacKView.
 */
static _Bool addOption_addOptions(const Array *array, ident obj, ident data) {
	$((View *) data, addSubview, (View *) obj); return false;
}

/**
 * @fn void Select::addOption(Select *self, const char *title, ident value)
 *
 * @memberof Select
 */
static void addOption(Select *self, const char *title, ident value) {
	
	Option *option = $alloc(Option, initWithTitle, title, value);
	assert(option);
	
	$(self->options, addObject, option);
	$((View *) self->stackView, addSubview, (View *) option);

	if (self->comparator) {
		const Array *options = (Array *) self->options;
		$(options, enumerateObjects, addOption_removeOptions, self->stackView);
		$(self->options, sort, self->comparator);
		$(options, enumerateObjects, addOption_addOptions, self->stackView);
	}

	if (self->selectedOption == NULL) {
		self->selectedOption = option;
	}

	self->control.view.needsLayout = true;
	
	release(option);
}

/**
 * @fn Select *Select::initWithFrame(Select *self, const SDL_Rect *frame, ControlStyle style)
 *
 * @memberof Select
 */
static Select *initWithFrame(Select *self, const SDL_Rect *frame, ControlStyle style) {
	
	self = (Select *) super(Control, self, initWithFrame, frame, style);
	if (self) {
		
		self->options = $$(MutableArray, array);
		assert(self->options);
		
		self->stackView = $alloc(StackView, initWithFrame, NULL);
		assert(self->stackView);
		
		$((View *) self, addSubview, (View *) self->stackView);
		
		self->stackView->view.alignment = ViewAlignmentMiddleLeft;
		self->stackView->view.autoresizingMask |= ViewAutoresizingWidth;
		self->stackView->view.borderWidth = 1;

		self->control.selection = ControlSelectionSingle;
		
		if (self->control.style == ControlStyleDefault) {
			self->control.bevel = ControlBevelTypeOutset;
			
			if (self->control.view.frame.w == 0) {
				self->control.view.frame.w = DEFAULT_SELECT_WIDTH;
			}

			self->stackView->view.borderColor = Colors.Silver;
		}
	}
	
	return self;
}

/**
 * @brief Predicate function for optionWithValue.
 */
static _Bool optionWithValue_predicate(ident obj, ident data) {
	return ((Option *) obj)->value == data;
}

/**
 * @fn Option *Select::optionWithValue(const Select *self, ident value)
 *
 * @memberof Select
 */
static Option *optionWithValue(const Select *self, const ident value) {
	return $((Array *) self->options, findObject, optionWithValue_predicate, value);
}

/**
 * @brief ArrayEnumerator for removeAllOptions.
 */
static _Bool removeAllOptions_enumerate(const Array *array, ident obj, ident data) {
	$((View *) obj, removeFromSuperview); return false;
}

/**
 * @fn void Select::removeAllOptions(Select *self)
 *
 * @brief Removes all Options from this Select.
 *
 * @memberof Select
 */
static void removeAllOptions(Select *self) {
	
	$((Array *) self->options, enumerateObjects, removeAllOptions_enumerate, self);

	$(self->options, removeAllObjects);

	self->selectedOption = NULL;

	self->control.view.needsLayout = true;
}

/**
 * @fn void Select::removeOption(Select *self, Option *option)
 *
 * @memberof Select
 */
static void removeOption(Select *self, Option *option) {

	$(self->options, removeObject, option);

	if ((Option *) option == self->selectedOption) {
		self->selectedOption = $((Array *) self->options, firstObject);
	}

	$((View *) option, removeFromSuperview);

	self->control.view.needsLayout = true;
}

/**
 * @fn void Select::removeOptionWithValue(Select *self, ident value)
 *
 * @memberof Select
 */
static void removeOptionWithValue(Select *self, ident value) {
	
	Option *option = (Option *) $(self, optionWithValue, value);
	if (option) {
		$(self, removeOption, option);
	}
}

/**
 * @fn void Select::selectOptionWithValue(Select *self, ident value)
 *
 * @memberof Select
 */
static void selectOptionWithValue(Select *self, ident value) {
	self->selectedOption = $(self, optionWithValue, value);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->init = init;
	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;
	
	((ControlInterface *) clazz->interface)->captureEvent = captureEvent;
	((ControlInterface *) clazz->interface)->stateDidChange = stateDidChange;
	
	((SelectInterface *) clazz->interface)->addOption = addOption;
	((SelectInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((SelectInterface *) clazz->interface)->optionWithValue = optionWithValue;
	((SelectInterface *) clazz->interface)->removeAllOptions = removeAllOptions;
	((SelectInterface *) clazz->interface)->removeOption = removeOption;
	((SelectInterface *) clazz->interface)->removeOptionWithValue = removeOptionWithValue;
	((SelectInterface *) clazz->interface)->selectOptionWithValue = selectOptionWithValue;
}

Class _Select = {
	.name = "Select",
	.superclass = &_Control,
	.instanceSize = sizeof(Select),
	.interfaceOffset = offsetof(Select, interface),
	.interfaceSize = sizeof(SelectInterface),
	.initialize = initialize,
};

#undef _Class

