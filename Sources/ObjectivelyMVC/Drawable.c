/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 GPU, SDL3 and GNU C.
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

#include "Drawable.h"

#define _Class _Drawable

#pragma mark - Drawable

/**
 * @fn void Drawable::copy(Drawable *self, SDL_GPUCopyPass *copyPass)
 * @memberof Drawable
 */
static void copy(Drawable *self, SDL_GPUCopyPass *copyPass) {
}

/**
 * @fn Drawable *Drawable::init(Drawable *self)
 * @memberof Drawable
 */
static Drawable *init(Drawable *self) {

  self = (Drawable *) super(Object, self, init);
  if (self) {
    self->dirty = true;
  }
  return self;
}

/**
 * @fn void Drawable::renderDeviceDidReset(Drawable *self, SDL_GPUDevice *device)
 * @memberof Drawable
 */
static void renderDeviceDidReset(Drawable *self, SDL_GPUDevice *device) {
}

/**
 * @fn void Drawable::renderDeviceWillReset(Drawable *self)
 * @memberof Drawable
 */
static void renderDeviceWillReset(Drawable *self) {
}

/**
 * @fn void Drawable::submit(Drawable *self, SDL_GPUCommandBuffer *cmd, SDL_GPURenderPass *renderPass)
 * @memberof Drawable
 */
static void submit(Drawable *self, SDL_GPUCommandBuffer *cmd, SDL_GPURenderPass *renderPass) {
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((DrawableInterface *) clazz->interface)->copy                 = copy;
  ((DrawableInterface *) clazz->interface)->init                 = init;
  ((DrawableInterface *) clazz->interface)->renderDeviceDidReset = renderDeviceDidReset;
  ((DrawableInterface *) clazz->interface)->renderDeviceWillReset = renderDeviceWillReset;
  ((DrawableInterface *) clazz->interface)->submit               = submit;
}

/**
 * @fn Class *Drawable::_Drawable(void)
 * @memberof Drawable
 */
Class *_Drawable(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name           = "Drawable",
      .superclass     = _Object(),
      .instanceSize   = sizeof(Drawable),
      .interfaceOffset = offsetof(Drawable, interface),
      .interfaceSize  = sizeof(DrawableInterface),
      .initialize     = initialize,
    });
  });

  return clazz;
}

#undef _Class
