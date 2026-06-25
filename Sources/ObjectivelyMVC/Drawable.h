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

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief Abstract base class for GPU-renderable units.
 */

typedef struct Drawable Drawable;
typedef struct DrawableInterface DrawableInterface;

/**
 * @brief Abstract base class for GPU-renderable units.
 *
 * A Drawable participates in the RenderDevice's frame loop. Each frame the
 * RenderDevice calls copy() inside the active copy pass (for GPU uploads, only
 * when dirty) and submit() inside the active render pass (for draw commands).
 *
 * Subclasses allocate their GPU resources in renderDeviceDidReset and release
 * them in renderDeviceWillReset. Alternatively, set the delegate function
 * pointers (copy, submit, renderDeviceDidReset, renderDeviceWillReset) on the
 * struct directly to avoid subclassing.
 *
 * @extends Object
 */
struct Drawable {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  DrawableInterface *interface;

  /**
   * @brief When true the RenderDevice will call copy() this frame to re-upload GPU data.
   * Subclasses should set this to true whenever their geometry or texture data changes.
   * The RenderDevice resets it to false after copy() returns.
   */
  bool dirty;

  /**
   * @brief Optional user data passed to delegate callbacks.
   */
  void *data;

  /**
   * @brief Optional delegate invoked by copy(). Called when dirty during the copy pass.
   * @param self The Drawable.
   * @param copyPass The active copy pass.
   */
  void (*copy)(Drawable *self, SDL_GPUCopyPass *copyPass);

  /**
   * @brief Optional delegate invoked by renderDeviceDidReset(). Allocate GPU resources here.
   * @param self The Drawable.
   * @param device The newly created SDL_GPUDevice.
   */
  void (*renderDeviceDidReset)(Drawable *self, SDL_GPUDevice *device);

  /**
   * @brief Optional delegate invoked by renderDeviceWillReset(). Release GPU resources here.
   * @param self The Drawable.
   */
  void (*renderDeviceWillReset)(Drawable *self);

  /**
   * @brief Optional delegate invoked by submit(). Called each frame during the render pass.
   * @param self The Drawable.
   * @param cmd The active command buffer.
   * @param renderPass The active render pass.
   */
  void (*submit)(Drawable *self, SDL_GPUCommandBuffer *cmd, SDL_GPURenderPass *renderPass);
};

/**
 * @brief The Drawable interface.
 */
struct DrawableInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn void Drawable::copy(Drawable *self, SDL_GPUCopyPass *copyPass)
   * @brief Upload geometry or texture data during the frame's copy pass.
   * @details Only called when self->dirty is true. The RenderDevice resets dirty
   * to false after this returns.
   * @param self The Drawable.
   * @param copyPass The active SDL_GPUCopyPass for this frame.
   * @memberof Drawable
   */
  void (*copy)(Drawable *self, SDL_GPUCopyPass *copyPass);

  /**
   * @fn Drawable *Drawable::init(Drawable *self)
   * @brief Initializes this Drawable.
   * @param self The Drawable.
   * @return The initialized Drawable, or `NULL` on error.
   * @memberof Drawable
   */
  Drawable *(*init)(Drawable *self);

  /**
   * @fn Drawable *Drawable::initWithData(Drawable *self, void *data)
   * @brief Initializes this Drawable with user data for delegate callbacks.
   * @details Sets self->data and starts with all delegate pointers NULL. Assign
   *   self->copy, self->submit, self->renderDeviceDidReset, and
   *   self->renderDeviceWillReset as needed.
   * @param self The Drawable.
   * @param data Arbitrary user data accessible in delegate callbacks via self->data.
   * @return The initialized Drawable, or `NULL` on error.
   * @memberof Drawable
   */
  Drawable *(*initWithData)(Drawable *self, void *data);

  /**
   * @fn void Drawable::renderDeviceDidReset(Drawable *self, SDL_GPUDevice *device)
   * @brief Invoked when the GPU device has been (re-)created.
   * @details Subclasses should allocate pipelines, buffers, and other GPU
   * resources here. The device pointer is valid for the lifetime of the
   * Drawable's registration with the RenderDevice.
   * @param self The Drawable.
   * @param device The newly created SDL_GPUDevice.
   * @memberof Drawable
   */
  void (*renderDeviceDidReset)(Drawable *self, SDL_GPUDevice *device);

  /**
   * @fn void Drawable::renderDeviceWillReset(Drawable *self)
   * @brief Invoked just before the GPU device is destroyed.
   * @details Subclasses must release all SDL_GPU resources they own here.
   * @param self The Drawable.
   * @memberof Drawable
   */
  void (*renderDeviceWillReset)(Drawable *self);

  /**
   * @fn void Drawable::submit(Drawable *self, SDL_GPUCommandBuffer *cmd, SDL_GPURenderPass *renderPass)
   * @brief Issue GPU draw commands during the frame's render pass.
   * @param self The Drawable.
   * @param cmd The active SDL_GPUCommandBuffer. Required for SDL_PushGPUVertexUniformData
   *   and SDL_PushGPUFragmentUniformData.
   * @param renderPass The active SDL_GPURenderPass for bind and draw calls.
   * @memberof Drawable
   */
  void (*submit)(Drawable *self, SDL_GPUCommandBuffer *cmd, SDL_GPURenderPass *renderPass);
};

/**
 * @fn Class *Drawable::_Drawable(void)
 * @brief The Drawable archetype.
 * @return The Drawable Class.
 * @memberof Drawable
 */
OBJECTIVELYMVC_EXPORT Class *_Drawable(void);
