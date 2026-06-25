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

#include <Objectively/Array.h>
#include <Objectively/Object.h>

#include <ObjectivelyMVC/Drawable.h>
#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief RenderDevice owns `SDL_gpu` infrastructure for a window and frame loop.
 */

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

typedef struct RenderDevice RenderDevice;
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
  Array *drawables;
};

/**
 * @brief The RenderDevice interface.
 */
struct RenderDeviceInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn void RenderDevice::addDrawable(RenderDevice *self, Drawable *drawable)
   * @brief Registers a Drawable to participate in this RenderDevice's frame loop.
   * @param self The RenderDevice.
   * @param drawable The Drawable to add.
   * @memberof RenderDevice
   */
  void (*addDrawable)(RenderDevice *self, Drawable *drawable);

  /**
   * @fn SDL_GPUCommandBuffer *RenderDevice::acquireCommandBuffer(const RenderDevice *self)
   * @brief Acquires a command buffer for recording GPU work.
   * @param self The RenderDevice.
   * @return A command buffer. GPU_Asserts on failure (wrong thread, device lost, etc.).
   * @remarks The returned command buffer must be submitted or cancelled by the caller.
   * @memberof RenderDevice
   */
  SDL_GPUCommandBuffer *(*acquireCommandBuffer)(const RenderDevice *self);

  /**
   * @fn bool RenderDevice::acquireSwapchainTexture(const RenderDevice *self, SDL_GPUCommandBuffer *cmd, Swapchain *swapchain)
   * @brief Attempts to acquire the swapchain texture for the given command buffer.
   * @param self The RenderDevice.
   * @param cmd The command buffer that will render to the swapchain.
   * @param swapchain Filled with the swapchain texture and dimensions on success.
   * @return True if the swapchain is available this frame; false if it is temporarily
   *   unavailable (window minimized, pipeline warm-up, etc.). The caller should cancel
   *   @c cmd and skip the frame when this returns false.
   * @memberof RenderDevice
   */
  bool (*acquireSwapchainTexture)(const RenderDevice *self, SDL_GPUCommandBuffer *cmd, Swapchain *swapchain);

  /**
   * @fn void RenderDevice::submit(const RenderDevice *self, SDL_GPUCommandBuffer *cmd)
   * @brief Submits a command buffer to the GPU.
   * @param self The RenderDevice.
   * @param cmd The command buffer to submit.
   * @memberof RenderDevice
   */
  void (*submit)(const RenderDevice *self, SDL_GPUCommandBuffer *cmd);

  /**
   * @fn SDL_GPUFence *RenderDevice::submitAndFence(const RenderDevice *self, SDL_GPUCommandBuffer *cmd)
   * @brief Submits a command buffer and returns a fence for tracking its completion.
   * @param self The RenderDevice.
   * @param cmd The command buffer to submit.
   * @return A fence that becomes signaled when the submission completes. The caller
   *   must release it with SDL_ReleaseGPUFence when done.
   * @memberof RenderDevice
   */
  SDL_GPUFence *(*submitAndFence)(const RenderDevice *self, SDL_GPUCommandBuffer *cmd);

  /**
   * @fn SDL_GPUBuffer *RenderDevice::createBuffer(const RenderDevice *self, const SDL_GPUBufferCreateInfo *info)
   * @brief Creates a GPU buffer on this RenderDevice's device.
   * @param self The RenderDevice.
   * @param info The buffer creation parameters.
   * @return The new buffer, or `NULL` on error.
   * @memberof RenderDevice
   */
  SDL_GPUBuffer *(*createBuffer)(const RenderDevice *self, const SDL_GPUBufferCreateInfo *info);

  /**
   * @fn SDL_GPUTexture *RenderDevice::createTexture(const RenderDevice *self, const SDL_GPUTextureCreateInfo *info, const void *pixels)
   * @brief Creates a GPU texture and optionally uploads pixel data.
   * @param self The RenderDevice.
   * @param info The texture creation parameters (type, format, dimensions, etc.).
   * @param pixels Optional pixel data to upload. If non-NULL, a one-shot transfer
   *   buffer is created, `info->width * info->height * info->layer_count_or_depth`
   *   texels (at `SDL_GPUTextureFormatTexelBlockSize` bytes each) are copied from
   *   `pixels`, and the upload is submitted immediately. For 3D textures the data
   *   is expected to be depth slices laid out contiguously. Pass `NULL` to create
   *   the texture without any upload (caller manages the transfer).
   * @return A new SDL_GPUTexture on success, or `NULL` on error.
   * @remarks The caller owns the returned texture; release it with
   *   SDL_ReleaseGPUTexture(self->device, texture).
   * @memberof RenderDevice
   */
  SDL_GPUTexture *(*createTexture)(const RenderDevice *self, const SDL_GPUTextureCreateInfo *info, const void *pixels);

  /**
   * @fn SDL_GPUTransferBuffer *RenderDevice::createTransferBuffer(const RenderDevice *self, const SDL_GPUTransferBufferCreateInfo *info)
   * @brief Creates a transfer buffer on this RenderDevice's device.
   * @param self The RenderDevice.
   * @param info The transfer buffer creation parameters.
   * @return The new transfer buffer, or `NULL` on error.
   * @memberof RenderDevice
   */
  SDL_GPUTransferBuffer *(*createTransferBuffer)(const RenderDevice *self, const SDL_GPUTransferBufferCreateInfo *info);

  /**
   * @protected
   * @fn RenderDevice *RenderDevice::init(RenderDevice *self)
   * @brief Initializes this RenderDevice.
   * @param self The RenderDevice.
   * @return The initialized RenderDevice, or `NULL` on error.
   * @memberof RenderDevice
   */
  RenderDevice *(*init)(RenderDevice *self);

  /**
   * @fn RenderDevice *RenderDevice::initWithWindow(RenderDevice *self, SDL_Window *window)
   * @brief Initializes this RenderDevice and immediately claims the given window.
   * @details Convenience initializer equivalent to `init` followed by `setWindow`.
   *   Use `init` instead when offscreen rendering is required.
   * @param self The RenderDevice.
   * @param window The window to claim for GPU rendering.
   * @return The initialized RenderDevice, or `NULL` on error.
   * @memberof RenderDevice
   */
  RenderDevice *(*initWithWindow)(RenderDevice *self, SDL_Window *window);

  /**
   * @fn void RenderDevice::removeDrawable(RenderDevice *self, Drawable *drawable)
   * @brief Unregisters a Drawable from this RenderDevice's frame loop.
   * @param self The RenderDevice.
   * @param drawable The Drawable to remove.
   * @memberof RenderDevice
   */
  void (*removeDrawable)(RenderDevice *self, Drawable *drawable);

  /**
   * @fn void RenderDevice::setWindow(RenderDevice *self, SDL_Window *window)
   * @brief Binds this RenderDevice to the given window.
   * @details Claims the window for GPU rendering. If a different window was
   *   previously claimed, it is released first and registered Drawables are
   *   notified via renderDeviceWillReset before the new window is claimed and
   *   Drawables are notified via renderDeviceDidReset.
   * @param self The RenderDevice.
   * @param window The SDL_Window to render into.
   * @memberof RenderDevice
   */
  void (*setWindow)(RenderDevice *self, SDL_Window *window);
};

/**
 * @fn Class *RenderDevice::_RenderDevice(void)
 * @brief The RenderDevice archetype.
 * @return The RenderDevice Class.
 * @memberof RenderDevice
 */
OBJECTIVELYMVC_EXPORT Class *_RenderDevice(void);
