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

#include <ObjectivelyMVC/Control.h>

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
static _Bool acceptsFirstResponder(const View *self) {
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
 * @see View::matchesSelector(Const View *, const SimpleSelector *)
 */
static _Bool matchesSelector(const View *self, const SimpleSelector *simpleSelector) {

	assert(simpleSelector);

	const Control *this = (Control *) self;

	switch (simpleSelector->type) {
		case SimpleSelectorTypePseudo:
			if (strcmp("highlighted", simpleSelector->pattern) == 0) {
				return $(this, highlighted);
			} else if (strcmp("disabled", simpleSelector->pattern) == 0) {
				return $(this, disabled);
			} else if (strcmp("selected", simpleSelector->pattern) == 0) {
				return $(this, selected);
			} else if (strcmp("focused", simpleSelector->pattern) == 0) {
				return $(this, focused);
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

	const _Bool didCaptureEvent = $(this, captureEvent, event);
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

	if (didCaptureEvent) {
		return;
	}

	super(View, self, respondToEvent, event);
}

#pragma mark - Control

/**
 * @brief Predicate for actionsForEvent.
 */
static _Bool actionsForEvent_predicate(const ident obj, ident data) {
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
 * @fn _Bool Control::captureEvent(Control *self, const SDL_Event *event)
 * @memberof Control
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {
	return false;
}

/**
 * @fn _Bool Control::disabled(const Control *self)
 * @memberof Control
 */
static _Bool disabled(const Control *self) {
	return (self->state & ControlStateDisabled) == ControlStateDisabled;
}

/**
 * @fn _Bool Control::focused(const Control *self)
 * @memberof Control
 */
static _Bool focused(const Control *self) {
	return (self->state & ControlStateFocused) == ControlStateFocused;
}

/**
 * @fn _Bool Control::highlighted(const Control *self)
 * @memberof Control
 */
static _Bool highlighted(const Control *self) {
	return (self->state & ControlStateHighlighted) == ControlStateHighlighted;
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
 * @fn _Bool Control::selected(const Control *self)
 * @memberof Control
 */
static _Bool selected(const Control *self) {
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

	$(this, invalidateStyle);

	this->needsLayout = true;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->acceptsFirstResponder = acceptsFirstResponder;
	((ViewInterface *) clazz->def->interface)->applyStyle = applyStyle;
	((ViewInterface *) clazz->def->interface)->init = init;
	((ViewInterface *) clazz->def->interface)->matchesSelector = matchesSelector;
	((ViewInterface *) clazz->def->interface)->render = render;
	((ViewInterface *) clazz->def->interface)->respondToEvent = respondToEvent;

	((ControlInterface *) clazz->def->interface)->actionsForEvent = actionsForEvent;
	((ControlInterface *) clazz->def->interface)->addActionForEventType = addActionForEventType;
	((ControlInterface *) clazz->def->interface)->captureEvent = captureEvent;
	((ControlInterface *) clazz->def->interface)->disabled = disabled;
	((ControlInterface *) clazz->def->interface)->focused = focused;
	((ControlInterface *) clazz->def->interface)->highlighted = highlighted;
	((ControlInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((ControlInterface *) clazz->def->interface)->selected = selected;
	((ControlInterface *) clazz->def->interface)->stateDidChange = stateDidChange;
}

/**
 * @fn Class *Control::_Control(void)
 * @memberof Control
 */
Class *_Control(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Control";
		clazz.superclass = _View();
		clazz.instanceSize = sizeof(Control);
		clazz.interfaceOffset = offsetof(Control, interface);
		clazz.interfaceSize = sizeof(ControlInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
