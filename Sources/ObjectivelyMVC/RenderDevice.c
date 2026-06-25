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

  SDL_AcquireGPUSwapchainTexture(cmd,
                                 self->window,
                                 &swapchain->texture,
                                 (unsigned int *) &swapchain->size.w,
                                 (unsigned int *) &swapchain->size.h);

  return swapchain->texture != NULL;
}

/**
 * @fn void RenderDevice::addDrawable(RenderDevice *self, Drawable *drawable)
 * @memberof RenderDevice
 */
static void addDrawable(RenderDevice *self, Drawable *drawable) {

  assert(drawable);

  $(self->drawables, add, (ident) &drawable);
}

/**
 * @fn SDL_GPUBuffer *RenderDevice::createBuffer(const RenderDevice *self, const SDL_GPUBufferCreateInfo *info)
 * @memberof RenderDevice
 */
static SDL_GPUBuffer *createBuffer(const RenderDevice *self, const SDL_GPUBufferCreateInfo *info) {

  assert(info);

  SDL_GPUBuffer *buffer = SDL_CreateGPUBuffer(self->device, info);
  GPU_Assert(buffer, "SDL_CreateGPUBuffer");
  return buffer;
}

/**
 * @fn SDL_GPUComputePipeline *RenderDevice::createComputePipeline(const RenderDevice *self, const SDL_GPUComputePipelineCreateInfo *info)
 * @memberof RenderDevice
 */
static SDL_GPUComputePipeline *createComputePipeline(const RenderDevice *self, const SDL_GPUComputePipelineCreateInfo *info) {

  assert(info);

  SDL_GPUComputePipeline *pipeline = SDL_CreateGPUComputePipeline(self->device, info);
  GPU_Assert(pipeline, "SDL_CreateGPUComputePipeline");
  return pipeline;
}

/**
 * @fn SDL_GPUGraphicsPipeline *RenderDevice::createGraphicsPipeline(const RenderDevice *self, const SDL_GPUGraphicsPipelineCreateInfo *info)
 * @memberof RenderDevice
 */
static SDL_GPUGraphicsPipeline *createGraphicsPipeline(const RenderDevice *self, const SDL_GPUGraphicsPipelineCreateInfo *info) {

  assert(info);

  SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(self->device, info);
  GPU_Assert(pipeline, "SDL_CreateGPUGraphicsPipeline");
  return pipeline;
}

/**
 * @fn SDL_GPUSampler *RenderDevice::createSampler(const RenderDevice *self, const SDL_GPUSamplerCreateInfo *info)
 * @memberof RenderDevice
 */
static SDL_GPUSampler *createSampler(const RenderDevice *self, const SDL_GPUSamplerCreateInfo *info) {

  assert(info);

  SDL_GPUSampler *sampler = SDL_CreateGPUSampler(self->device, info);
  GPU_Assert(sampler, "SDL_CreateGPUSampler");
  return sampler;
}

/**
 * @fn SDL_GPUShader *RenderDevice::createShader(const RenderDevice *self, const SDL_GPUShaderCreateInfo *info)
 * @memberof RenderDevice
 */
static SDL_GPUShader *createShader(const RenderDevice *self, const SDL_GPUShaderCreateInfo *info) {

  assert(info);

  SDL_GPUShader *shader = SDL_CreateGPUShader(self->device, info);
  GPU_Assert(shader, "SDL_CreateGPUShader");
  return shader;
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

  SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(self->device, info);
  GPU_Assert(transferBuffer, "SDL_CreateGPUTransferBuffer");
  return transferBuffer;
}

/**
 * @fn SDL_GPUTextureFormat RenderDevice::getSwapchainTextureFormat(const RenderDevice *self, SDL_Window *window)
 * @memberof RenderDevice
 */
static SDL_GPUTextureFormat getSwapchainTextureFormat(const RenderDevice *self, SDL_Window *window) {

  assert(window);

  return SDL_GetGPUSwapchainTextureFormat(self->device, window);
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
 * @fn void *RenderDevice::mapTransferBuffer(const RenderDevice *self, SDL_GPUTransferBuffer *tbuf, bool cycle)
 * @memberof RenderDevice
 */
static void *mapTransferBuffer(const RenderDevice *self, SDL_GPUTransferBuffer *tbuf, bool cycle) {

  assert(tbuf);

  void *mapped = SDL_MapGPUTransferBuffer(self->device, tbuf, cycle);
  GPU_Assert(mapped, "SDL_MapGPUTransferBuffer");
  return mapped;
}

/**
 * @fn bool RenderDevice::queryFence(const RenderDevice *self, SDL_GPUFence *fence)
 * @memberof RenderDevice
 */
static bool queryFence(const RenderDevice *self, SDL_GPUFence *fence) {

  assert(fence);

  return SDL_QueryGPUFence(self->device, fence);
}

/**
 * @fn void RenderDevice::releaseBuffer(const RenderDevice *self, SDL_GPUBuffer *buffer)
 * @memberof RenderDevice
 */
static void releaseBuffer(const RenderDevice *self, SDL_GPUBuffer *buffer) {

  if (buffer) {
    SDL_ReleaseGPUBuffer(self->device, buffer);
  }
}

/**
 * @fn void RenderDevice::releaseComputePipeline(const RenderDevice *self, SDL_GPUComputePipeline *pipeline)
 * @memberof RenderDevice
 */
static void releaseComputePipeline(const RenderDevice *self, SDL_GPUComputePipeline *pipeline) {

  if (pipeline) {
    SDL_ReleaseGPUComputePipeline(self->device, pipeline);
  }
}

/**
 * @fn void RenderDevice::releaseFence(const RenderDevice *self, SDL_GPUFence *fence)
 * @memberof RenderDevice
 */
static void releaseFence(const RenderDevice *self, SDL_GPUFence *fence) {

  if (fence) {
    SDL_ReleaseGPUFence(self->device, fence);
  }
}

/**
 * @fn void RenderDevice::releaseGraphicsPipeline(const RenderDevice *self, SDL_GPUGraphicsPipeline *pipeline)
 * @memberof RenderDevice
 */
static void releaseGraphicsPipeline(const RenderDevice *self, SDL_GPUGraphicsPipeline *pipeline) {

  if (pipeline) {
    SDL_ReleaseGPUGraphicsPipeline(self->device, pipeline);
  }
}

/**
 * @fn void RenderDevice::releaseSampler(const RenderDevice *self, SDL_GPUSampler *sampler)
 * @memberof RenderDevice
 */
static void releaseSampler(const RenderDevice *self, SDL_GPUSampler *sampler) {

  if (sampler) {
    SDL_ReleaseGPUSampler(self->device, sampler);
  }
}

/**
 * @fn void RenderDevice::releaseShader(const RenderDevice *self, SDL_GPUShader *shader)
 * @memberof RenderDevice
 */
static void releaseShader(const RenderDevice *self, SDL_GPUShader *shader) {

  if (shader) {
    SDL_ReleaseGPUShader(self->device, shader);
  }
}

/**
 * @fn void RenderDevice::releaseTexture(const RenderDevice *self, SDL_GPUTexture *texture)
 * @memberof RenderDevice
 */
static void releaseTexture(const RenderDevice *self, SDL_GPUTexture *texture) {

  if (texture) {
    SDL_ReleaseGPUTexture(self->device, texture);
  }
}

/**
 * @fn void RenderDevice::releaseTransferBuffer(const RenderDevice *self, SDL_GPUTransferBuffer *tbuf)
 * @memberof RenderDevice
 */
static void releaseTransferBuffer(const RenderDevice *self, SDL_GPUTransferBuffer *tbuf) {

  if (tbuf) {
    SDL_ReleaseGPUTransferBuffer(self->device, tbuf);
  }
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
 * @fn bool RenderDevice::setAllowedFramesInFlight(const RenderDevice *self, Uint32 allowed)
 * @memberof RenderDevice
 */
static bool setAllowedFramesInFlight(const RenderDevice *self, Uint32 allowed) {

  return SDL_SetGPUAllowedFramesInFlight(self->device, allowed);
}

/**
 * @fn void RenderDevice::setBufferName(const RenderDevice *self, SDL_GPUBuffer *buffer, const char *name)
 * @memberof RenderDevice
 */
static void setBufferName(const RenderDevice *self, SDL_GPUBuffer *buffer, const char *name) {

  assert(buffer);
  assert(name);

  SDL_SetGPUBufferName(self->device, buffer, name);
}

/**
 * @fn bool RenderDevice::setSwapchainParameters(const RenderDevice *self, SDL_Window *window, SDL_GPUSwapchainComposition composition, SDL_GPUPresentMode mode)
 * @memberof RenderDevice
 */
static bool setSwapchainParameters(const RenderDevice *self, SDL_Window *window, SDL_GPUSwapchainComposition composition, SDL_GPUPresentMode mode) {

  assert(window);

  return SDL_SetGPUSwapchainParameters(self->device, window, composition, mode);
}

/**
 * @fn void RenderDevice::setTextureName(const RenderDevice *self, SDL_GPUTexture *texture, const char *name)
 * @memberof RenderDevice
 */
static void setTextureName(const RenderDevice *self, SDL_GPUTexture *texture, const char *name) {

  assert(texture);
  assert(name);

  SDL_SetGPUTextureName(self->device, texture, name);
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
      if (draw->deviceWillReset) {
        draw->deviceWillReset(draw->data);
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
      if (draw->deviceDidReset) {
        draw->deviceDidReset(self->device, draw->data);
      }
    }
  }
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
 * @fn bool RenderDevice::textureSupportsFormat(const RenderDevice *self, SDL_GPUTextureFormat format, SDL_GPUTextureType type, SDL_GPUTextureUsageFlags usage)
 * @memberof RenderDevice
 */
static bool textureSupportsFormat(const RenderDevice *self, SDL_GPUTextureFormat format, SDL_GPUTextureType type, SDL_GPUTextureUsageFlags usage) {

  return SDL_GPUTextureSupportsFormat(self->device, format, type, usage);
}

/**
 * @fn bool RenderDevice::textureSupportsSampleCount(const RenderDevice *self, SDL_GPUTextureFormat format, SDL_GPUSampleCount sample_count)
 * @memberof RenderDevice
 */
static bool textureSupportsSampleCount(const RenderDevice *self, SDL_GPUTextureFormat format, SDL_GPUSampleCount sample_count) {

  return SDL_GPUTextureSupportsSampleCount(self->device, format, sample_count);
}

/**
 * @fn void RenderDevice::unmapTransferBuffer(const RenderDevice *self, SDL_GPUTransferBuffer *tbuf)
 * @memberof RenderDevice
 */
static void unmapTransferBuffer(const RenderDevice *self, SDL_GPUTransferBuffer *tbuf) {

  assert(tbuf);

  SDL_UnmapGPUTransferBuffer(self->device, tbuf);
}

/**
 * @fn bool RenderDevice::waitForFences(const RenderDevice *self, bool wait_all, SDL_GPUFence *const *fences, Uint32 num_fences)
 * @memberof RenderDevice
 */
static bool waitForFences(const RenderDevice *self, bool wait_all, SDL_GPUFence *const *fences, Uint32 num_fences) {

  assert(fences || num_fences == 0);

  return SDL_WaitForGPUFences(self->device, wait_all, fences, num_fences);
}

/**
 * @fn bool RenderDevice::waitForIdle(const RenderDevice *self)
 * @memberof RenderDevice
 */
static bool waitForIdle(const RenderDevice *self) {

  return SDL_WaitForGPUIdle(self->device);
}

/**
 * @fn bool RenderDevice::waitForSwapchain(const RenderDevice *self, SDL_Window *window)
 * @memberof RenderDevice
 */
static bool waitForSwapchain(const RenderDevice *self, SDL_Window *window) {

  assert(window);

  return SDL_WaitForGPUSwapchain(self->device, window);
}

/**
 * @fn bool RenderDevice::windowSupportsPresentMode(const RenderDevice *self, SDL_Window *window, SDL_GPUPresentMode mode)
 * @memberof RenderDevice
 */
static bool windowSupportsPresentMode(const RenderDevice *self, SDL_Window *window, SDL_GPUPresentMode mode) {

  assert(window);

  return SDL_WindowSupportsGPUPresentMode(self->device, window, mode);
}

/**
 * @fn bool RenderDevice::windowSupportsSwapchainComposition(const RenderDevice *self, SDL_Window *window, SDL_GPUSwapchainComposition composition)
 * @memberof RenderDevice
 */
static bool windowSupportsSwapchainComposition(const RenderDevice *self, SDL_Window *window, SDL_GPUSwapchainComposition composition) {

  assert(window);

  return SDL_WindowSupportsGPUSwapchainComposition(self->device, window, composition);
}

#pragma mark - Object lifecycle

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  RenderDevice *this = (RenderDevice *) self;

  for (size_t i = 0; i < this->drawables->count; i++) {
    Drawable *draw = *VectorElement(this->drawables, Drawable *, i);
    if (draw->deviceWillReset) {
      draw->deviceWillReset(draw->data);
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

  ((RenderDeviceInterface *) clazz->interface)->acquireCommandBuffer = acquireCommandBuffer;
  ((RenderDeviceInterface *) clazz->interface)->acquireSwapchainTexture = acquireSwapchainTexture;
  ((RenderDeviceInterface *) clazz->interface)->addDrawable = addDrawable;
  ((RenderDeviceInterface *) clazz->interface)->createBuffer = createBuffer;
  ((RenderDeviceInterface *) clazz->interface)->createComputePipeline = createComputePipeline;
  ((RenderDeviceInterface *) clazz->interface)->createGraphicsPipeline = createGraphicsPipeline;
  ((RenderDeviceInterface *) clazz->interface)->createSampler = createSampler;
  ((RenderDeviceInterface *) clazz->interface)->createShader = createShader;
  ((RenderDeviceInterface *) clazz->interface)->createTexture = createTexture;
  ((RenderDeviceInterface *) clazz->interface)->createTransferBuffer = createTransferBuffer;
  ((RenderDeviceInterface *) clazz->interface)->getSwapchainTextureFormat = getSwapchainTextureFormat;
  ((RenderDeviceInterface *) clazz->interface)->init = init;
  ((RenderDeviceInterface *) clazz->interface)->initWithWindow = initWithWindow;
  ((RenderDeviceInterface *) clazz->interface)->mapTransferBuffer = mapTransferBuffer;
  ((RenderDeviceInterface *) clazz->interface)->queryFence = queryFence;
  ((RenderDeviceInterface *) clazz->interface)->releaseBuffer = releaseBuffer;
  ((RenderDeviceInterface *) clazz->interface)->releaseComputePipeline = releaseComputePipeline;
  ((RenderDeviceInterface *) clazz->interface)->releaseFence = releaseFence;
  ((RenderDeviceInterface *) clazz->interface)->releaseGraphicsPipeline = releaseGraphicsPipeline;
  ((RenderDeviceInterface *) clazz->interface)->releaseSampler = releaseSampler;
  ((RenderDeviceInterface *) clazz->interface)->releaseShader = releaseShader;
  ((RenderDeviceInterface *) clazz->interface)->releaseTexture = releaseTexture;
  ((RenderDeviceInterface *) clazz->interface)->releaseTransferBuffer = releaseTransferBuffer;
  ((RenderDeviceInterface *) clazz->interface)->removeDrawable = removeDrawable;
  ((RenderDeviceInterface *) clazz->interface)->setAllowedFramesInFlight = setAllowedFramesInFlight;
  ((RenderDeviceInterface *) clazz->interface)->setBufferName = setBufferName;
  ((RenderDeviceInterface *) clazz->interface)->setSwapchainParameters = setSwapchainParameters;
  ((RenderDeviceInterface *) clazz->interface)->setTextureName = setTextureName;
  ((RenderDeviceInterface *) clazz->interface)->setWindow = setWindow;
  ((RenderDeviceInterface *) clazz->interface)->submit = submit;
  ((RenderDeviceInterface *) clazz->interface)->submitAndFence = submitAndFence;
  ((RenderDeviceInterface *) clazz->interface)->textureSupportsFormat = textureSupportsFormat;
  ((RenderDeviceInterface *) clazz->interface)->textureSupportsSampleCount = textureSupportsSampleCount;
  ((RenderDeviceInterface *) clazz->interface)->unmapTransferBuffer = unmapTransferBuffer;
  ((RenderDeviceInterface *) clazz->interface)->waitForFences = waitForFences;
  ((RenderDeviceInterface *) clazz->interface)->waitForIdle = waitForIdle;
  ((RenderDeviceInterface *) clazz->interface)->waitForSwapchain = waitForSwapchain;
  ((RenderDeviceInterface *) clazz->interface)->windowSupportsPresentMode = windowSupportsPresentMode;
  ((RenderDeviceInterface *) clazz->interface)->windowSupportsSwapchainComposition = windowSupportsSwapchainComposition;
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
