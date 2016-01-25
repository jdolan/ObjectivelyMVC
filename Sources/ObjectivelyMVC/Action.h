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

#ifndef _ObjectivelyMVC_Action_h_
#define _ObjectivelyMVC_Action_h_

#include <SDL2/SDL_events.h>

#include <Objectively/Object.h>

/**
 * @file
 * 
 * @brief Actions bind event-driven behavior to Controls.
 */

typedef struct Action Action;
typedef struct ActionInterface ActionInterface;

/**
 * @brief The ActionFunction callback.
 */
typedef void (*ActionFunction)(ident sender, const SDL_Event *event, ident data);

/**
 * @brief The Action type.
 *
 * @extends Object
 */
struct Action {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	ActionInterface *interface;
	
	/**
	 * @brief The event type.
	 */
	SDL_EventType eventType;

	/**
	 * @brief The function.
	 */
	ActionFunction function;

	/**
	 * @brief The data.
	 */
	ident data;
};

/**
 * @brief The Action interface.
 */
struct ActionInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn Action *Action::initWithFunction(Action *self, SDL_EventType eventType, ActionFunction function, ident data)
	 *
	 * @brief Initializes this Action with the given function and data.
	 *
	 * @param eventType The event type.
	 * @param function The ActionFunction.
	 * @param data The data.
	 *
	 * @return The initialized Action, or `NULL` on error.
	 *
	 * @memberof Action
	 */
	Action *(*initWithEventType)(Action *self, SDL_EventType eventType, ActionFunction function, ident data);
};

/**
 * @brief The Action Class.
 */
extern Class _Action;

#endif