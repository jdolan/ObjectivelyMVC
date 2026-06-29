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
#include <stdlib.h>
#include <string.h>

#include <Objectively/Vector.h>
#include <ObjectivelyGPU/CopyPass.h>
#include <ObjectivelyGPU/Mathlib.h>
#include <ObjectivelyGPU/RenderPass.h>

#include "Renderer.h"
#include "View.h"
#include "Window.h"

#include "../Assets/Renderer.vert.spv.h"
#include "../Assets/Renderer.vert.msl.h"
#include "../Assets/Renderer.frag.spv.h"
#include "../Assets/Renderer.frag.msl.h"

#define _Class _Renderer

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Renderer *this = (Renderer *) self;

  if (this->cmd) {
    $(this->cmd, cancel);
    release(this->cmd);
    this->cmd = NULL;
  }

  release(this->vertices);
  release(this->drawArrays);
  release(this->device);

  super(Object, self, dealloc);
}

#pragma mark - Renderer

/**
 * @fn void Renderer::beginFrame(Renderer *self)
 * @memberof Renderer
 */
static void beginFrame(Renderer *self) {

  self->cmd = $(self->device, acquireCommandBuffer);

  $(self->cmd, waitAndAcquireSwapchainTexture, &self->swapchain);

  $(self->vertices, removeAll);
  $(self->drawArrays, removeAll);

  self->scissor = MakeRect(0, 0, self->swapchain.size.w, self->swapchain.size.h);
}

/**
 * @fn void Renderer::drawLine(const Renderer *self, const SDL_Point *points, const SDL_Color *color)
 * @memberof Renderer
 */
static void drawLine(const Renderer *self, const SDL_Point *points, const SDL_Color *color) {

  assert(points);

  $(self, drawLines, points, 2, color);
}

/**
 * @fn void Renderer::drawLines(const Renderer *self, const SDL_Point *points, size_t count, const SDL_Color *color)
 * @memberof Renderer
 */
static void drawLines(const Renderer *self, const SDL_Point *points, size_t count, const SDL_Color *color) {

  assert(points);
  assert(color);

  if (count < 2) {
    return;
  }

  const size_t segCount = count - 1;
  MVC_Vertex *verts = malloc(segCount * 6 * sizeof(MVC_Vertex));
  assert(verts);

  for (size_t i = 0; i < segCount; i++) {
    const float ax = (float) points[i].x,     ay = (float) points[i].y;
    const float bx = (float) points[i+1].x,   by = (float) points[i+1].y;

    const float dx = bx - ax, dy = by - ay;
    const float len = sqrtf(dx * dx + dy * dy);

    float nx = 0.0f, ny = 0.0f;
    if (len > 0.001f) {
      nx = (-dy / len) * 0.5f;
      ny = ( dx / len) * 0.5f;
    }

    MVC_Vertex *v = &verts[i * 6];
    v[0] = (MVC_Vertex) { ax - nx, ay - ny, 0.0f, 0.0f };
    v[1] = (MVC_Vertex) { ax + nx, ay + ny, 0.0f, 0.0f };
    v[2] = (MVC_Vertex) { bx - nx, by - ny, 0.0f, 0.0f };
    v[3] = (MVC_Vertex) { ax + nx, ay + ny, 0.0f, 0.0f };
    v[4] = (MVC_Vertex) { bx + nx, by + ny, 0.0f, 0.0f };
    v[5] = (MVC_Vertex) { bx - nx, by - ny, 0.0f, 0.0f };
  }

  $(self, pushDrawArrays, verts, segCount * 6, NULL, color);

  free(verts);
}

/**
 * @fn void Renderer::drawRect(const Renderer *self, const SDL_Rect *rect, const SDL_Color *color)
 * @memberof Renderer
 */
static void drawRect(const Renderer *self, const SDL_Rect *rect, const SDL_Color *color) {

  assert(rect);

  const SDL_Point points[5] = {
    { rect->x,           rect->y           },
    { rect->x + rect->w, rect->y           },
    { rect->x + rect->w, rect->y + rect->h },
    { rect->x,           rect->y + rect->h },
    { rect->x,           rect->y           },
  };

  $(self, drawLines, points, 5, color);
}

/**
 * @fn void Renderer::drawRectFilled(const Renderer *self, const SDL_Rect *rect, const SDL_Color *color)
 * @memberof Renderer
 */
static void drawRectFilled(const Renderer *self, const SDL_Rect *rect, const SDL_Color *color) {

  assert(rect);

  const float x1 = (float) rect->x,           y1 = (float) rect->y;
  const float x2 = (float) rect->x + rect->w, y2 = (float) rect->y + rect->h;

  const MVC_Vertex verts[6] = {
    { x1, y1, 0.0f, 0.0f },
    { x2, y1, 0.0f, 0.0f },
    { x1, y2, 0.0f, 0.0f },
    { x2, y1, 0.0f, 0.0f },
    { x2, y2, 0.0f, 0.0f },
    { x1, y2, 0.0f, 0.0f },
  };

  $(self, pushDrawArrays, verts, 6, NULL, color);
}

/**
 * @fn void Renderer::drawTexture(const Renderer *self, SDL_GPUTexture *texture, const SDL_Rect *dest, const SDL_Color *color)
 * @memberof Renderer
 */
static void drawTexture(const Renderer *self, SDL_GPUTexture *texture, const SDL_Rect *rect, const SDL_Color *color) {

  assert(rect);

  const float x1 = (float) rect->x,           y1 = (float) rect->y;
  const float x2 = (float) rect->x + rect->w, y2 = (float) rect->y + rect->h;

  const MVC_Vertex verts[6] = {
    { x1, y1, 0.0f, 0.0f },
    { x2, y1, 1.0f, 0.0f },
    { x1, y2, 0.0f, 1.0f },
    { x2, y1, 1.0f, 0.0f },
    { x2, y2, 1.0f, 1.0f },
    { x1, y2, 0.0f, 1.0f },
  };

  $(self, pushDrawArrays, verts, 6, texture, color);
}

/**
 * @fn void Renderer::drawView(Renderer *self, View *view)
 * @memberof Renderer
 */
static void drawView(Renderer *self, View *view) {

  assert(view);

  const SDL_Rect clippingFrame = $(view, clippingFrame);
  if (clippingFrame.w && clippingFrame.h) {
    $(self, setClippingFrame, &clippingFrame);
    $(view, render, self);
  }
}

/**
 * @fn void Renderer::endFrame(Renderer *self)
 * @memberof Renderer
 */
static void endFrame(Renderer *self) {

  const size_t vtxCount = self->vertices->count;

  CopyPass *copyPass = $(self->cmd, beginCopyPass);

  if (vtxCount > 0) {
    const Uint32 vtxSize = (Uint32) (vtxCount * sizeof(MVC_Vertex));

    if (vtxCount > self->vertexBufferCapacity) {
      if (self->vertexBuffer) {
        $(self->device, releaseBuffer, self->vertexBuffer);
      }
      const SDL_GPUBufferCreateInfo info = { .usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = vtxSize };
      self->vertexBuffer = $(self->device, createBuffer, &info);
      self->vertexBufferCapacity = (Uint32) vtxCount;
    }

    $(copyPass, uploadData, self->vertexBuffer, self->vertices->elements, vtxSize, 0, true);
  }

  release(copyPass);

  const SDL_GPUColorTargetInfo colorTarget = {
    .texture = self->swapchain.texture,
    .load_op = self->clear ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD,
    .store_op = SDL_GPU_STOREOP_STORE,
    .clear_color = (SDL_FColor) { 0.f, 0.f, 0.f, 0.f },
  };

  RenderPass *renderPass = $(self->cmd, beginRenderPass, &colorTarget, 1, NULL);

  const SDL_GPUViewport viewport = {
    .x = 0.0f, .y = 0.0f,
    .w = (float) self->swapchain.size.w, .h = (float) self->swapchain.size.h,
    .min_depth = 0.0f, .max_depth = 1.0f,
  };
  $(renderPass, setViewport, &viewport);

  int winW, winH;
  SDL_GetWindowSize(self->device->window, &winW, &winH);
  const mat4 projection = mat4_ortho(0.f, (float) winW, (float) winH, 0.f, -1.f, 1.f);
  $(self->cmd, pushVertexUniformData, 0, projection.f, sizeof(projection));

  $(renderPass, bindPipeline, self->pipeline);
  $(renderPass, bindVertexBuffers, 0, &(SDL_GPUBufferBinding) { .buffer = self->vertexBuffer }, 1);

  for (size_t i = 0; i < self->drawArrays->count; i++) {
    const MVC_DrawArrays *draw = VectorElement(self->drawArrays, MVC_DrawArrays, i);

    const int swW = (int) self->swapchain.size.w;
    const int swH = (int) self->swapchain.size.h;
    const SDL_Rect scissor = {
      .x = SDL_max(draw->scissor.x, 0),
      .y = SDL_max(draw->scissor.y, 0),
      .w = SDL_min(draw->scissor.x + draw->scissor.w, swW) - SDL_max(draw->scissor.x, 0),
      .h = SDL_min(draw->scissor.y + draw->scissor.h, swH) - SDL_max(draw->scissor.y, 0),
    };

    if (scissor.w <= 0 || scissor.h <= 0) {
      continue;
    }

    $(renderPass, setScissor, &scissor);

    $(renderPass, bindFragmentSamplers, 0, &(SDL_GPUTextureSamplerBinding) {
      .texture = draw->texture, .sampler = self->sampler,
    }, 1);

    $(self->cmd, pushFragmentUniformData, 0, draw->color, sizeof(draw->color));
    $(renderPass, drawPrimitives, draw->vertexCount, 1, draw->firstVertex, 0);
  }

  release(renderPass);

  $(self->device, submit, self->cmd);
  release(self->cmd);
  self->cmd = NULL;
  self->swapchain = (SwapchainTexture) { 0 };
}

/**
 * @fn Renderer *Renderer::init(Renderer *self)
 * @memberof Renderer
 */
static Renderer *init(Renderer *self) {

  RenderDevice *device = $(alloc(RenderDevice), init);

  $(self, initWithDevice, device);

  release(device);

  return self;
}

/**
 * @fn Renderer *Renderer::initWithDevice(Renderer *self, RenderDevice *device)
 * @memberof Renderer
 */
static Renderer *initWithDevice(Renderer *self, RenderDevice *device) {

  self = (Renderer *) super(Object, self, init);
  if (self) {
    self->device = retain(device);
    assert(self->device);

    self->vertices = $(alloc(Vector), initWithSize, sizeof(MVC_Vertex));
    assert(self->vertices);

    self->drawArrays = $(alloc(Vector), initWithSize, sizeof(MVC_DrawArrays));
    assert(self->drawArrays);
  }

  return self;
}

/**
 * @fn void Renderer::pushDrawArrays(const Renderer *self, const MVC_Vertex *verts, size_t count, SDL_GPUTexture *texture, const SDL_Color *color)
 * @memberof Renderer
 */
static void pushDrawArrays(const Renderer *self, const MVC_Vertex *verts, size_t count, SDL_GPUTexture *texture, const SDL_Color *color) {

  assert(verts);
  assert(color);

  const MVC_DrawArrays draw = {
    .firstVertex = (Uint32) self->vertices->count,
    .vertexCount = (Uint32) count,
    .texture     = texture ? texture : self->white,
    .color       = {
      color->r / 255.0f,
      color->g / 255.0f,
      color->b / 255.0f,
      color->a / 255.0f,
    },
      .scissor = self->scissor,
  };

  for (size_t i = 0; i < count; i++) {
    $(self->vertices, add, (MVC_Vertex *) &verts[i]);
  }

  $(self->drawArrays, add, (MVC_DrawArrays *) &draw);
}

/**
 * @fn void Renderer::renderDeviceDidReset(Renderer *self)
 * @memberof Renderer
 */
static void renderDeviceDidReset(Renderer *self) {

  $(self, renderDeviceWillReset);

  SDL_GPUShaderCreateInfo vsInfo = {
    .stage               = SDL_GPU_SHADERSTAGE_VERTEX,
    .num_uniform_buffers = 1,
  };

  SDL_GPUShaderCreateInfo fsInfo = {
    .stage               = SDL_GPU_SHADERSTAGE_FRAGMENT,
    .num_samplers        = 1,
    .num_uniform_buffers = 1,
  };

  const SDL_GPUShaderFormat supported = SDL_GetGPUShaderFormats(self->device->device);
  if (supported & SDL_GPU_SHADERFORMAT_MSL) {
    vsInfo.code       = (const Uint8 *) Renderer_vert_msl;
    vsInfo.code_size  = Renderer_vert_msl_len - 1;
    vsInfo.entrypoint = "main0";
    vsInfo.format     = SDL_GPU_SHADERFORMAT_MSL;
    fsInfo.code       = (const Uint8 *) Renderer_frag_msl;
    fsInfo.code_size  = Renderer_frag_msl_len - 1;
    fsInfo.entrypoint = "main0";
    fsInfo.format     = SDL_GPU_SHADERFORMAT_MSL;
  } else if (supported & SDL_GPU_SHADERFORMAT_SPIRV) {
    vsInfo.code       = Renderer_vert_spv;
    vsInfo.code_size  = Renderer_vert_spv_len - 1;
    vsInfo.entrypoint = "main";
    vsInfo.format     = SDL_GPU_SHADERFORMAT_SPIRV;
    fsInfo.code       = Renderer_frag_spv;
    fsInfo.code_size  = Renderer_frag_spv_len - 1;
    fsInfo.entrypoint = "main";
    fsInfo.format     = SDL_GPU_SHADERFORMAT_SPIRV;
  } else {
    GPU_Assert(false, "Unsupported GPU shader format (need MSL or SPIRV)");
  }

  SDL_GPUShader *vs = $(self->device, createShader, &vsInfo);
  SDL_GPUShader *fs = $(self->device, createShader, &fsInfo);

  const SDL_GPUVertexBufferDescription vbDesc = {
    .slot               = 0,
    .pitch              = sizeof(MVC_Vertex),
    .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
    .instance_step_rate = 0,
  };

  SDL_GPUVertexAttribute vbAttrs[2] = {
    { .location = 0, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, .offset = 0 },
    { .location = 1, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, .offset = 8 },
  };

  const SDL_GPUTextureFormat swapchainFmt = $(self->device, getSwapchainTextureFormat, self->device->window);
  const SDL_GPUColorTargetDescription colorTarget = {
    .format = swapchainFmt,
    .blend_state = {
      .enable_blend          = true,
      .color_blend_op        = SDL_GPU_BLENDOP_ADD,
      .alpha_blend_op        = SDL_GPU_BLENDOP_ADD,
      .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
      .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
      .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
      .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
    },
  };

  const SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {
    .vertex_shader = vs,
    .fragment_shader = fs,
    .vertex_input_state = {
      .vertex_buffer_descriptions = &vbDesc,
      .num_vertex_buffers         = 1,
      .vertex_attributes          = vbAttrs,
      .num_vertex_attributes      = 2,
    },
    .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
    .rasterizer_state = {
      .fill_mode  = SDL_GPU_FILLMODE_FILL,
      .cull_mode  = SDL_GPU_CULLMODE_NONE,
      .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
    },
    .target_info = {
      .color_target_descriptions = &colorTarget,
      .num_color_targets         = 1,
    },
  };

  self->pipeline = $(self->device, createGraphicsPipeline, &pipelineInfo);

  $(self->device, releaseShader, vs);
  $(self->device, releaseShader, fs);

  self->sampler = $(self->device, samplerLinearClamp);

  const Uint8 whitePx[4] = { 255, 255, 255, 255 };
  const SDL_GPUTextureCreateInfo whiteInfo = {
    .type                 = SDL_GPU_TEXTURETYPE_2D,
    .format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
    .usage                = SDL_GPU_TEXTUREUSAGE_SAMPLER,
    .width                = 1,
    .height               = 1,
    .layer_count_or_depth = 1,
    .num_levels           = 1,
  };
  self->white = $(self->device, createTexture, &whiteInfo, whitePx);
}

/**
 * @fn void Renderer::renderDeviceWillReset(Renderer *self)
 * @memberof Renderer
 */
static void renderDeviceWillReset(Renderer *self) {

  if (self->cmd) {
    $(self->cmd, cancel);
    release(self->cmd);
    self->cmd = NULL;
    self->swapchain = (SwapchainTexture) { 0 };
  }

  $(self->device, releaseTexture, self->white);
  self->white = NULL;

  // sampler is device-owned (samplerLinearClamp preset) — do not release
  self->sampler = NULL;

  $(self->device, releaseBuffer, self->vertexBuffer);
  self->vertexBuffer = NULL;
  self->vertexBufferCapacity = 0;

  $(self->device, releaseGraphicsPipeline, self->pipeline);
  self->pipeline = NULL;

  $(self->vertices, removeAll);
  $(self->drawArrays, removeAll);
}

/**
 * @fn void Renderer::setClippingFrame(Renderer *self, const SDL_Rect *clippingFrame)
 * @memberof Renderer
 */
static void setClippingFrame(Renderer *self, const SDL_Rect *clippingFrame) {

  if (clippingFrame) {
    self->scissor = MVC_TransformToWindow(self->device->window, clippingFrame);
  } else {
    self->scissor = MakeRect(0, 0, self->swapchain.size.w, self->swapchain.size.h);
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((RendererInterface *) clazz->interface)->beginFrame = beginFrame;
  ((RendererInterface *) clazz->interface)->drawLine = drawLine;
  ((RendererInterface *) clazz->interface)->drawLines = drawLines;
  ((RendererInterface *) clazz->interface)->drawRect = drawRect;
  ((RendererInterface *) clazz->interface)->drawRectFilled = drawRectFilled;
  ((RendererInterface *) clazz->interface)->drawTexture = drawTexture;
  ((RendererInterface *) clazz->interface)->drawView = drawView;
  ((RendererInterface *) clazz->interface)->endFrame = endFrame;
  ((RendererInterface *) clazz->interface)->init = init;
  ((RendererInterface *) clazz->interface)->pushDrawArrays = pushDrawArrays;
  ((RendererInterface *) clazz->interface)->initWithDevice = initWithDevice;
  ((RendererInterface *) clazz->interface)->renderDeviceDidReset = renderDeviceDidReset;
  ((RendererInterface *) clazz->interface)->renderDeviceWillReset = renderDeviceWillReset;
  ((RendererInterface *) clazz->interface)->setClippingFrame = setClippingFrame;
}

/**
 * @fn Class *Renderer::_Renderer(void)
 * @memberof Renderer
 */
Class *_Renderer(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name            = "Renderer",
      .superclass      = _Object(),
      .instanceSize    = sizeof(Renderer),
      .interfaceOffset = offsetof(Renderer, interface),
      .interfaceSize   = sizeof(RendererInterface),
      .initialize      = initialize,
    });
  });

  return clazz;
}

#undef _Class
