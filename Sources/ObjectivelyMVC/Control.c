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
	MakeEnumName(ControlBevelNone),
	MakeEnumName(ControlBevelInset),
	MakeEnumName(ControlBevelOutset)
);

const EnumName ControlSelectionNames[] = MakeEnumNames(
	MakeEnumName(ControlSelectionNone),
	MakeEnumName(ControlSelectionSingle),
	MakeEnumName(ControlSelectionMultiple)
);

const EnumName ControlStateNames[] = MakeEnumNames(
	MakeEnumName(ControlStateDefault),
	MakeEnumName(ControlStateHighlighted),
	MakeEnumName(ControlStateSelected),
	MakeEnumName(ControlStateFocused)
);

const EnumName ControlStyleNames[] = MakeEnumNames(
	MakeEnumName(ControlStyleDefault),
	MakeEnumName(ControlStyleCustom)
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
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	Control *this = (Control *) self;

	const Inlet inlets[] = MakeInlets(
	    MakeInlet("bevel", InletTypeEnum, &this->bevel, (ident) (ControlBevelNames)),
		MakeInlet("selection", InletTypeEnum, &this->selection, (ident) ControlSelectionNames),
		MakeInlet("style", InletTypeEnum, &this->style, (ident) ControlStyleNames)
	);

	$(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((Control *) self, initWithFrame, NULL, ControlStyleDefault);
}

/**
 * @see View::matchesSelector(Const View *, const SimpleSelector *)
 */
static _Bool matchesSelector(const View *self, const SimpleSelector *simpleSelector) {

	assert(simpleSelector);

	const Control *this = (Control *) self;
	const char *pattern = simpleSelector->pattern;

	switch (simpleSelector->type) {
		case SimpleSelectorTypePseudo:
			if (strcmp("highlighted", pattern) == 0) {
				return $(this, highlighted);
			} else if (strcmp("disabled", pattern) == 0) {
				return $(this, disabled);
			} else if (strcmp("selected", pattern) == 0) {
				return $(this, selected);
			} else if (strcmp("focused", pattern) == 0) {
				return $(this, focused);
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

		points[0].x = frame.x + 1;
		points[0].y = frame.y + frame.h - 1;

		points[1].x = frame.x + frame.w - 1;
		points[1].y = frame.y + frame.h - 1;

		points[2].x = frame.x + frame.w - 1;
		points[2].y = frame.y + 1;

		$(renderer, drawLines, points, lengthof(points));

		$(renderer, setDrawColor, &Colors.Charcoal);

		points[0].x = frame.x + 1;
		points[0].y = frame.y + frame.h - 1;

		points[1].x = frame.x + 1;
		points[1].y = frame.y + 1;

		points[2].x = frame.x + frame.w - 1;
		points[2].y = frame.y + 1;

		$(renderer, drawLines, points, lengthof(points));

	} else if (this->bevel == ControlBevelOutset) {

		$(renderer, setDrawColor, &Colors.Charcoal);

		SDL_Point points[3];

		points[0].x = frame.x + 1;
		points[0].y = frame.y + frame.h - 1;

		points[1].x = frame.x + frame.w - 1;
		points[1].y = frame.y + frame.h - 1;

		points[2].x = frame.x + frame.w - 1;
		points[2].y = frame.y + 1;

		$(renderer, drawLines, points, lengthof(points));

		$(renderer, setDrawColor, &Colors.Silver);

		points[0].x = frame.x + 1;
		points[0].y = frame.y + frame.h - 1;

		points[1].x = frame.x + 1;
		points[1].y = frame.y + 1;

		points[2].x = frame.x + frame.w - 1;
		points[2].y = frame.y + 1;

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

		Action *action = $(this, actionForEvent, event);
		if (action) {
			action->function(this, event, action->sender, action->data);
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
 * @fn Action *Controll::actionForEvent(const Control *self, const SDL_Event *event)
 * @memberof Control
 */
static Action *actionForEvent(const Control *self, const SDL_Event *event) {

	Array *actions = (Array *) self->actions;
	for (size_t i = 0; i < actions->count; i++) {

		Action *action = (Action *) $(actions, objectAtIndex, i);
		if (action->eventType == event->type) {
			return action;
		}
	}

	return NULL;
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
 * @fn Control Control::initWithFrame(Control *self, const SDL_Rect *frame, ControlStyle style)
 * @memberof Control
 */
static Control *initWithFrame(Control *self, const SDL_Rect *frame, ControlStyle style) {

	self = (Control *) super(View, self, initWithFrame, frame);
	if (self) {

		self->actions = $$(MutableArray, array);
		assert(self->actions);

		self->style = style;
		if (self->style == ControlStyleDefault) {

			if (self->view.frame.h == 0) {
				self->view.frame.h = DEFAULT_CONTROL_HEIGHT;
			}

			self->view.padding.top = DEFAULT_CONTROL_PADDING;
			self->view.padding.right = DEFAULT_CONTROL_PADDING;
			self->view.padding.bottom = DEFAULT_CONTROL_PADDING;
			self->view.padding.left = DEFAULT_CONTROL_PADDING;
		}
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

	if (self->state & (ControlStateHighlighted | ControlStateFocused)) {
		$((View *) self, becomeFirstResponder);
	} else {
		$((View *) self, resignFirstResponder);
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->acceptsFirstResponder = acceptsFirstResponder;
	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->init = init;
	((ViewInterface *) clazz->def->interface)->matchesSelector = matchesSelector;
	((ViewInterface *) clazz->def->interface)->render = render;
	((ViewInterface *) clazz->def->interface)->respondToEvent = respondToEvent;

	((ControlInterface *) clazz->def->interface)->actionForEvent = actionForEvent;
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
