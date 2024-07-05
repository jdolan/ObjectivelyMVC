/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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

#include "Control.h"
#include "Notification.h"

const EnumName ControlBevelNames[] = MakeEnumNames(
	MakeEnumAlias(ControlBevelNone, none),
	MakeEnumAlias(ControlBevelInset, inset),
	MakeEnumAlias(ControlBevelOutset, outset)
);

const EnumName ControlSelectionNames[] = MakeEnumNames(
	MakeEnumAlias(ControlSelectionNone, none),
	MakeEnumAlias(ControlSelectionSingle, single),
	MakeEnumAlias(ControlSelectionMultiple, multiple)
);

const EnumName ControlStateNames[] = MakeEnumNames(
	MakeEnumAlias(ControlStateDefault, default),
	MakeEnumAlias(ControlStateHighlighted, highlighted),
	MakeEnumAlias(ControlStateSelected, selected),
	MakeEnumAlias(ControlStateFocused, focused)
);

#define _Class _Control

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Control *this = (Control *) self;

	release(this->actions);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::acceptsFirstResponder(const View *)
 */
static bool acceptsFirstResponder(const View *self) {
	return true;
}

/**
 * @see View::applyStyle(View *, const Style *)
 */
static void applyStyle(View *self, const Style *style) {

	super(View, self, applyStyle, style);

	Control *this = (Control *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("bevel", InletTypeEnum, &this->bevel, (ident) (ControlBevelNames)),
		MakeInlet("selection", InletTypeEnum, &this->selection, (ident) ControlSelectionNames)
	);

	$(self, bind, inlets, (Dictionary *) style->attributes);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((Control *) self, initWithFrame, NULL);
}

/**
 * @see View::matchesSelector(const View *, const SimpleSelector *)
 */
static bool matchesSelector(const View *self, const SimpleSelector *simpleSelector) {

	assert(simpleSelector);

	const Control *this = (Control *) self;

	switch (simpleSelector->type) {
		case SimpleSelectorTypePseudo:
			if (strcmp("highlighted", simpleSelector->pattern) == 0) {
				return $(this, isHighlighted);
			} else if (strcmp("disabled", simpleSelector->pattern) == 0) {
				return $(this, isDisabled);
			} else if (strcmp("selected", simpleSelector->pattern) == 0) {
				return $(this, isSelected);
			} else if (strcmp("focused", simpleSelector->pattern) == 0) {
				return $(this, isFocused);
			} else if (strcmp("single", simpleSelector->pattern) == 0) {
				return this->selection == ControlSelectionSingle;
			} else if (strcmp("multiple", simpleSelector->pattern) == 0) {
				return this->selection == ControlSelectionMultiple;
			}
			break;
		default:
			break;
	}

	return super(View, self, matchesSelector, simpleSelector);
}

/**
 * @see View::render(View *, Renderer *)
 */
static void render(View *self, Renderer *renderer) {

	super(View, self, render, renderer);

	Control *this = (Control *) self;

	const SDL_Rect frame = $(self, renderFrame);

	if (this->bevel == ControlBevelInset) {

		$(renderer, setDrawColor, &Colors.Silver);

		SDL_Point points[3];

		points[0].x = frame.x;
		points[0].y = frame.y + frame.h;

		points[1].x = frame.x + frame.w;
		points[1].y = frame.y + frame.h;

		points[2].x = frame.x + frame.w;
		points[2].y = frame.y;

		$(renderer, drawLines, points, lengthof(points));

		$(renderer, setDrawColor, &Colors.Black);

		points[0].x = frame.x;
		points[0].y = frame.y + frame.h;

		points[1].x = frame.x;
		points[1].y = frame.y;

		points[2].x = frame.x + frame.w;
		points[2].y = frame.y;

		$(renderer, drawLines, points, lengthof(points));

	} else if (this->bevel == ControlBevelOutset) {

		$(renderer, setDrawColor, &Colors.Black);

		SDL_Point points[3];

		points[0].x = frame.x;
		points[0].y = frame.y + frame.h;

		points[1].x = frame.x + frame.w;
		points[1].y = frame.y + frame.h;

		points[2].x = frame.x + frame.w;
		points[2].y = frame.y;

		$(renderer, drawLines, points, lengthof(points));

		$(renderer, setDrawColor, &Colors.Silver);

		points[0].x = frame.x;
		points[0].y = frame.y + frame.h;

		points[1].x = frame.x;
		points[1].y = frame.y;

		points[2].x = frame.x + frame.w;
		points[2].y = frame.y;

		$(renderer, drawLines, points, lengthof(points));
	}

	if (this->state & ControlStateFocused) {

		$(renderer, setDrawColor, &Colors.Charcoal);

		$(renderer, drawRect, &frame);
	}

	$(renderer, setDrawColor, &Colors.White);
}

/**
 * @see View::respondToEvent(View *, const SDL_Event *)
 */
static void respondToEvent(View *self, const SDL_Event *event) {

	Control *this = (Control *) self;

	const ControlState state = this->state;

	const bool didCaptureEvent = $(this, captureEvent, event);
	if (didCaptureEvent) {

		const Array *actions = (Array *) this->actions;
		for (size_t i = 0; i < actions->count; i++) {

			const Action *action = $(actions, objectAtIndex, i);
			if (action->eventType == event->type) {
				action->function(this, event, action->sender, action->data);
			}
		}
	}

	if (this->state != state) {
		$(this, stateDidChange);
	}

	// FIXME: We want Button events to propagate up, but not motion events..
	if (didCaptureEvent) {
		return;
	}

	super(View, self, respondToEvent, event);
}

#pragma mark - Control

/**
 * @brief Predicate for actionsForEvent.
 */
static bool actionsForEvent_predicate(const ident obj, ident data) {
	return ((Action *) obj)->eventType == ((SDL_Event *) data)->type;
}

/**
 * @fn Array *Control::actionsForEvent(const Control *self, const SDL_Event *event)
 * @memberof Control
 */
static Array *actionsForEvent(const Control *self, const SDL_Event *event) {
	return $((Array *) self->actions, filteredArray, actionsForEvent_predicate, (ident) event);
}

/**
 * @fn void Control::addActionForEventType(Control *self, SDL_EventType eventType, ActionFunction function, ident sender, ident data)
 * @memberof Control
 */
static void addActionForEventType(Control *self, SDL_EventType eventType, ActionFunction function, ident sender, ident data) {

	Action *action = $(alloc(Action), initWithEventType, eventType, function, sender, data);

	$(self->actions, addObject, action);

	release(action);
}

/**
 * @fn bool Control::captureEvent(Control *self, const SDL_Event *event)
 * @memberof Control
 */
static bool captureEvent(Control *self, const SDL_Event *event) {
	return false;
}

/**
 * @fn Control Control::initWithFrame(Control *self, const SDL_Rect *frame)
 * @memberof Control
 */
static Control *initWithFrame(Control *self, const SDL_Rect *frame) {

	self = (Control *) super(View, self, initWithFrame, frame);
	if (self) {

		self->actions = $$(MutableArray, arrayWithCapacity, 0);
		assert(self->actions);
	}

	return self;
}

/**
 * @fn bool Control::isDisabled(const Control *self)
 * @memberof Control
 */
static bool isDisabled(const Control *self) {
	return (self->state & ControlStateDisabled) == ControlStateDisabled;
}

/**
 * @fn bool Control::isFocused(const Control *self)
 * @memberof Control
 */
static bool isFocused(const Control *self) {
	return (self->state & ControlStateFocused) == ControlStateFocused;
}

/**
 * @fn bool Control::isHighlighted(const Control *self)
 * @memberof Control
 */
static bool isHighlighted(const Control *self) {
	return (self->state & ControlStateHighlighted) == ControlStateHighlighted;
}

/**
 * @fn bool Control::isSelected(const Control *self)
 * @memberof Control
 */
static bool isSelected(const Control *self) {
	return (self->state & ControlStateSelected) == ControlStateSelected;
}

/**
 * @fn void Control::stateDidChange(Control *self)
 * @memberof Control
 */
static void stateDidChange(Control *self) {

	View *this = (View *) self;

	if (self->state & (ControlStateHighlighted | ControlStateFocused)) {
		$(this, becomeFirstResponder);
	} else {
		$(this, resignFirstResponder);
	}

	if (self->state & ControlStateFocused) {
		$(this, emitViewEvent, ViewEventFocus, NULL);
	} else {
		$(this, emitViewEvent, ViewEventBlur, NULL);
	}

	$(this, invalidateStyle);

	this->needsLayout = true;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->acceptsFirstResponder = acceptsFirstResponder;
	((ViewInterface *) clazz->interface)->applyStyle = applyStyle;
	((ViewInterface *) clazz->interface)->init = init;
	((ViewInterface *) clazz->interface)->matchesSelector = matchesSelector;
	((ViewInterface *) clazz->interface)->render = render;
	((ViewInterface *) clazz->interface)->respondToEvent = respondToEvent;

	((ControlInterface *) clazz->interface)->actionsForEvent = actionsForEvent;
	((ControlInterface *) clazz->interface)->addActionForEventType = addActionForEventType;
	((ControlInterface *) clazz->interface)->captureEvent = captureEvent;
	((ControlInterface *) clazz->interface)->initWithFrame = initWithFrame;
	((ControlInterface *) clazz->interface)->isDisabled = isDisabled;
	((ControlInterface *) clazz->interface)->isFocused = isFocused;
	((ControlInterface *) clazz->interface)->isHighlighted = isHighlighted;
	((ControlInterface *) clazz->interface)->isSelected = isSelected;
	((ControlInterface *) clazz->interface)->stateDidChange = stateDidChange;
}

/**
 * @fn Class *Control::_Control(void)
 * @memberof Control
 */
Class *_Control(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Control",
			.superclass = _View(),
			.instanceSize = sizeof(Control),
			.interfaceOffset = offsetof(Control, interface),
			.interfaceSize = sizeof(ControlInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
