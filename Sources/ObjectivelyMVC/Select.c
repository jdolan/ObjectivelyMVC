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

	if (this->control.selection == ControlSelectionSingle) {

		const Array *options = (Array *) this->options;
		for (size_t i = 0; i < options->count; i++) {

			Option *option = $(options, objectAtIndex, i);
			if ($((Control *) self, highlighted)) {
				option->view.hidden = false;
			} else if (option->isSelected) {
				option->view.hidden = false;
			} else {
				option->view.hidden = true;
			}
		}

		$((View *) this->stackView, sizeToFit);
	}

	super(View, self, layoutSubviews);
}

/**
 * @see View::sizeThatFits(const View *)
 */
static SDL_Size sizeThatFits(const View *self) {

	const Select *this = (Select *) self;

	SDL_Size size = super(View, self, sizeThatFits);

	const Array *options = (Array *) this->options;
	for (size_t i = 0; i < options->count; i++) {

		const View *option = $(options, objectAtIndex, i);
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
					$(this, selectOptionWithValue, option->value);
					return true;
				}
			}
		} else if ($((View *) self, didReceiveEvent, event)) {
			self->state |= ControlStateHighlighted;
			return true;
		}
	}

	return super(Control, self, captureEvent, event);
}

#pragma mark - Select

/**
 * @brief ArrayEnumerator for adding sorted Options.
 */
static void addOption_removeSubview(const Array *array, ident obj, ident data) {
	$((View *) data, removeSubview, (View *) obj);
}

/**
 * @brief ArrayEnumerator for removing sorted Options.
 */
static void addOption_addSubview(const Array *array, ident obj, ident data) {
	$((View *) data, addSubview, (View *) obj);
}

/**
 * @fn void Select::addOption(Select *self, const char *title, ident value)
 * @memberof Select
 */
static void addOption(Select *self, const char *title, ident value) {

	Option *option = $(alloc(Option), initWithTitle, title, value);
	assert(option);

	$(self->options, addObject, option);
	$((View *) self->stackView, addSubview, (View *) option);

	if (self->comparator) {
		const Array *options = (Array *) self->options;
		$(options, enumerateObjects, addOption_removeSubview, self->stackView);
		$(self->options, sort, self->comparator);
		$(options, enumerateObjects, addOption_addSubview, self->stackView);
	}

	if (self->control.selection == ControlSelectionSingle) {
		if ($(self, selectedOption) == NULL) {
			$(self, selectOption, option);
		}
	}

	release(option);
}

/**
 * @fn Select *Select::initWithFrame(Select *self, const SDL_Rect *frame, ControlStyle style)
 * @memberof Select
 */
static Select *initWithFrame(Select *self, const SDL_Rect *frame, ControlStyle style) {

	self = (Select *) super(Control, self, initWithFrame, frame, style);
	if (self) {

		self->control.selection = ControlSelectionSingle;

		self->options = $$(MutableArray, array);
		assert(self->options);

		self->stackView = $(alloc(StackView), initWithFrame, NULL);
		assert(self->stackView);

		$((View *) self, addSubview, (View *) self->stackView);
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
 * @memberof Select
 */
static Option *optionWithValue(const Select *self, const ident value) {
	return $((Array *) self->options, findObject, optionWithValue_predicate, value);
}

/**
 * @brief ArrayEnumerator for removeAllOptions.
 */
static void removeAllOptions_enumerate(const Array *array, ident obj, ident data) {
	$((Select *) data, removeOption, obj);
}

/**
 * @fn void Select::removeAllOptions(Select *self)
 * @memberof Select
 */
static void removeAllOptions(Select *self) {
	$((Array *) self->options, enumerateObjects, removeAllOptions_enumerate, self);
}

/**
 * @fn void Select::removeOption(Select *self, Option *option)
 * @memberof Select
 */
static void removeOption(Select *self, Option *option) {

	if ($((Array *) self->options, containsObject, option)) {

		$(self->options, removeObject, option);
		$((View *) self->stackView, removeSubview, (View *) option);

		if (self->control.selection == ControlSelectionSingle) {
			if ($(self, selectedOption) == NULL) {
				Option *option = $((Array *) self->options, firstObject);
				if (option) {
					$(self, selectOption, option);
				}
			}
		}
	}
}

/**
 * @fn void Select::removeOptionWithValue(Select *self, ident value)
 * @memberof Select
 */
static void removeOptionWithValue(Select *self, ident value) {

	Option *option = (Option *) $(self, optionWithValue, value);
	if (option) {
		$(self, removeOption, option);
	}
}

/**
 * @brief ArrayEnumerator for enforcing ControlSelectionSingle.
 */
static void selectOption_enumerate(const Array *array, ident obj, ident data) {
	$((Option *) obj, setSelected, false);
}

/**
 * @fn void Select::selectOption(Select *self, Option *option)
 * @memberof Select
 */
static void selectOption(Select *self, Option *option) {

	if (self->control.selection == ControlSelectionSingle) {
		$((Array *) self->options, enumerateObjects, selectOption_enumerate, NULL);
	}

	if (option) {

		if (option && option->isSelected == false) {
			$(option, setSelected, true);

			if (self->delegate.didSelectOption) {
				self->delegate.didSelectOption(self, option);
			}
		}
	}

	self->stackView->view.needsLayout = true;
}

/**
 * @fn void Select::selectOptionWithValue(Select *self, ident value)
 * @memberof Select
 */
static void selectOptionWithValue(Select *self, ident value) {

	Option *option = $(self, optionWithValue, value);

	$(self, selectOption, option);
}

/**
 * @brief Predicate for selectedOption and selectedOptions.
 */
static _Bool selectedOptions_predicate(ident obj, ident data) {
	return ((Option *) obj)->isSelected;
}

/**
 * @fn Option *Select::selectedOption(const Select *self)
 * @memberof Select
 */
static Option *selectedOption(const Select *self) {
	return $((Array *) self->options, findObject, selectedOptions_predicate, NULL);
}

/**
 * @fn Array *Select::selectedOptions(const Select *self)
 * @memberof Select
 */
static Array *selectedOptions(const Select *self) {
	return $((Array *) self->options, filteredArray, selectedOptions_predicate, NULL);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->init = init;
	((ViewInterface *) clazz->def->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->def->interface)->sizeThatFits = sizeThatFits;

	((ControlInterface *) clazz->def->interface)->captureEvent = captureEvent;

	((SelectInterface *) clazz->def->interface)->addOption = addOption;
	((SelectInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((SelectInterface *) clazz->def->interface)->optionWithValue = optionWithValue;
	((SelectInterface *) clazz->def->interface)->removeAllOptions = removeAllOptions;
	((SelectInterface *) clazz->def->interface)->removeOption = removeOption;
	((SelectInterface *) clazz->def->interface)->removeOptionWithValue = removeOptionWithValue;
	((SelectInterface *) clazz->def->interface)->selectOption = selectOption;
	((SelectInterface *) clazz->def->interface)->selectOptionWithValue = selectOptionWithValue;
	((SelectInterface *) clazz->def->interface)->selectedOption = selectedOption;
	((SelectInterface *) clazz->def->interface)->selectedOptions = selectedOptions;
}

/**
 * @fn Class *Select::_Select(void)
 * @memberof Select
 */
Class *_Select(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Select";
		clazz.superclass = _Control();
		clazz.instanceSize = sizeof(Select);
		clazz.interfaceOffset = offsetof(Select, interface);
		clazz.interfaceSize = sizeof(SelectInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class

