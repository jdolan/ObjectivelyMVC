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
#include <string.h>

#include "Log.h"
#include "RenderDevice.h"

#define _Class _RenderDevice

#pragma mark - RenderDevice

/**
 * @fn void RenderDevice::addDrawable(RenderDevice *self, Drawable *drawable)
 * @memberof RenderDevice
 */
static void addDrawable(RenderDevice *self, Drawable *drawable) {

  assert(drawable);

  $(self->drawables, add, (ident) &drawable);
}

/**
 * @fn SDL_GPUCommandBuffer *RenderDevice::acquireCommandBuffer(const RenderDevice *self)
 * @memberof RenderDevice
 */
static SDL_GPUCommandBuffer *acquireCommandBuffer(const RenderDevice *self) {

  SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(self->device);
  GPU_Assert(cmd, "SDL_AcquireGPUCommandBuffer");
  return cmd;
}

/**
 * @fn bool RenderDevice::acquireSwapchainTexture(const RenderDevice *self, SDL_GPUCommandBuffer *cmd, Swapchain *swapchain)
 * @memberof RenderDevice
 */
static bool acquireSwapchainTexture(const RenderDevice *self, SDL_GPUCommandBuffer *cmd, Swapchain *swapchain) {

  assert(cmd);
  assert(swapchain);

  SDL_AcquireGPUSwapchainTexture(cmd, self->window, &swapchain->texture, &swapchain->size.w, &swapchain->size.h);

  return swapchain->texture != NULL;
}

/**
 * @fn void RenderDevice::submit(const RenderDevice *self, SDL_GPUCommandBuffer *cmd)
 * @memberof RenderDevice
 */
static void submit(const RenderDevice *self, SDL_GPUCommandBuffer *cmd) {

  assert(cmd);

  const bool ok = SDL_SubmitGPUCommandBuffer(cmd);
  GPU_Assert(ok, "SDL_SubmitGPUCommandBuffer");
}

/**
 * @fn SDL_GPUFence *RenderDevice::submitAndFence(const RenderDevice *self, SDL_GPUCommandBuffer *cmd)
 * @memberof RenderDevice
 */
static SDL_GPUFence *submitAndFence(const RenderDevice *self, SDL_GPUCommandBuffer *cmd) {

  assert(cmd);

  SDL_GPUFence *fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
  GPU_Assert(fence, "SDL_SubmitGPUCommandBufferAndAcquireFence");
  return fence;
}

/**
 * @fn SDL_GPUBuffer *RenderDevice::createBuffer(const RenderDevice *self, const SDL_GPUBufferCreateInfo *info)
 * @memberof RenderDevice
 */
static SDL_GPUBuffer *createBuffer(const RenderDevice *self, const SDL_GPUBufferCreateInfo *info) {

  assert(info);
  return SDL_CreateGPUBuffer(self->device, info);
}

/**
 * @fn SDL_GPUTexture *RenderDevice::createTexture(const RenderDevice *self, const SDL_GPUTextureCreateInfo *info, const void *pixels)
 * @memberof RenderDevice
 */
static SDL_GPUTexture *createTexture(const RenderDevice *self, const SDL_GPUTextureCreateInfo *info, const void *pixels) {

  assert(info);

  SDL_GPUTexture *texture = SDL_CreateGPUTexture(self->device, info);
  GPU_Assert(texture, "SDL_CreateGPUTexture");

  if (pixels) {
    const Uint32 bytesPerTexel = SDL_GPUTextureFormatTexelBlockSize(info->format);
    const Uint32 totalBytes = info->width * info->height * info->layer_count_or_depth * bytesPerTexel;

    const SDL_GPUTransferBufferCreateInfo tbufInfo = {
      .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
      .size  = totalBytes,
    };

    SDL_GPUTransferBuffer *tbuf = SDL_CreateGPUTransferBuffer(self->device, &tbufInfo);
    GPU_Assert(tbuf, "SDL_CreateGPUTransferBuffer");

    void *mapped = SDL_MapGPUTransferBuffer(self->device, tbuf, false);
    GPU_Assert(mapped, "SDL_MapGPUTransferBuffer");
    memcpy(mapped, pixels, totalBytes);
    SDL_UnmapGPUTransferBuffer(self->device, tbuf);

    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(self->device);
    GPU_Assert(cmd, "SDL_AcquireGPUCommandBuffer");
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmd);

    const SDL_GPUTextureTransferInfo src = {
      .transfer_buffer = tbuf,
      .offset          = 0,
      .pixels_per_row  = info->width,
      .rows_per_layer  = info->height,
    };
    const SDL_GPUTextureRegion dst = {
      .texture = texture,
      .w       = info->width,
      .h       = info->height,
      .d       = info->layer_count_or_depth,
    };
    SDL_UploadToGPUTexture(copyPass, &src, &dst, false);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(self->device, tbuf);
  }

  return texture;
}

/**
 * @fn SDL_GPUTransferBuffer *RenderDevice::createTransferBuffer(const RenderDevice *self, const SDL_GPUTransferBufferCreateInfo *info)
 * @memberof RenderDevice
 */
static SDL_GPUTransferBuffer *createTransferBuffer(const RenderDevice *self, const SDL_GPUTransferBufferCreateInfo *info) {

  assert(info);
  return SDL_CreateGPUTransferBuffer(self->device, info);
}

/**
 * @fn RenderDevice *RenderDevice::init(RenderDevice *self)
 * @memberof RenderDevice
 */
static RenderDevice *init(RenderDevice *self) {

  self = (RenderDevice *) super(Object, self, init);
  if (self) {
    self->clear = true;
    self->clearColor = (SDL_FColor) { 0.f, 0.f, 0.f, 1.f };
    self->drawables = $(alloc(Vector), initWithSize, sizeof(Drawable *));
    assert(self->drawables);

    const SDL_GPUShaderFormat formats =
      SDL_GPU_SHADERFORMAT_MSL |
      SDL_GPU_SHADERFORMAT_SPIRV |
      SDL_GPU_SHADERFORMAT_DXIL;

    self->device = SDL_CreateGPUDevice(formats, false, NULL);
    GPU_Assert(self->device, "SDL_CreateGPUDevice");
  }

  return self;
}

/**
 * @fn RenderDevice *RenderDevice::initWithWindow(RenderDevice *self, SDL_Window *window)
 * @memberof RenderDevice
 */
static RenderDevice *initWithWindow(RenderDevice *self, SDL_Window *window) {

  self = $(self, init);
  if (self) {
    $(self, setWindow, window);
  }
  return self;
}

/**
 * @fn void RenderDevice::removeDrawable(RenderDevice *self, Drawable *drawable)
 * @memberof RenderDevice
 */
static void removeDrawable(RenderDevice *self, Drawable *drawable) {

  for (size_t i = 0; i < self->drawables->count; i++) {
    if (*VectorElement(self->drawables, Drawable *, i) == drawable) {
      $(self->drawables, removeAt, i);
      return;
    }
  }
}

/**
 * @fn void RenderDevice::setWindow(RenderDevice *self, SDL_Window *window)
 * @memberof RenderDevice
 */
static void setWindow(RenderDevice *self, SDL_Window *window) {

  if (self->window == window) {
    return;
  }

  if (self->window) {
    for (size_t i = 0; i < self->drawables->count; i++) {
      Drawable *draw = *VectorElement(self->drawables, Drawable *, i);
      if (draw->renderDeviceWillReset) {
        draw->renderDeviceWillReset(draw);
      }
    }
    SDL_ReleaseWindowFromGPUDevice(self->device, self->window);
  }

  self->window = window;

  if (window) {
    const bool claimed = SDL_ClaimWindowForGPUDevice(self->device, window);
    GPU_Assert(claimed, "SDL_ClaimWindowForGPUDevice");
    for (size_t i = 0; i < self->drawables->count; i++) {
      Drawable *draw = *VectorElement(self->drawables, Drawable *, i);
      if (draw->renderDeviceDidReset) {
        draw->renderDeviceDidReset(draw, self->device);
      }
    }
  }
}

#pragma mark - Object lifecycle

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  RenderDevice *this = (RenderDevice *) self;

  for (size_t i = 0; i < this->drawables->count; i++) {
    Drawable *d = *VectorElement(this->drawables, Drawable *, i);
    if (d->renderDeviceWillReset) {
      d->renderDeviceWillReset(d);
    }
  }

  if (this->window && this->device) {
    SDL_ReleaseWindowFromGPUDevice(this->device, this->window);
  }

  if (this->device) {
    SDL_DestroyGPUDevice(this->device);
  }

  release(this->drawables);

  super(Object, self, dealloc);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((RenderDeviceInterface *) clazz->interface)->addDrawable = addDrawable;
  ((RenderDeviceInterface *) clazz->interface)->acquireCommandBuffer = acquireCommandBuffer;
  ((RenderDeviceInterface *) clazz->interface)->acquireSwapchainTexture = acquireSwapchainTexture;
  ((RenderDeviceInterface *) clazz->interface)->submit = submit;
  ((RenderDeviceInterface *) clazz->interface)->submitAndFence = submitAndFence;
  ((RenderDeviceInterface *) clazz->interface)->createBuffer = createBuffer;
  ((RenderDeviceInterface *) clazz->interface)->createTexture = createTexture;
  ((RenderDeviceInterface *) clazz->interface)->createTransferBuffer = createTransferBuffer;
  ((RenderDeviceInterface *) clazz->interface)->init = init;
  ((RenderDeviceInterface *) clazz->interface)->initWithWindow = initWithWindow;
  ((RenderDeviceInterface *) clazz->interface)->removeDrawable = removeDrawable;
  ((RenderDeviceInterface *) clazz->interface)->setWindow = setWindow;
}

/**
 * @fn Class *RenderDevice::_RenderDevice(void)
 * @memberof RenderDevice
 */
Class *_RenderDevice(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name            = "RenderDevice",
      .superclass      = _Object(),
      .instanceSize    = sizeof(RenderDevice),
      .interfaceOffset = offsetof(RenderDevice, interface),
      .interfaceSize   = sizeof(RenderDeviceInterface),
      .initialize      = initialize,
    });
  });

  return clazz;
}

#undef _Class
