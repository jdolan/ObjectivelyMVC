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

#include <Objectively/Types.h>

#undef interface

#if defined(_MSC_VER)
 #include "WindowlyMVC.h"
#endif

#ifndef OBJECTIVELYMVC_EXPORT
 #define OBJECTIVELYMVC_EXPORT extern
#endif

/**
 * @file
 * @brief ObjectivelyMVC base types.
 */

typedef struct View View;
typedef struct ViewController ViewController;

/**
 * @brief A function type for View enumeration
 * @param view The View.
 * @param data User data.
 */
typedef void (*ViewEnumerator)(View *view, ident data);

typedef struct SDL_Size SDL_Size;

/**
 * @brief The SDL_Size type.
 */
struct SDL_Size {
	int w, h;
};

/**
 * @brief Creates an SDL_Point with the given coordinates.
 */
#define MakePoint(x, y) (SDL_Point) { (x), (y) }

/**
 * @brief Creates an SDL_Rect with the given origin and size.
 */
#define MakeRect(x, y, w, h) (SDL_Rect) { (x), (y), (w), (h) }

/**
 * @brief Creates an SDL_Size with the given dimensions.
 */
#define MakeSize(w, h) (SDL_Size) { (w), (h) }
