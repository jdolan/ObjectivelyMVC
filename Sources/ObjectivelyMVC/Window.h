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

#include <ObjectivelyMVC/Types.h>

/**
 * @brief Transforms the specified rectangle to normalized device coordinates in `window`.
 * @param window The window.
 * @param rect A rectangle defined in object space.
 * @return The transformed rectangle.
 */
OBJECTIVELYMVC_EXPORT SDL_Rect MVC_TransformToWindow(SDL_Window *window, const SDL_Rect *rect);

/**
 * @brief Resolves the scale factor of the specified window for High-DPI support.
 * @param window The window, or `NULL` for the current OpenGL window.
 * @param height An optional output parameter to retrieve the window height.
 * @param drawableHeight AN optional output parameter to retrieve the window drawable height.
 * @return The scale factor of the specified window.
 * @remarks Views and other classes should invoke this method to alter their rendering behavior for
 * High-DPI displays. This is particularly relevant for Views that render textures.
 */
OBJECTIVELYMVC_EXPORT double MVC_WindowScale(SDL_Window *window, int *height, int *drawableHeight);
