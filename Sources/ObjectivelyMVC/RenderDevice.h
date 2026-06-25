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

#pragma once

#include <SDL3/SDL_gpu.h>

#include <Objectively/Object.h>
#include <Objectively/Vector.h>

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief RenderDevice owns `SDL_gpu` infrastructure for a window and frame loop.
 */

typedef struct RenderDevice RenderDevice;

/**
 * @brief A GPU-renderable unit with optional transfer and submit callbacks.
 *
 * Drawables participate in the RenderDevice's frame loop. Assign only the
 * callbacks you need; NULL callbacks are silently skipped.
 *
 * Typical use without subclassing:
 * @code
 *   $(device, addDrawable, &(Drawable) {
 *     .transfer = R_LoadLightgrid,
 *     .data = r_world_model,
 *   });
 * @endcode
 */
struct Drawable {

  /**
   * @brief Called when the GPU device has been (re-)created. Allocate GPU resources here.
   */
  void (*deviceDidReset)(SDL_GPUDevice *device, ident data);

  /**
   * @brief Called just before the GPU device is destroyed. Release GPU resources here.
   */
  void (*deviceWillReset)(ident data);

  /**
   * @brief Called each frame during the render pass. Issue draw commands here.
   */
  void (*submit)(SDL_GPUCommandBuffer *cmd, SDL_GPURenderPass *renderPass, ident data);

  /**
   * @brief Called when dirty during the copy pass. Upload CPU data to GPU buffers/textures here.
   */
  void (*transfer)(SDL_GPUCopyPass *copyPass, ident data);

  /**
   * @brief When true, the RenderDevice calls transfer() this frame.
   * Reset to false after transfer() returns.
   */
  bool isDirty;

  /**
   * @brief User data, passed to all callbacks.
   */
  ident data;
};

typedef struct Drawable Drawable;

/**
 * @brief The Swapchain (render target) type.
 */
struct Swapchain {

  /**
   * @brief The current render target.
   * @private
   */
  SDL_GPUTexture *texture;

  /**
   * @brief The swapchain dimensions for this frame.
   * @private
   */
  SDL_Size size;
};

typedef struct Swapchain Swapchain;

typedef struct RenderDeviceInterface RenderDeviceInterface;

/**
 * @brief Owns the `SDL_GPUDevice`, window, frame lifecycle, and Drawable management.
 *
 * RenderDevice is the GPU infrastructure base class. It creates and owns the
 * `SDL_GPUDevice`, manages the frame command buffer and swapchain, registers
 * Drawables for participation in the frame loop, and provides factory methods
 * for GPU resources.
 *
 * @extends Object
 */
struct RenderDevice {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  RenderDeviceInterface *interface;

  /**
   * @brief The SDL_GPU device.
   */
  SDL_GPUDevice *device;

  /**
   * @brief The window currently being rendered.
   */
  SDL_Window *window;

  /**
   * @brief When true, the render pass clears to @c clear_color before drawing.
   * @details Defaults to `true` (clear to opaque black).
   */
  bool clear;

  /**
   * @brief The color used to clear the swapchain texture when @c clear is true.
   * @details Defaults to opaque black `{0, 0, 0, 1}`.
   */
  SDL_FColor clearColor;

  /**
   * @brief Registered Drawables participating in this device's frame loop.
   * @private
   */
  Vector *drawables;
};

/**
 * @brief The RenderDevice interface.
 */
struct RenderDeviceInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  SDL_GPUCommandBuffer *(*acquireCommandBuffer)(const RenderDevice *self);
  bool (*acquireSwapchainTexture)(const RenderDevice *self, SDL_GPUCommandBuffer *cmd, Swapchain *swapchain);
  void (*addDrawable)(RenderDevice *self, Drawable *drawable);

  SDL_GPUBuffer *(*createBuffer)(const RenderDevice *self, const SDL_GPUBufferCreateInfo *info);
  SDL_GPUComputePipeline *(*createComputePipeline)(const RenderDevice *self, const SDL_GPUComputePipelineCreateInfo *info);
  SDL_GPUGraphicsPipeline *(*createGraphicsPipeline)(const RenderDevice *self, const SDL_GPUGraphicsPipelineCreateInfo *info);
  SDL_GPUSampler *(*createSampler)(const RenderDevice *self, const SDL_GPUSamplerCreateInfo *info);
  SDL_GPUShader *(*createShader)(const RenderDevice *self, const SDL_GPUShaderCreateInfo *info);
  SDL_GPUTexture *(*createTexture)(const RenderDevice *self, const SDL_GPUTextureCreateInfo *info, const void *pixels);
  SDL_GPUTransferBuffer *(*createTransferBuffer)(const RenderDevice *self, const SDL_GPUTransferBufferCreateInfo *info);

  SDL_GPUTextureFormat (*getSwapchainTextureFormat)(const RenderDevice *self, SDL_Window *window);

  RenderDevice *(*init)(RenderDevice *self);
  RenderDevice *(*initWithWindow)(RenderDevice *self, SDL_Window *window);

  void *(*mapTransferBuffer)(const RenderDevice *self, SDL_GPUTransferBuffer *tbuf, bool cycle);

  bool (*queryFence)(const RenderDevice *self, SDL_GPUFence *fence);

  void (*releaseBuffer)(const RenderDevice *self, SDL_GPUBuffer *buffer);
  void (*releaseComputePipeline)(const RenderDevice *self, SDL_GPUComputePipeline *pipeline);
  void (*releaseFence)(const RenderDevice *self, SDL_GPUFence *fence);
  void (*releaseGraphicsPipeline)(const RenderDevice *self, SDL_GPUGraphicsPipeline *pipeline);
  void (*releaseSampler)(const RenderDevice *self, SDL_GPUSampler *sampler);
  void (*releaseShader)(const RenderDevice *self, SDL_GPUShader *shader);
  void (*releaseTexture)(const RenderDevice *self, SDL_GPUTexture *texture);
  void (*releaseTransferBuffer)(const RenderDevice *self, SDL_GPUTransferBuffer *tbuf);

  void (*removeDrawable)(RenderDevice *self, Drawable *drawable);

  bool (*setAllowedFramesInFlight)(const RenderDevice *self, Uint32 allowed);
  void (*setBufferName)(const RenderDevice *self, SDL_GPUBuffer *buffer, const char *name);
  bool (*setSwapchainParameters)(const RenderDevice *self, SDL_Window *window, SDL_GPUSwapchainComposition composition, SDL_GPUPresentMode mode);
  void (*setTextureName)(const RenderDevice *self, SDL_GPUTexture *texture, const char *name);
  void (*setWindow)(RenderDevice *self, SDL_Window *window);

  void (*submit)(const RenderDevice *self, SDL_GPUCommandBuffer *cmd);
  SDL_GPUFence *(*submitAndFence)(const RenderDevice *self, SDL_GPUCommandBuffer *cmd);

  bool (*textureSupportsFormat)(const RenderDevice *self, SDL_GPUTextureFormat format, SDL_GPUTextureType type, SDL_GPUTextureUsageFlags usage);
  bool (*textureSupportsSampleCount)(const RenderDevice *self, SDL_GPUTextureFormat format, SDL_GPUSampleCount sample_count);

  void (*unmapTransferBuffer)(const RenderDevice *self, SDL_GPUTransferBuffer *tbuf);

  bool (*waitForFences)(const RenderDevice *self, bool wait_all, SDL_GPUFence *const *fences, Uint32 num_fences);
  bool (*waitForIdle)(const RenderDevice *self);
  bool (*waitForSwapchain)(const RenderDevice *self, SDL_Window *window);

  bool (*windowSupportsPresentMode)(const RenderDevice *self, SDL_Window *window, SDL_GPUPresentMode mode);
  bool (*windowSupportsSwapchainComposition)(const RenderDevice *self, SDL_Window *window, SDL_GPUSwapchainComposition composition);
};

/**
 * @fn Class *RenderDevice::_RenderDevice(void)
 * @brief The RenderDevice archetype.
 * @return The RenderDevice Class.
 * @memberof RenderDevice
 */
OBJECTIVELYMVC_EXPORT Class *_RenderDevice(void);
