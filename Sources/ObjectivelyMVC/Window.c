/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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

#include "Window.h"

SDL_Rect MVC_TransformToWindow(SDL_Window *window, const SDL_Rect *rect) {

  assert(rect);
  SDL_Rect transformed = *rect;
  
  SDL_Rect safeArea;
  SDL_GetWindowSafeArea(window, &safeArea);
  
  transformed.x += safeArea.x;
  transformed.y += safeArea.y;

  // Flip Y because OpenGL uses bottom-left coordinates
  transformed.y = safeArea.h - transformed.h - transformed.y;

  const float pixelDensity = SDL_GetWindowPixelDensity(window);

  transformed.x *= pixelDensity;
  transformed.y *= pixelDensity;
  transformed.w *= pixelDensity;
  transformed.h *= pixelDensity;

  return transformed;
}
