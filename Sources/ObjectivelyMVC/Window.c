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

#include <ObjectivelyMVC/Window.h>

SDL_Rect MVC_TransformToWindow(SDL_Window *window, const SDL_Rect *rect) {

	assert(rect);

	SDL_Rect transformed = *rect;

	int dh = 0;
	const double scale = MVC_WindowScale(window, NULL, &dh);

	transformed.x *= scale;
	transformed.y *= scale;
	transformed.w *= scale;
	transformed.h *= scale;

	transformed.y = dh - transformed.h - transformed.y;

	return transformed;
}

double MVC_WindowScale(SDL_Window *window, int *height, int *drawableHeight) {

	window = window ?: SDL_GL_GetCurrentWindow();
	assert(window);

	int h;
	SDL_GetWindowSize(window, NULL, &h);

	if (height) {
		*height = h;
	}

	if (h) {

		int dh;
		SDL_GL_GetDrawableSize(window, NULL, &dh);

		if (drawableHeight) {
			*drawableHeight = dh;
		}

		if (dh) {
			return dh / (double) h;
		}
	}

	return 1.0;
}
