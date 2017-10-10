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

#include <ObjectivelyMVC/View.h>

/**
 * @file
 * @brief The Theme type.
 */

/**
 * @brief The Theme provides color, font and spacing hints for Views.
 */
OBJECTIVELYMVC_EXPORT struct _Theme {

	SDL_Color alternateBackgroundColor;

	SDL_Color backgroundColor;

	SDL_Color borderColor;

	ViewPadding containerPadding;

	int containerSpacing;

	int controlHeight;

	ViewPadding controlPadding;

	SDL_Color darkBackgroundColor;

	SDL_Color darkBorderColor;

	SDL_Color darkForegroundColor;

	SDL_Color disabledbackgroundColor;

	SDL_Color disabledForegroundColor;

	SDL_Color focusedBackgroundColor;

	SDL_Color focusedForegroundColor;

	SDL_Color foregroundColor;

	SDL_Color highlightedBackgroundColor;

	SDL_Color highlightedForegroundColor;

	SDL_Color lightBackgroundColor;

	SDL_Color lightBorderColor;

	SDL_Color lightForegroundColor;

	SDL_Color selectedBackgroundColor;

	SDL_Color selectedForegroundColor;

	SDL_Color selectionBackgroundColor;

	SDL_Color selectionForegroundColor;

} Theme;
