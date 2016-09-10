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

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>

typedef struct SDL_Size SDL_Size;

/**
 * @brief The Size type.
 */
struct SDL_Size {
	int w, h;
};

/**
 * @brief Creates an SDL_Size with the given dimensions.
 */
#define MakeSize(w, h) \
	({ SDL_Size _size = { (w), (h) }; _size; })


/**
 * @brief Creates an SDL_Rect with the given origin and size.
 */
#define MakeRect(x, y, w, h) \
	({ SDL_Rect _rect = { (x), (y), (w), (h) }; _rect; })

/**
 * @remarks Push this SDL_EventType to prompt `View::renderDeviceReset`.
 */
extern Uint32 MVC_EVENT_RENDER_DEVICE_RESET;

/**
 * @remarks Push this SDL_EventType to prompt `View::updateBindings`.
 */
extern Uint32 MVC_EVENT_UPDATE_BINDINGS;
