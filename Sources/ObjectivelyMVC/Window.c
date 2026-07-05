/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and C.
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
#include <math.h>

#include "Window.h"

SDL_Rect MVC_TransformToWindow(SDL_Window *window, const SDL_Rect *rect) {

  assert(rect);

  const float pixelDensity = SDL_GetWindowPixelDensity(window);

  // SDL_gpu uses top-left origin (same as SDL), so no Y-flip is required.
  const float x = SDL_roundf(rect->x * pixelDensity);
  const float y = SDL_roundf(rect->y * pixelDensity);
  const float w = SDL_roundf(rect->w * pixelDensity);
  const float h = SDL_roundf(rect->h * pixelDensity);

  return (SDL_Rect) {
    .x = (int) x,
    .y = (int) y,
    .w = (int) w,
    .h = (int) h,
  };
}
