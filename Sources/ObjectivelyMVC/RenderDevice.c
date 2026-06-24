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
  $(self->drawables, addObject, drawable);
}

/**
 * @fn void RenderDevice::beginFrame(RenderDevice *self)
 * @memberof RenderDevice
 */
static void beginFrame(RenderDevice *self) {

  assert(self->device);
  assert(self->window);

  self->cmd = SDL_AcquireGPUCommandBuffer(self->device);
  GPU_Assert(self->cmd, "SDL_AcquireGPUCommandBuffer");

  if (!SDL_AcquireGPUSwapchainTexture(self->cmd, self->window,
      &self->swapchain.texture, &self->swapchain.size.w, &self->swapchain.size.h)) {
    SDL_CancelGPUCommandBuffer(self->cmd);
    self->cmd = NULL;
  }
}

/**
 * @fn SDL_GPUBuffer *RenderDevice::createBuffer(const RenderDevice *self, SDL_GPUBufferUsageFlags usage, Uint32 size)
 * @memberof RenderDevice
 */
static SDL_GPUBuffer *createBuffer(const RenderDevice *self, SDL_GPUBufferUsageFlags usage, Uint32 size) {

  const SDL_GPUBufferCreateInfo info = { .usage = usage, .size = size };
  return SDL_CreateGPUBuffer(self->device, &info);
}

/**
 * @fn SDL_GPUTexture *RenderDevice::createTexture(const RenderDevice *self, const SDL_Surface *surface)
 * @memberof RenderDevice
 */
static SDL_GPUTexture *createTexture(const RenderDevice *self, const SDL_Surface *surface) {

  assert(surface);

  SDL_Surface *rgba = NULL;
  const bool isAlphaMask = (SDL_BYTESPERPIXEL(surface->format) == 1);

  if (isAlphaMask) {
    rgba = SDL_CreateSurface(surface->w, surface->h, SDL_PIXELFORMAT_RGBA32);
    GPU_Assert(rgba, "SDL_CreateSurface");

    const Uint8 *src = (const Uint8 *) surface->pixels;
    Uint32 *dst = (Uint32 *) rgba->pixels;
    for (int y = 0; y < surface->h; y++) {
      for (int x = 0; x < surface->w; x++) {
        const Uint8 a = src[y * surface->pitch + x];
        dst[y * rgba->w + x] = SDL_MapRGBA(
          SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32), NULL, 255, 255, 255, a);
      }
    }
  } else if (surface->format != SDL_PIXELFORMAT_RGBA32) {
    rgba = SDL_ConvertSurface((SDL_Surface *) surface, SDL_PIXELFORMAT_RGBA32);
    GPU_Assert(rgba, "SDL_ConvertSurface");
  }

  const SDL_Surface *upload = rgba ? rgba : surface;

  const SDL_GPUTextureCreateInfo texInfo = {
    .type                 = SDL_GPU_TEXTURETYPE_2D,
    .format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
    .usage                = SDL_GPU_TEXTUREUSAGE_SAMPLER,
    .width                = (Uint32) upload->w,
    .height               = (Uint32) upload->h,
    .layer_count_or_depth = 1,
    .num_levels           = 1,
  };

  SDL_GPUTexture *texture = SDL_CreateGPUTexture(self->device, &texInfo);
  GPU_Assert(texture, "SDL_CreateGPUTexture");

  const Uint32 pixelSize = (Uint32) (upload->w * upload->h * 4);

  const SDL_GPUTransferBufferCreateInfo tbufInfo = {
    .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    .size  = pixelSize,
  };

  SDL_GPUTransferBuffer *tbuf = SDL_CreateGPUTransferBuffer(self->device, &tbufInfo);
  GPU_Assert(tbuf, "SDL_CreateGPUTransferBuffer");

  void *mapped = SDL_MapGPUTransferBuffer(self->device, tbuf, false);
  memcpy(mapped, upload->pixels, pixelSize);
  SDL_UnmapGPUTransferBuffer(self->device, tbuf);

  if (rgba) {
    SDL_DestroySurface(rgba);
  }

  SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(self->device);
  GPU_Assert(cmd, "SDL_AcquireGPUCommandBuffer");
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmd);

  const SDL_GPUTextureTransferInfo src = {
    .transfer_buffer = tbuf,
    .offset          = 0,
    .pixels_per_row  = (Uint32) upload->w,
    .rows_per_layer  = (Uint32) upload->h,
  };
  const SDL_GPUTextureRegion dst = {
    .texture = texture,
    .w       = (Uint32) upload->w,
    .h       = (Uint32) upload->h,
    .d       = 1,
  };
  SDL_UploadToGPUTexture(copyPass, &src, &dst, false);

  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(cmd);

  SDL_ReleaseGPUTransferBuffer(self->device, tbuf);

  return texture;
}

/**
 * @fn SDL_GPUTransferBuffer *RenderDevice::createTransferBuffer(const RenderDevice *self, SDL_GPUTransferBufferUsage usage, Uint32 size)
 * @memberof RenderDevice
 */
static SDL_GPUTransferBuffer *createTransferBuffer(const RenderDevice *self, SDL_GPUTransferBufferUsage usage, Uint32 size) {

  const SDL_GPUTransferBufferCreateInfo info = { .usage = usage, .size = size };
  return SDL_CreateGPUTransferBuffer(self->device, &info);
}

/**
 * @fn void RenderDevice::endFrame(RenderDevice *self)
 * @memberof RenderDevice
 */
static void endFrame(RenderDevice *self) {

  if (!self->cmd || !self->swapchain.texture) {
    return;
  }

  bool anyDirty = false;
  for (size_t i = 0; i < self->drawables->count; i++) {
    if (((Drawable *) $(self->drawables, objectAtIndex, i))->dirty) {
      anyDirty = true;
      break;
    }
  }

  if (anyDirty) {
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(self->cmd);

    for (size_t i = 0; i < self->drawables->count; i++) {
      Drawable *d = $(self->drawables, objectAtIndex, i);
      if (d->dirty) {
        $(d, copy, copyPass);
        d->dirty = false;
      }
    }

    SDL_EndGPUCopyPass(copyPass);
  }

  const SDL_GPUColorTargetInfo colorTarget = {
    .texture     = self->swapchain.texture,
    .load_op     = self->clear ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD,
    .store_op    = SDL_GPU_STOREOP_STORE,
    .clear_color = self->clearColor,
  };

  SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(self->cmd, &colorTarget, 1, NULL);
  for (size_t i = 0; i < self->drawables->count; i++) {
    Drawable *d = $(self->drawables, objectAtIndex, i);
    $(d, submit, self->cmd, renderPass);
  }
  SDL_EndGPURenderPass(renderPass);
  SDL_SubmitGPUCommandBuffer(self->cmd);

  self->cmd = NULL;
  self->swapchain = (Swapchain) { 0 };
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
    self->drawables = $$(Array, array);
    assert(self->drawables);
  }

  return self;
}

/**
 * @fn void RenderDevice::removeDrawable(RenderDevice *self, Drawable *drawable)
 * @memberof RenderDevice
 */
static void removeDrawable(RenderDevice *self, Drawable *drawable) {
  $(self->drawables, removeObject, drawable);
}

/**
 * @fn void RenderDevice::renderDeviceDidReset(RenderDevice *self)
 * @memberof RenderDevice
 */
static void renderDeviceDidReset(RenderDevice *self) {

  $(self, renderDeviceWillReset);

  if (!self->window) {
    MVC_LogError("renderDeviceDidReset: no window set\n");
    return;
  }

  const SDL_GPUShaderFormat formats =
    SDL_GPU_SHADERFORMAT_MSL |
    SDL_GPU_SHADERFORMAT_SPIRV |
    SDL_GPU_SHADERFORMAT_DXIL;

  self->device = SDL_CreateGPUDevice(formats, false, NULL);
  GPU_Assert(self->device, "SDL_CreateGPUDevice");

  const bool claimed = SDL_ClaimWindowForGPUDevice(self->device, self->window);
  GPU_Assert(claimed, "SDL_ClaimWindowForGPUDevice");

  for (size_t i = 0; i < self->drawables->count; i++) {
    Drawable *d = $(self->drawables, objectAtIndex, i);
    $(d, renderDeviceDidReset, self->device);
  }
}

/**
 * @fn void RenderDevice::renderDeviceWillReset(RenderDevice *self)
 * @memberof RenderDevice
 */
static void renderDeviceWillReset(RenderDevice *self) {

  if (!self->device) {
    return;
  }

  for (size_t i = 0; i < self->drawables->count; i++) {
    Drawable *d = $(self->drawables, objectAtIndex, i);
    $(d, renderDeviceWillReset);
  }

  if (self->window) {
    SDL_ReleaseWindowFromGPUDevice(self->device, self->window);
  }

  SDL_DestroyGPUDevice(self->device);
  self->device = NULL;
  self->cmd = NULL;
  self->swapchain = (Swapchain) { 0 };
}

/**
 * @fn void RenderDevice::setWindow(RenderDevice *self, SDL_Window *window)
 * @memberof RenderDevice
 */
static void setWindow(RenderDevice *self, SDL_Window *window) {
  self->window = window;
}

#pragma mark - Object lifecycle

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  RenderDevice *this = (RenderDevice *) self;

  $(this, renderDeviceWillReset);
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
  ((RenderDeviceInterface *) clazz->interface)->beginFrame = beginFrame;
  ((RenderDeviceInterface *) clazz->interface)->createBuffer = createBuffer;
  ((RenderDeviceInterface *) clazz->interface)->createTexture = createTexture;
  ((RenderDeviceInterface *) clazz->interface)->createTransferBuffer = createTransferBuffer;
  ((RenderDeviceInterface *) clazz->interface)->endFrame = endFrame;
  ((RenderDeviceInterface *) clazz->interface)->init = init;
  ((RenderDeviceInterface *) clazz->interface)->removeDrawable = removeDrawable;
  ((RenderDeviceInterface *) clazz->interface)->renderDeviceDidReset = renderDeviceDidReset;
  ((RenderDeviceInterface *) clazz->interface)->renderDeviceWillReset = renderDeviceWillReset;
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
