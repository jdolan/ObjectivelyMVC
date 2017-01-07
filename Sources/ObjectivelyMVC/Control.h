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

#pragma once

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/Action.h>
#include <ObjectivelyMVC/Text.h>

/**
 * @file
 * @brief Controls are Views which capture events and dispatch Actions.
 */

/**
 * @defgroup Controls Controls
 * @brief Controls capture input events and dispatch Actions.
 */

/**
 * @brief Bevel types.
 */
typedef enum {
	ControlBevelTypeNone,
	ControlBevelTypeInset,
	ControlBevelTypeOutset
} ControlBevelType;

OBJECTIVELYMVC_EXPORT const EnumName ControlBevelTypeNames[];

/**
 * @brief Control selection styles, for Controls that support user selection.
 */
typedef enum {
	ControlSelectionNone,
	ControlSelectionSingle,
	ControlSelectionMultiple
} ControlSelection;

OBJECTIVELYMVC_EXPORT const EnumName ControlSelectionNames[];

/**
 * @brief Control states, which are bit-masked.
 */
typedef enum {
	ControlStateDefault = 0x0,
	ControlStateHighlighted = 0x1,
	ControlStateDisabled = 0x2,
	ControlStateSelected = 0x4,
	ControlStateFocused = 0x8,
} ControlState;

OBJECTIVELYMVC_EXPORT const EnumName ControlStateNames[];

/**
 * @brief Control styles.
 */
typedef enum {
	ControlStyleDefault,
	ControlStyleCustom,
} ControlStyle;

#define DEFAULT_CONTROL_HEIGHT 32
#define DEFAULT_CONTROL_PADDING 8

typedef struct ControlInterface ControlInterface;

/**
 * @brief Controls are Views which capture events and dispatch Actions.
 * @extends View
 * @ingroup Controls
 */
struct Control {

	/**
	 * @brief The superclass.
	 */
	View view;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ControlInterface *interface;

	/**
	 * @brief The Actions bound to this Control.
	 * @private
	 */
	MutableArray *actions;
	
	/**
	 * @brief The ControlBevelType.
	 */
	ControlBevelType bevel;

	/**
	 * @brief The bit mask of ControlState.
	 */
	int state;

	/**
	 * @brief The ControlSelection.
	 */
	ControlSelection selection;
	
	/**
	 * @brief The ControlStyle.
	 */
	ControlStyle style;
};

/**
 * @brief The Control interface.
 */
struct ControlInterface {

	/**
	 * @brief The superclass interface.
	 */
	ViewInterface viewInterface;

	/**
	 * @fn Action *Control::actionForEvent(const Control *self, const SDL_Event *event)
	 * @param self The Control.
	 * @param event An SDL_Event.
	 * @memberof Control
	 */
	Action *(*actionForEvent)(const Control *self, const SDL_Event *event);

	/**
	 * @fn void Control::addActionForEventType(Control *self, SDL_EventType eventType, ActionFunction function, ident sender, ident data)
	 * @brief Adds an Action for the given event type to this Control.
	 * @param self The Control.
	 * @param eventType The event type.
	 * @param function The ActionFunction.
	 * @param sender The sender.
	 * @param data The data.
	 * @memberof Control
	 */
	void (*addActionForEventType)(Control *self, SDL_EventType eventType, ActionFunction function, ident sender, ident data);

	/**
	 * @fn _Bool Control::captureEvent(Control *self, const SDL_Event *event)
	 * @brief Captures a given event, potentially altering the state of this Control.
	 * @param self The Control.
	 * @param event The event.
	 * @return True if the Event was captured, false otherwise.
	 * @remarks Subclasses should override this method to capture events.
	 * @memberof Control
	 */
	_Bool (*captureEvent)(Control *self, const SDL_Event *event);
	
	/**
	 * @fn _Bool Control::enabled(const Control *self)
	 * @param self The Control.
	 * @return True if this Control is enabled, false otherwise.
	 * @memberof Control
	 */
	_Bool (*enabled)(const Control *self);

	/**
	 * @fn _Bool Control::focused(const Control *self)
	 * @param self The Control.
	 * @return True if this Control is focused, false otherwise.
	 * @memberof Control
	 */
	_Bool (*focused)(const Control *self);

	/**
	 * @fn _Bool Control::highlighted(const Control *self)
	 * @param self The Control.
	 * @return True if this Control is highlighted, false otherwise.
	 * @memberof Control
	 */
	_Bool (*highlighted)(const Control *self);

	/**
	 * @fn Control Control::initWithFrame(Control *self, const SDL_Rect *frame, ControlStyle style)
	 * @brief Initializes this Control with the specified frame and style.
	 * @param self The Control.
	 * @param frame The frame.
	 * @param style The ControlStyle.
	 * @return The intialized Control, or `NULL` on error.
	 * @memberof Control
	 */
	Control *(*initWithFrame)(Control *self, const SDL_Rect *frame, ControlStyle style);

	/**
	 * @fn _Bool Control::selected(const Control *self)
	 * @param self The Control.
	 * @return True if this Control is selected, false otherwise.
	 * @memberof Control
	 */
	_Bool (*selected)(const Control *self);
	
	/**
	 * @fn void Control::stateDidChange(Control *self)
	 * @brief Called when the state of this Control changes.
	 * @param self The Control.
	 * @remarks Subclasses may implement this method to handle discrete state changes.
	 * @memberof Control
	 */
	void (*stateDidChange)(Control *self);
};

/**
 * @fn Class *Control::_Control(void)
 * @brief The Control archetype.
 * @return The Control Class.
 * @memberof Control
 */
OBJECTIVELYMVC_EXPORT Class *_Control(void);
