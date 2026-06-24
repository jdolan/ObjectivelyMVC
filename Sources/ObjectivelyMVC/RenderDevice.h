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
   * @brief The current frame command buffer (valid between beginFrame and endFrame).
   * @private
   */
  SDL_GPUCommandBuffer *cmd;

  /**
   * @brief The current Swapchain.
   */
  Swapchain swapchain;

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
   * @fn void RenderDevice::beginFrame(RenderDevice *self)
   * @brief Acquires the frame command buffer and swapchain texture.
   * @param self The RenderDevice.
   * @remarks If the swapchain texture is unavailable for this frame, the command
   * buffer is cancelled and the frame is skipped.
   * @memberof RenderDevice
   */
  void (*beginFrame)(RenderDevice *self);

  /**
   * @fn SDL_GPUBuffer *RenderDevice::createBuffer(const RenderDevice *self, SDL_GPUBufferUsageFlags usage, Uint32 size)
   * @brief Creates a GPU buffer on this RenderDevice's device.
   * @param self The RenderDevice.
   * @param usage Buffer usage flags.
   * @param size Size in bytes.
   * @return The new buffer, or `NULL` on error.
   * @memberof RenderDevice
   */
  SDL_GPUBuffer *(*createBuffer)(const RenderDevice *self, SDL_GPUBufferUsageFlags usage, Uint32 size);

  /**
   * @fn SDL_GPUTexture *RenderDevice::createTexture(const RenderDevice *self, const SDL_Surface *surface)
   * @brief Creates a GPU texture from the given surface, uploading its pixels.
   * @param self The RenderDevice.
   * @param surface The surface to upload.
   * @return A new SDL_GPUTexture on success, or `NULL` on error.
   * @remarks The caller owns the returned texture; release it with
   * SDL_ReleaseGPUTexture(self->device, texture).
   * @memberof RenderDevice
   */
  SDL_GPUTexture *(*createTexture)(const RenderDevice *self, const SDL_Surface *surface);

  /**
   * @fn SDL_GPUTransferBuffer *RenderDevice::createTransferBuffer(const RenderDevice *self, SDL_GPUTransferBufferUsage usage, Uint32 size)
   * @brief Creates a transfer buffer on this RenderDevice's device.
   * @param self The RenderDevice.
   * @param usage Transfer buffer usage.
   * @param size Size in bytes.
   * @return The new transfer buffer, or `NULL` on error.
   * @memberof RenderDevice
   */
  SDL_GPUTransferBuffer *(*createTransferBuffer)(const RenderDevice *self, SDL_GPUTransferBufferUsage usage, Uint32 size);

  /**
   * @fn void RenderDevice::endFrame(RenderDevice *self)
   * @brief Submits this frame's Drawable uploads and render pass.
   * @param self The RenderDevice.
   * @memberof RenderDevice
   */
  void (*endFrame)(RenderDevice *self);

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
   * @fn void RenderDevice::removeDrawable(RenderDevice *self, Drawable *drawable)
   * @brief Unregisters a Drawable from this RenderDevice's frame loop.
   * @param self The RenderDevice.
   * @param drawable The Drawable to remove.
   * @memberof RenderDevice
   */
  void (*removeDrawable)(RenderDevice *self, Drawable *drawable);

  /**
   * @fn void RenderDevice::renderDeviceDidReset(RenderDevice *self)
   * @brief Invoked when the GPU device has been (re-)created.
   * @details Creates the SDL_GPUDevice, claims the window, and notifies
   * registered Drawables.
   * @param self The RenderDevice.
   * @memberof RenderDevice
   */
  void (*renderDeviceDidReset)(RenderDevice *self);

  /**
   * @fn void RenderDevice::renderDeviceWillReset(RenderDevice *self)
   * @brief Invoked just before the GPU device is destroyed.
   * @details Notifies registered Drawables, releases the window, and destroys
   * the SDL_GPUDevice.
   * @param self The RenderDevice.
   * @memberof RenderDevice
   */
  void (*renderDeviceWillReset)(RenderDevice *self);

  /**
   * @fn void RenderDevice::setWindow(RenderDevice *self, SDL_Window *window)
   * @brief Binds this RenderDevice to the given window.
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
