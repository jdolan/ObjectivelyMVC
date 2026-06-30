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

#include <ObjectivelyGPU.h>

#include "Types.h"

/**
 * @file
 * @brief Renderer extends Object with ObjectivelyMVC's UI rendering layer.
 * @details Renderer provides the MVC-specific shaders, pipelines, vertex
 * streaming, draw call queue, scissor state, and draw* helpers.
 */

typedef struct Renderer Renderer;
typedef struct RendererInterface RendererInterface;

/**
 * @brief Interleaved position + texcoord + color vertex for GPU upload.
 */
typedef struct {
  vec2 position;
  vec2 uv;
  SDL_Color color;
} MVC_Vertex;

/**
 * @brief A recorded draw call for deferred GPU submission.
 * @private
 */
typedef struct {
  Texture *texture;
  SDL_Rect scissor;
  Uint32 firstVertex;
  Uint32 vertexCount;
} MVC_DrawArrays;

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
   * @brief The current frame command buffer (valid between beginFrame and endFrame).
   * @private
   */
  CommandBuffer *commands;

  /**
   * @brief The color format the Renderer's pipeline targets.
   * @details Initialized from the swapchain texture format in `renderDeviceDidReset`.
   *   The lazy pipeline rebuild in `WindowController::render` keeps this in sync with
   *   the Framebuffer passed each frame, so direct writes are rarely needed.
   */
  SDL_GPUTextureFormat colorFormat;

  /**
   * @brief The backing RenderDevice.
   */
  RenderDevice *device;

  /**
   * @brief CPU-side frame accumulation of draw arrays.
   * @private
   */
  Vector *drawArrays;

  /**
   * @brief The current frame Framebuffer (valid between beginFrame and endFrame).
   * @details Borrowed reference — valid only while the frame is in flight. Do not retain.
   * @private
   */
  Framebuffer *framebuffer;

  /**
   * @brief The graphics pipeline (TRIANGLELIST, for all MVC geometry).
   * @private
   */
  GraphicsPipeline *pipeline;

  /**
   * @brief The linear clamp-to-edge sampler for texture rendering.
   * @details Owned by this Renderer; created in `renderDeviceDidReset` and
   *   released in `renderDeviceWillReset`.
   * @private
   */
  Sampler *sampler;

  /**
   * @brief The current scissor rectangle (in pixel coordinates).
   * @private
   */
  SDL_Rect scissor;

  /**
   * @brief CPU-side frame accumulation of vertices.
   * @private
   */
  Vector *vertices;

  /**
   * @brief The GPU-side vertex buffer (resized as needed to fit vertices).
   * @private
   */
  Buffer *vertexBuffer;

  /**
   * @brief Capacity of vertexBuffer in vertices.
   * @private
   */
  Uint32 vertexBufferCapacity;

  /**
   * @brief The 1×1 white fallback texture (used for solid-color primitives).
   * @private
   */
  Texture *white;
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
   * @fn void Renderer::beginFrame(Renderer *self, CommandBuffer *commands, Framebuffer *framebuffer)
   * @brief Prepares this Renderer for a new frame using the given command buffer and framebuffer.
   * @param self The Renderer.
   * @param commands The frame's CommandBuffer. The caller retains ownership and must submit and release it.
   * @param framebuffer The target Framebuffer for this frame. Borrowed for the duration of the frame.
   * @memberof Renderer
   */
  void (*beginFrame)(Renderer *self, CommandBuffer *commands, Framebuffer *framebuffer);

  /**
   * @fn void Renderer::drawLine(const Renderer *self, const SDL_Point *points, const SDL_Color *color)
   * @brief Records a line segment between two points.
   * @param self The Renderer.
   * @param points Two points defining the line segment.
   * @param color The line color.
   * @memberof Renderer
   */
  void (*drawLine)(const Renderer *self, const SDL_Point *points, const SDL_Color *color);

  /**
   * @fn void Renderer::drawLines(const Renderer *self, const SDL_Point *points, size_t count, const SDL_Color *color)
   * @brief Records a polyline through the given points.
   * @param self The Renderer.
   * @param points The points.
   * @param count The number of points.
   * @param color The line color.
   * @memberof Renderer
   */
  void (*drawLines)(const Renderer *self, const SDL_Point *points, size_t count, const SDL_Color *color);

  /**
   * @fn void Renderer::drawRect(const Renderer *self, const SDL_Rect *rect, const SDL_Color *color)
   * @brief Records a rectangle outline.
   * @param self The Renderer.
   * @param rect The rectangle.
   * @param color The outline color.
   * @memberof Renderer
   */
  void (*drawRect)(const Renderer *self, const SDL_Rect *rect, const SDL_Color *color);

  /**
   * @fn void Renderer::drawRectFilled(const Renderer *self, const SDL_Rect *rect, const SDL_Color *color)
   * @brief Records a filled rectangle.
   * @param self The Renderer.
   * @param rect The rectangle.
   * @param color The fill color.
   * @memberof Renderer
   */
  void (*drawRectFilled)(const Renderer *self, const SDL_Rect *rect, const SDL_Color *color);

  /**
   * @fn void Renderer::drawTexture(const Renderer *self, Texture *texture, const SDL_Rect *dest, const SDL_Color *color)
   * @brief Records a textured quad in the given destination rectangle.
   * @param self The Renderer.
   * @param texture The Texture to sample.
   * @param dest The destination rectangle in logical screen coordinates.
   * @param color The color multiplier (use `&Colors.White` for no tint).
   * @memberof Renderer
   */
  void (*drawTexture)(const Renderer *self, Texture *texture, const SDL_Rect *dest, const SDL_Color *color);

  /**
   * @fn void Renderer::drawView(Renderer *self, View *view)
   * @brief Sets the clipping frame and invokes View::render for the given View.
   * @param self The Renderer.
   * @param view The View to render.
   * @memberof Renderer
   */
  void (*drawView)(Renderer *self, View *view);

  /**
   * @fn void Renderer::endFrame(Renderer *self, Framebuffer *framebuffer)
   * @brief Uploads MVC vertices and executes the UI render pass into the given framebuffer.
   * @details The caller is responsible for submitting the command buffer after this returns.
   * @param self The Renderer.
   * @param framebuffer The target Framebuffer to render the UI into (LOAD_OP_LOAD).
   * @memberof Renderer
   */
  void (*endFrame)(Renderer *self, Framebuffer *framebuffer);

  /**
   * @fn Renderer *Renderer::initWithDevice(Renderer *self, RenderDevice *device)
   * @brief Initializes this Renderer with the given RenderDevice.
   * @param self The Renderer.
   * @param device The RenderDevice.
   * @return The initialized Renderer, or `NULL` on error.
   * @memberof Renderer
   */
  Renderer *(*initWithDevice)(Renderer *self, RenderDevice *device);

  /**
   * @fn void Renderer::pushDrawArrays(const Renderer *self, const MVC_Vertex *verts, size_t count, Texture *texture, const SDL_Color *color)
   * @brief Appends raw vertices and a draw call record to the frame queue.
   * @details Views that need full draw-call control can call this directly
   *   instead of going through the drawLine/drawRect/drawTexture helpers.
   * @param self The Renderer.
   * @param verts The vertices to append (in logical screen coordinates).
   * @param count The number of vertices.
   * @param texture The texture to bind, or `NULL` to use the 1×1 white fallback.
   * @param color The color multiplier applied in the fragment shader.
   * @memberof Renderer
   */
  void (*pushDrawArrays)(const Renderer *self, const MVC_Vertex *verts, size_t count,
                         Texture *texture, const SDL_Color *color);

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
   * @fn void Renderer::setClippingFrame(Renderer *self, const SDL_Rect *clippingFrame)
   * @brief Sets the scissor rectangle for subsequent draw calls.
   * @param self The Renderer.
   * @param clippingFrame The clipping rectangle in logical screen coordinates, or
   *   `NULL` to disable clipping (full window scissor).
   * @memberof Renderer
   */
  void (*setClippingFrame)(Renderer *self, const SDL_Rect *clippingFrame);
};

/**
 * @fn Class *Renderer::_Renderer(void)
 * @brief The Renderer archetype.
 * @return The Renderer Class.
 * @memberof Renderer
 */
OBJECTIVELYMVC_EXPORT Class *_Renderer(void);
