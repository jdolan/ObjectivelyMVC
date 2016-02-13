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

#include <ObjectivelyMVC/Label.h>
#include <ObjectivelyMVC/Select.h>

#define _Class _Select

#pragma mark - View

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {
	
	Select *this = (Select *) self;
	Array *options = $(this, options);

	const View *selected = (View *) $(this, selectedOption) ?: $(options, firstObject);
	
	self->frame.h = DEFAULT_CONTROL_HEIGHT;
	int pos = 0;

	for (size_t i = 0; i < options->count; i++) {
		
		View *option = $(options, objectAtIndex, i);
		if (option == selected) {
			option->hidden = false;
		} else {
			option->hidden = this->control.state != ControlStateHighlighted;
		}
		
		if (option->hidden) {
			option->frame.y = 0;
		} else {
			option->frame.y = pos;
			pos += option->frame.h;
		}
	}
	
	release(options);
	
	$(self, sizeToFit);
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {
	
	if (event->type == SDL_MOUSEBUTTONUP) {
		if ($((View *) self, didReceiveEvent, event)) {
			self->state |= ControlStateHighlighted;
			return true;
		}
		self->state &= ~ControlStateHighlighted;
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
	
	super(Control, self, stateDidChange);
}

#pragma mark - Select

/**
 * @fn void Select::addOption(Select *self, ident value, const char *text, Font *font)
 *
 * @memberof Select
 */
static void addOption(Select *self, ident value, const char *text, Font *font) {
	
	View *option = (View *) $(alloc(Option), initWithValue, value, text, font);
	assert(option);
	
	if (self->control.style == ControlStyleDefault) {
		const int padding = DEFAULT_OPTION_HEIGHT - option->frame.h;
		if (padding > 0) {
			option->padding.top = option->padding.bottom = padding * 0.5;
		}
		option->borderWidth = 1;
	}
	
	$((View *) self, addSubview, option);
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
		
		if (self->control.style == ControlStyleDefault) {
			self->control.bevel = BevelTypeOutset;
			
			if (self->control.view.frame.w == 0) {
				self->control.view.frame.w = DEFAULT_SELECT_WIDTH;
			}
		}
	}
	
	return self;
}

/**
 * @brief ArrayEnumerator for filtering Options from subviews.
 */
static _Bool options_filter(const Array *array, ident obj, ident data) {
	return $((Object *) obj, isKindOfClass, &_Option);
}

/**
 * @fn Array *Select::options(const Select *self)
 *
 * @memberof Select
 */
static Array *options(const Select *self) {
	
	const Array *subviews = (Array *) ((View *) self)->subviews;
	
	return $(subviews, filterObjects, options_filter, NULL);
}

/**
 * @fn void Select::removeOptionWithValue(Select *self, const ident value)
 *
 * @memberof Select
 */
static void removeOptionWithValue(Select *self, const ident value) {
	
	Array *options = $(self, options);
	
	for (size_t i = 0; i < options->count; i++) {
		View *option = $(options, objectAtIndex, i);
		if (((Option *) option)->value == value) {
			$(option, removeFromSuperview);
			break;
		}
	}
	
	release(options);
}

/**
 * @fn Option *Select::selectedOption(const Select *self)
 *
 * @memberof Select
 */
static Option *selectedOption(const Select *self) {
	
	Option *selected = NULL;
	Array *options = $(self, options);
	
	for (size_t i = 0; i < options->count; i++) {
		Option *option = $(options, objectAtIndex, i);
		if (option->selected) {
			selected = option;
			break;
		}
	}
	
	release(options);
	return selected;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
	
	((ControlInterface *) clazz->interface)->captureEvent = captureEvent;
	((ControlInterface *) clazz->interface)->stateDidChange = stateDidChange;
	
	((SelectInterface *) clazz->interface)->addOption = addOption;
	((SelectInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((SelectInterface *) clazz->interface)->options = options;
	((SelectInterface *) clazz->interface)->removeOptionWithValue = removeOptionWithValue;
	((SelectInterface *) clazz->interface)->selectedOption = selectedOption;
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

