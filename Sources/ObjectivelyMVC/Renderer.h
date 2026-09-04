/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and C.
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
 * @brief Interleaved position + texcoord + color + rounded rect shape vertex for GPU upload.
 * @details `rect` holds the rounded rectangle's center (`xy`) and half extent (`zw`), and
 * `shape` its corner radius (`x`) and stroke width (`y`, `0` for filled), all in logical
 * coordinates. Leave both zeroed for plain quads; the shader skips the SDF when `rect.z == 0`.
 */
typedef struct {
  vec2 position;
  vec2 uv;
  SDL_Color color;
  vec4 rect;
  vec2 shape;
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
 * @details Create a WindowController to instantiate a default Renderer.
 * @extends Object
 */
struct Renderer {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface type.
   * @protected
   */
  RendererInterface *interface[0];

  /**
   * @brief The current frame command buffer (valid between beginFrame and endFrame).
   * @private
   */
  CommandBuffer *commands;

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
   * @brief The transfer buffer sourcing the vertex upload, held for the renderer's
   * lifetime because the vertices are uploaded every frame. Grown with the buffer it
   * sources.
   * @private
   */
  TransferBuffer *transferBuffer;

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
   * @fn void Renderer::beginFrame(Renderer *self)
   * @brief Prepares this Renderer for a new frame using `self->device`'s current command
   *   buffer and framebuffer.
   * @details Convenience over `beginFrameWith` for the common case of rendering the UI
   *   into the device's own current frame. Equivalent to
   *   `$(self, beginFrameWith, self->device->commands, self->device->framebuffer)`.
   * @param self The Renderer.
   * @memberof Renderer
   */
  void (*beginFrame)(Renderer *self);

  /**
   * @fn void Renderer::beginFrameWith(Renderer *self, CommandBuffer *commands, Framebuffer *framebuffer)
   * @brief Prepares this Renderer for a new frame using the given command buffer and framebuffer.
   * @param self The Renderer.
   * @param commands The frame's CommandBuffer. The caller retains ownership and must submit and release it.
   * @param framebuffer The target Framebuffer for this frame. Borrowed for the duration of the frame.
   * @memberof Renderer
   */
  void (*beginFrameWith)(Renderer *self, CommandBuffer *commands, Framebuffer *framebuffer);

  /**
   * @fn void Renderer::drawBevel(const Renderer *self, const SDL_Rect *rect, int radius, int width, const SDL_Color *topLeft, const SDL_Color *bottomRight)
   * @brief Records a rounded rectangle outline split diagonally into two colors.
   * @details The outline is drawn inside `rect`. The half above the bottom-left to top-right
   * diagonal takes `topLeft`; the other half takes `bottomRight`.
   * @param self The Renderer.
   * @param rect The rectangle.
   * @param radius The corner radius.
   * @param width The outline width.
   * @param topLeft The color of the top and left edges.
   * @param bottomRight The color of the bottom and right edges.
   * @memberof Renderer
   */
  void (*drawBevel)(const Renderer *self, const SDL_Rect *rect, int radius, int width,
                    const SDL_Color *topLeft, const SDL_Color *bottomRight);

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
   * @fn void Renderer::drawRoundedRect(const Renderer *self, const SDL_Rect *rect, int radius, int width, const SDL_Color *color)
   * @brief Records a rounded rectangle outline of the given width, drawn inside `rect`.
   * @param self The Renderer.
   * @param rect The rectangle.
   * @param radius The corner radius.
   * @param width The outline width.
   * @param color The outline color.
   * @memberof Renderer
   */
  void (*drawRoundedRect)(const Renderer *self, const SDL_Rect *rect, int radius, int width, const SDL_Color *color);

  /**
   * @fn void Renderer::drawRoundedRectFilled(const Renderer *self, const SDL_Rect *rect, int radius, const SDL_Color *color)
   * @brief Records a filled rounded rectangle.
   * @param self The Renderer.
   * @param rect The rectangle.
   * @param radius The corner radius.
   * @param color The fill color.
   * @memberof Renderer
   */
  void (*drawRoundedRectFilled)(const Renderer *self, const SDL_Rect *rect, int radius, const SDL_Color *color);

  /**
   * @fn void Renderer::drawRoundedTexture(const Renderer *self, Texture *texture, const SDL_FRect *dest, int radius, const SDL_Color *color)
   * @brief Records a textured quad clipped to a rounded rectangle.
   * @param self The Renderer.
   * @param texture The Texture to sample.
   * @param dest The destination rectangle in logical screen coordinates.
   * @param radius The corner radius.
   * @param color The color multiplier (use `&Colors.White` for no tint).
   * @memberof Renderer
   */
  void (*drawRoundedTexture)(const Renderer *self, Texture *texture, const SDL_FRect *dest, int radius, const SDL_Color *color);

  /**
   * @fn void Renderer::drawTexture(const Renderer *self, Texture *texture, const SDL_FRect *dest, const SDL_Color *color)
   * @brief Records a textured quad in the given destination rectangle.
   * @param self The Renderer.
   * @param texture The Texture to sample.
   * @param dest The destination rectangle in logical screen coordinates.
   * @remarks `dest` is a sub-pixel-precise SDL_FRect, not SDL_Rect: a caller that needs the
   * texture drawn at its exact native resolution (e.g. Text, to avoid a fractional stretch that
   * shifts every glyph as the string's pixel width changes) must pass an unrounded size here,
   * since rounding it to the nearest integer logical unit first is precisely what reintroduces
   * that stretch.
   * @param color The color multiplier (use `&Colors.White` for no tint).
   * @memberof Renderer
   */
  void (*drawTexture)(const Renderer *self, Texture *texture, const SDL_FRect *dest, const SDL_Color *color);

  /**
   * @fn void Renderer::drawView(Renderer *self, View *view)
   * @brief Sets the clipping frame and invokes View::render for the given View.
   * @details A View whose View::clippingFrame is empty is culled. Otherwise the scissor is set
   * to the View::clippingFrame of the nearest ancestor with `clipsSubviews` set, or to the
   * full framebuffer when there is none. A View is therefore NOT clipped to its own frame:
   * one that draws outside its bounds does so visibly, and a container that wants to clip its
   * contents MUST set `clipsSubviews`.
   * @param self The Renderer.
   * @param view The View to render.
   * @memberof Renderer
   */
  void (*drawView)(Renderer *self, View *view);

  /**
   * @fn void Renderer::endFrame(Renderer *self)
   * @brief Uploads MVC vertices and executes the UI render pass into the Framebuffer
   *   given to `beginFrame`/`beginFrameWith` (LOAD_OP_LOAD).
   * @details The caller is responsible for submitting the command buffer after this returns.
   * @param self The Renderer.
   * @memberof Renderer
   */
  void (*endFrame)(Renderer *self);

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
   *   Consecutive calls that share both `texture` and the current scissor are merged into
   *   one draw call. Only the immediately preceding record is considered, so draw order
   *   matters: a quad from another texture between two same-texture quads breaks the run.
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
