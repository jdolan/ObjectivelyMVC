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
#include <ObjectivelyGPU/RenderDevice.h>

/**
 * @file
 * @brief Renderer extends Object with ObjectivelyMVC's UI rendering layer.
 * @details Renderer provides the MVC-specific shaders, pipelines, vertex
 * streaming, draw call queue, scissor state, color state, and draw* helpers.
 */

typedef struct Renderer Renderer;
typedef struct RendererInterface RendererInterface;

/**
 * @brief Interleaved position + texcoord vertex for GPU upload.
 */
typedef struct {
  float x, y;
  float u, v;
} MVC_Vertex;

/**
 * @brief A recorded draw call for deferred GPU submission.
 * @private
 */
typedef struct {
  Uint32 firstVertex;
  Uint32 vertexCount;
  SDL_GPUTexture *texture;
  float color[4];
  SDL_Rect scissor;
  bool hasScissor;
} MVC_DrawCall;

/**
 * @brief Renderer extends Object with ObjectivelyMVC's UI rendering layer.
 * @details Create a WindowController to instantiate a default Renderer, or
 * supply your own subclass via WindowController::setRenderer.
 * @extends Object
 */
struct Renderer {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  RendererInterface *interface;

  /**
   * @brief The backing RenderDevice.
   */
  RenderDevice *device;

  /**
   * @brief The current frame command buffer (valid between beginFrame and endFrame).
   * @private
   */
  CommandBuffer *cmd;

  /**
   * @brief The current swapchain texture and dimensions (valid between beginFrame and endFrame).
   * @private
   */
  Swapchain swapchain;

  /**
   * @brief The current draw color.
   * @private
   */
  SDL_Color color;

  /**
   * @brief The graphics pipeline (TRIANGLELIST, for all MVC geometry).
   * @private
   */
  SDL_GPUGraphicsPipeline *pipeline;

  /**
   * @brief The GPU-side vertex buffer (resized as needed to fit vertices).
   * @private
   */
  SDL_GPUBuffer *vertexBuffer;

  /**
   * @brief Capacity of vertexBuffer in vertices.
   * @private
   */
  Uint32 vertexBufferCapacity;

  /**
   * @brief The linear clamp-to-edge sampler for texture rendering.
   * @details Borrowed from `RenderDevice::samplerLinearClamp` — do not release.
   * @private
   */
  SDL_GPUSampler *sampler;

  /**
   * @brief The 1×1 white fallback texture (used for solid-color primitives).
   * @private
   */
  SDL_GPUTexture *white;

  /**
   * @brief CPU-side vertex accumulation buffer for this frame.
   * @private
   */
  Vector *vertices;

  /**
   * @brief Draw call queue for this frame.
   * @private
   */
  Vector *drawCalls;

  /**
   * @brief The current scissor rectangle (in pixel coordinates).
   * @private
   */
  SDL_Rect scissor;

  /**
   * @brief True if a scissor rect is active.
   * @private
   */
  bool hasScissor;

  /**
   * @brief If true (default), endFrame clears the swapchain before rendering.
   * Set to false when a game scene has already rendered into the swapchain on
   * the same command buffer, so MVC composites on top.
   */
  bool clear;
};

/**
 * @brief The Renderer interface.
 */
struct RendererInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @protected
   * @fn Renderer *Renderer::init(Renderer *self)
   * @brief Initializes this Renderer.
   * @param self The Renderer.
   * @return The initialized Renderer, or `NULL` on error.
   * @memberof Renderer
   */
  Renderer *(*init)(Renderer *self);

  /**
   * @fn void Renderer::beginFrame(Renderer *self)
   * @brief Begins a new frame on the backing RenderDevice and maps MVC staging buffers.
   * @param self The Renderer.
   * @memberof Renderer
   */
  void (*beginFrame)(Renderer *self);

  /**
   * @fn void Renderer::endFrame(Renderer *self)
   * @brief Finalizes MVC uploads and submits the frame on the backing RenderDevice.
   * @param self The Renderer.
   * @memberof Renderer
   */
  void (*endFrame)(Renderer *self);

  /**
   * @fn void Renderer::renderDeviceDidReset(Renderer *self)
   * @brief Recreates MVC GPU resources after the backing RenderDevice resets.
   * @param self The Renderer.
   * @memberof Renderer
   */
  void (*renderDeviceDidReset)(Renderer *self);

  /**
   * @fn void Renderer::renderDeviceWillReset(Renderer *self)
   * @brief Releases MVC GPU resources before the backing RenderDevice resets.
   * @param self The Renderer.
   * @memberof Renderer
   */
  void (*renderDeviceWillReset)(Renderer *self);

  /**
   * @fn void Renderer::drawLine(const Renderer *self, const SDL_Point *points)
   * @brief Records a line segment between two points.
   * @param self The Renderer.
   * @param points Two points defining the line segment.
   * @memberof Renderer
   */
  void (*drawLine)(const Renderer *self, const SDL_Point *points);

  /**
   * @fn void Renderer::drawLines(const Renderer *self, const SDL_Point *points, size_t count)
   * @brief Records a polyline through the given points.
   * @param self The Renderer.
   * @param points The points.
   * @param count The number of points.
   * @memberof Renderer
   */
  void (*drawLines)(const Renderer *self, const SDL_Point *points, size_t count);

  /**
   * @fn void Renderer::drawRect(const Renderer *self, const SDL_Rect *rect)
   * @brief Records a rectangle outline.
   * @param self The Renderer.
   * @param rect The rectangle.
   * @memberof Renderer
   */
  void (*drawRect)(const Renderer *self, const SDL_Rect *rect);

  /**
   * @fn void Renderer::drawRectFilled(const Renderer *self, const SDL_Rect *rect)
   * @brief Records a filled rectangle.
   * @param self The Renderer.
   * @param rect The rectangle.
   * @memberof Renderer
   */
  void (*drawRectFilled)(const Renderer *self, const SDL_Rect *rect);

  /**
   * @fn void Renderer::drawTexture(const Renderer *self, SDL_GPUTexture *texture, const SDL_Rect *dest)
   * @brief Records a textured quad in the given destination rectangle.
   * @param self The Renderer.
   * @param texture The GPU texture to sample.
   * @param dest The destination rectangle in logical screen coordinates.
   * @memberof Renderer
   */
  void (*drawTexture)(const Renderer *self, SDL_GPUTexture *texture, const SDL_Rect *dest);

  /**
   * @fn void Renderer::drawView(Renderer *self, View *view)
   * @brief Sets the clipping frame and invokes View::render for the given View.
   * @param self The Renderer.
   * @param view The View to render.
   * @memberof Renderer
   */
  void (*drawView)(Renderer *self, View *view);

  /**
   * @fn void Renderer::setClippingFrame(Renderer *self, const SDL_Rect *clippingFrame)
   * @brief Sets the scissor rectangle for subsequent draw calls.
   * @param self The Renderer.
   * @param clippingFrame The clipping rectangle in logical screen coordinates, or
   *   `NULL` to disable clipping (full window scissor).
   * @memberof Renderer
   */
  void (*setClippingFrame)(Renderer *self, const SDL_Rect *clippingFrame);

  /**
   * @fn void Renderer::setDrawColor(Renderer *self, const SDL_Color *color)
   * @brief Sets the primary color multiplier for subsequent draw calls.
   * @param self The Renderer.
   * @param color The color.
   * @memberof Renderer
   */
  void (*setDrawColor)(Renderer *self, const SDL_Color *color);
};

/**
 * @fn Class *Renderer::_Renderer(void)
 * @brief The Renderer archetype.
 * @return The Renderer Class.
 * @memberof Renderer
 */
OBJECTIVELYMVC_EXPORT Class *_Renderer(void);
