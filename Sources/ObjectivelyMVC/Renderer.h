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

#include <Objectively/Array.h>

#include <ObjectivelyMVC/Renderer+SDLgpu.h>

/**
 * @file
 * @brief The Renderer is responsible for rasterizing the View hierarchy of a WindowController.
 * @details This class provides an SDL_gpu (Metal/Vulkan/Direct3D 12) implementation.
 * The application must create an SDL_Window before instantiating a WindowController.
 * The Renderer creates its own SDL_GPUDevice and claims the window automatically via
 * `renderDeviceDidReset`. Applications may subclass Renderer to use a custom pipeline
 * or integrate with an existing SDL_GPUDevice.
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
  bool fill;
  Uint32 first_vertex;
  Uint32 vertex_count;
  SDL_GPUTexture *texture;
  float color[4];
  SDL_Rect scissor;
  bool has_scissor;
} MVC_DrawCall;

/**
 * @brief The Renderer is responsible for rasterizing the View hierarchy of a WindowController.
 * @details This class provides an SDL_gpu (Metal/Vulkan/Direct3D 12) implementation.
 * Create a WindowController to instantiate a default Renderer, or supply your own
 * subclass via WindowController::setRenderer.
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
   * @brief The current draw color.
   * @private
   */
  SDL_Color color;

  /**
   * @brief The SDL_GPU device.
   * @private
   */
  SDL_GPUDevice *device;

  /**
   * @brief The fill graphics pipeline (TRIANGLESTRIP, for rects and textured quads).
   * @private
   */
  SDL_GPUGraphicsPipeline *fill_pipeline;

  /**
   * @brief The line graphics pipeline (LINESTRIP, for outlines and line segments).
   * @private
   */
  SDL_GPUGraphicsPipeline *line_pipeline;

  /**
   * @brief The GPU-side vertex buffer.
   * @private
   */
  SDL_GPUBuffer *vertex_buffer;

  /**
   * @brief The CPU-to-GPU vertex transfer buffer (staging).
   * @private
   */
  SDL_GPUTransferBuffer *vertex_transfer;

  /**
   * @brief The linear texture sampler.
   * @private
   */
  SDL_GPUSampler *sampler;

  /**
   * @brief The 1×1 white fallback texture (used for solid-color primitives).
   * @private
   */
  SDL_GPUTexture *white;

  /**
   * @brief The window currently being rendered.
   * @private
   */
  SDL_Window *window;

  /**
   * @brief When true, the render pass clears to @c clear_color before drawing.
   * @details Defaults to `true` (clear to opaque black). Set to `false` when
   * rendering the UI on top of an existing GPU frame (e.g. a 3D game scene
   * that has already written to the swapchain texture).
   */
  bool clear;

  /**
   * @brief The color used to clear the swapchain texture when @c clear is true.
   * @details Defaults to opaque black `{0, 0, 0, 1}`.
   */
  SDL_FColor clear_color;

  /**
   * @brief The current frame command buffer (valid between beginFrame and endFrame).
   * @private
   */
  SDL_GPUCommandBuffer *cmd;

  /**
   * @brief The swapchain texture acquired for this frame.
   * @private
   */
  SDL_GPUTexture *swapchain_texture;

  /**
   * @brief The swapchain dimensions for this frame.
   * @private
   */
  Uint32 swapchain_w, swapchain_h;

  /**
   * @brief CPU-mapped pointer into vertex_transfer (valid between beginFrame and endFrame).
   * @private
   */
  MVC_Vertex *vertex_staging;

  /**
   * @brief Number of vertices written this frame.
   * @private
   */
  Uint32 vertex_count;

  /**
   * @brief Draw call queue for this frame (heap-allocated).
   * @private
   */
  MVC_DrawCall *draw_calls;

  /**
   * @brief Number of draw calls recorded this frame.
   * @private
   */
  Uint32 draw_call_count;

  /**
   * @brief The current scissor rectangle (in pixel coordinates).
   * @private
   */
  SDL_Rect current_scissor;

  /**
   * @brief True if a scissor rect is active.
   * @private
   */
  bool has_scissor;
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
   * @brief Acquires the swapchain texture and prepares for draw calls.
   * @param self The Renderer.
   * @remarks Called by WindowController to begin rendering each frame. Override
   * to perform custom setup before the first draw call.
   * @memberof Renderer
   */
  void (*beginFrame)(Renderer *self);

  /**
   * @fn SDL_GPUTexture *Renderer::createTexture(const Renderer *self, const SDL_Surface *surface)
   * @brief Creates a GPU texture from the given surface, uploading its pixels.
   * @param self The Renderer.
   * @param surface The surface to upload.
   * @return A new SDL_GPUTexture on success, or `NULL` on error.
   * @remarks The caller owns the returned texture; release it with MVC_ReleaseGPUTexture().
   * @memberof Renderer
   */
  SDL_GPUTexture *(*createTexture)(const Renderer *self, const SDL_Surface *surface);

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
   * @fn void Renderer::endFrame(Renderer *self)
   * @brief Flushes recorded draw calls to the GPU and submits the command buffer.
   * @param self The Renderer.
   * @remarks Called by WindowController after all draw calls for a frame. Does not
   * swap buffers; swapping is handled by SDL_gpu's swapchain presentation.
   * @memberof Renderer
   */
  void (*endFrame)(Renderer *self);

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
   * @fn void Renderer::renderDeviceDidReset(Renderer *self)
   * @brief Invoked when the render device has been (re-)initialized.
   * @param self The Renderer.
   * @remarks Creates the SDL_GPUDevice, shaders, pipelines, and GPU buffers.
   * Subclasses may override to allocate additional GPU resources.
   * @memberof Renderer
   */
  void (*renderDeviceDidReset)(Renderer *self);

  /**
   * @fn void Renderer::renderDeviceWillReset(Renderer *self)
   * @brief Invoked when the render device is about to be destroyed.
   * @param self The Renderer.
   * @remarks Releases all GPU resources owned by this Renderer.
   * Subclasses should override to free any additional GPU resources they own.
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

  /**
   * @fn void Renderer::setDrawColor(Renderer *self, const SDL_Color *color)
   * @brief Sets the primary color multiplier for subsequent draw calls.
   * @param self The Renderer.
   * @param color The color.
   * @memberof Renderer
   */
  void (*setDrawColor)(Renderer *self, const SDL_Color *color);

  /**
   * @fn void Renderer::setWindow(Renderer *self, SDL_Window *window)
   * @brief Binds this Renderer to the given window.
   * @param self The Renderer.
   * @param window The SDL_Window to render into.
   * @remarks Called automatically by WindowController when its window changes.
   * @memberof Renderer
   */
  void (*setWindow)(Renderer *self, SDL_Window *window);
};

/**
 * @fn Class *Renderer::_Renderer(void)
 * @brief The Renderer archetype.
 * @return The Renderer Class.
 * @memberof Renderer
 */
OBJECTIVELYMVC_EXPORT Class *_Renderer(void);
