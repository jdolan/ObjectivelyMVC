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

#include "Colors.h"
#include "Log.h"
#include "Renderer+SDLgpu.h"
#include "Renderer.h"
#include "View.h"
#include "Window.h"

#define _Class _Renderer

#define MVC_MAX_VERTICES  16384
#define MVC_MAX_DRAW_CALLS 1024

/**
 * @brief Records vertices into the staging buffer and appends a DrawCall.
 */
static void pushDrawCall(const Renderer *self, bool fill, const MVC_Vertex *verts, Uint32 count,
                         SDL_GPUTexture *texture) {

  if (!self->vertex_staging) {
    return;
  }
  assert(self->vertex_count + count <= MVC_MAX_VERTICES);
  assert(self->draw_call_count < MVC_MAX_DRAW_CALLS);

  memcpy(self->vertex_staging + self->vertex_count, verts, count * sizeof(MVC_Vertex));

  MVC_DrawCall *dc = &self->draw_calls[self->draw_call_count];
  dc->fill         = fill;
  dc->first_vertex = self->vertex_count;
  dc->vertex_count = count;
  dc->texture      = texture ? texture : self->white;
  dc->color[0]     = self->color.r / 255.0f;
  dc->color[1]     = self->color.g / 255.0f;
  dc->color[2]     = self->color.b / 255.0f;
  dc->color[3]     = self->color.a / 255.0f;
  dc->scissor      = self->current_scissor;
  dc->has_scissor  = self->has_scissor;

  ((Renderer *) self)->vertex_count    += count;
  ((Renderer *) self)->draw_call_count += 1;
}

#pragma mark - Renderer

/**
 * @fn void Renderer::beginFrame(Renderer *self)
 * @memberof Renderer
 */
static void beginFrame(Renderer *self) {

  assert(self->device);
  assert(self->window);

  mvc_current_window = self->window;

  self->cmd = SDL_AcquireGPUCommandBuffer(self->device);
  if (!self->cmd) {
    MVC_LogError("beginFrame: SDL_AcquireGPUCommandBuffer: %s\n", SDL_GetError());
    return;
  }

  bool ok = SDL_AcquireGPUSwapchainTexture(
    self->cmd, self->window, &self->swapchain_texture, &self->swapchain_w, &self->swapchain_h);

  if (!ok || !self->swapchain_texture) {
    SDL_CancelGPUCommandBuffer(self->cmd);
    self->cmd = NULL;
    return;
  }

  self->vertex_staging = (MVC_Vertex *) SDL_MapGPUTransferBuffer(self->device, self->vertex_transfer, true);
  self->vertex_count   = 0;
  self->draw_call_count = 0;

  // Full-window default scissor (in pixels).
  self->current_scissor = MakeRect(0, 0, (int) self->swapchain_w, (int) self->swapchain_h);
  self->has_scissor = false;

  $(self, setDrawColor, &Colors.White);
}

/**
 * @fn SDL_GPUTexture *Renderer::createTexture(const Renderer *self, const SDL_Surface *surface)
 * @memberof Renderer
 */
static SDL_GPUTexture *createTexture(const Renderer *self, const SDL_Surface *surface) {

  assert(surface);

  // Convert the surface to RGBA32 for a uniform upload path.
  // Single-channel (grayscale) surfaces are treated as alpha masks: we convert
  // them to RGBA with R=G=B=255 and A=source so the white tint in the shader
  // produces correctly-coloured, anti-aliased glyphs.
  SDL_Surface *rgba = NULL;
  const bool is_alpha_mask = (SDL_BYTESPERPIXEL(surface->format) == 1);

  if (is_alpha_mask) {
    rgba = SDL_CreateSurface(surface->w, surface->h, SDL_PIXELFORMAT_RGBA32);
    if (!rgba) {
      MVC_LogError("createTexture: SDL_CreateSurface: %s\n", SDL_GetError());
      return NULL;
    }
    const Uint8 *src = (const Uint8 *) surface->pixels;
    Uint32 *dst = (Uint32 *) rgba->pixels;
    for (int y = 0; y < surface->h; y++) {
      for (int x = 0; x < surface->w; x++) {
        const Uint8 a = src[y * surface->pitch + x];
        dst[y * rgba->w + x] = SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32), NULL, 255, 255, 255, a);
      }
    }
  } else if (surface->format != SDL_PIXELFORMAT_RGBA32) {
    rgba = SDL_ConvertSurface((SDL_Surface *) surface, SDL_PIXELFORMAT_RGBA32);
    if (!rgba) {
      MVC_LogError("createTexture: SDL_ConvertSurface: %s\n", SDL_GetError());
      return NULL;
    }
  }

  const SDL_Surface *upload = rgba ? rgba : surface;

  SDL_GPUTextureCreateInfo tex_info = {
    .type               = SDL_GPU_TEXTURETYPE_2D,
    .format             = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
    .usage              = SDL_GPU_TEXTUREUSAGE_SAMPLER,
    .width              = (Uint32) upload->w,
    .height             = (Uint32) upload->h,
    .layer_count_or_depth = 1,
    .num_levels         = 1,
  };

  SDL_GPUTexture *texture = SDL_CreateGPUTexture(self->device, &tex_info);
  if (!texture) {
    MVC_LogError("createTexture: SDL_CreateGPUTexture: %s\n", SDL_GetError());
    if (rgba) SDL_DestroySurface(rgba);
    return NULL;
  }

  const Uint32 pixel_size = (Uint32) (upload->w * upload->h * 4);

  SDL_GPUTransferBufferCreateInfo tbuf_info = {
    .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    .size  = pixel_size,
  };

  SDL_GPUTransferBuffer *tbuf = SDL_CreateGPUTransferBuffer(self->device, &tbuf_info);
  if (!tbuf) {
    MVC_LogError("createTexture: SDL_CreateGPUTransferBuffer: %s\n", SDL_GetError());
    SDL_ReleaseGPUTexture(self->device, texture);
    if (rgba) SDL_DestroySurface(rgba);
    return NULL;
  }

  void *mapped = SDL_MapGPUTransferBuffer(self->device, tbuf, false);
  memcpy(mapped, upload->pixels, pixel_size);
  SDL_UnmapGPUTransferBuffer(self->device, tbuf);

  if (rgba) {
    SDL_DestroySurface(rgba);
  }

  SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(self->device);
  SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd);

  SDL_GPUTextureTransferInfo src = {
    .transfer_buffer = tbuf,
    .offset          = 0,
    .pixels_per_row  = (Uint32) upload->w,
    .rows_per_layer  = (Uint32) upload->h,
  };
  SDL_GPUTextureRegion dst = {
    .texture = texture,
    .w       = (Uint32) upload->w,
    .h       = (Uint32) upload->h,
    .d       = 1,
  };
  SDL_UploadToGPUTexture(copy_pass, &src, &dst, false);

  SDL_EndGPUCopyPass(copy_pass);
  SDL_SubmitGPUCommandBuffer(cmd);

  SDL_ReleaseGPUTransferBuffer(self->device, tbuf);

  return texture;
}

/**
 * @fn void Renderer::drawLine(const Renderer *self, const SDL_Point *points)
 * @memberof Renderer
 */
static void drawLine(const Renderer *self, const SDL_Point *points) {

  assert(points);

  $(self, drawLines, points, 2);
}

/**
 * @fn void Renderer::drawLines(const Renderer *self, const SDL_Point *points, size_t count)
 * @memberof Renderer
 */
static void drawLines(const Renderer *self, const SDL_Point *points, size_t count) {

  assert(points);

  MVC_Vertex *verts = malloc(count * sizeof(MVC_Vertex));
  assert(verts);

  for (size_t i = 0; i < count; i++) {
    verts[i] = (MVC_Vertex) { (float) points[i].x, (float) points[i].y, 0.0f, 0.0f };
  }

  pushDrawCall(self, false, verts, (Uint32) count, NULL);

  free(verts);
}

/**
 * @fn void Renderer::drawRect(const Renderer *self, const SDL_Rect *rect)
 * @memberof Renderer
 */
static void drawRect(const Renderer *self, const SDL_Rect *rect) {

  assert(rect);

  // LINESTRIP: 5 vertices closing the loop.
  const MVC_Vertex verts[5] = {
    { (float) rect->x,           (float) rect->y,           0.0f, 0.0f },
    { (float) rect->x + rect->w, (float) rect->y,           0.0f, 0.0f },
    { (float) rect->x + rect->w, (float) rect->y + rect->h, 0.0f, 0.0f },
    { (float) rect->x,           (float) rect->y + rect->h, 0.0f, 0.0f },
    { (float) rect->x,           (float) rect->y,           0.0f, 0.0f },
  };

  pushDrawCall(self, false, verts, 5, NULL);
}

/**
 * @fn void Renderer::drawRectFilled(const Renderer *self, const SDL_Rect *rect)
 * @memberof Renderer
 */
static void drawRectFilled(const Renderer *self, const SDL_Rect *rect) {

  assert(rect);

  const float x1 = (float) rect->x,          y1 = (float) rect->y;
  const float x2 = (float) rect->x + rect->w, y2 = (float) rect->y + rect->h;

  // TRIANGLESTRIP: TL, TR, BL, BR.
  const MVC_Vertex verts[4] = {
    { x1, y1, 0.0f, 0.0f },
    { x2, y1, 0.0f, 0.0f },
    { x1, y2, 0.0f, 0.0f },
    { x2, y2, 0.0f, 0.0f },
  };

  pushDrawCall(self, true, verts, 4, NULL);
}

/**
 * @fn void Renderer::drawTexture(const Renderer *self, SDL_GPUTexture *texture, const SDL_Rect *dest)
 * @memberof Renderer
 */
static void drawTexture(const Renderer *self, SDL_GPUTexture *texture, const SDL_Rect *rect) {

  assert(rect);

  // TRIANGLESTRIP: TL, TR, BL, BR with full [0,1] texcoords.
  const MVC_Vertex verts[4] = {
    { (float) rect->x,           (float) rect->y,           0.0f, 0.0f },
    { (float) rect->x + rect->w, (float) rect->y,           1.0f, 0.0f },
    { (float) rect->x,           (float) rect->y + rect->h, 0.0f, 1.0f },
    { (float) rect->x + rect->w, (float) rect->y + rect->h, 1.0f, 1.0f },
  };

  pushDrawCall(self, true, verts, 4, texture);
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

  if (!self->cmd || !self->swapchain_texture) {
    return;
  }

  SDL_UnmapGPUTransferBuffer(self->device, self->vertex_transfer);
  self->vertex_staging = NULL;

  // Upload vertices from the transfer buffer to the GPU vertex buffer.
  if (self->vertex_count > 0) {
    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(self->cmd);

    SDL_GPUTransferBufferLocation src = {
      .transfer_buffer = self->vertex_transfer,
      .offset          = 0,
    };
    SDL_GPUBufferRegion dst = {
      .buffer = self->vertex_buffer,
      .offset = 0,
      .size   = self->vertex_count * sizeof(MVC_Vertex),
    };
    SDL_UploadToGPUBuffer(copy_pass, &src, &dst, true);

    SDL_EndGPUCopyPass(copy_pass);
  }

  // Begin the render pass targeting the swapchain texture.
  SDL_GPUColorTargetInfo color_target = {
    .texture     = self->swapchain_texture,
    .load_op     = self->clear ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD,
    .store_op    = SDL_GPU_STOREOP_STORE,
    .clear_color = self->clear_color,
  };

  SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(self->cmd, &color_target, 1, NULL);

  // Viewport: full swapchain in pixels, Y-down (SDL_gpu normalises clip-space Y).
  SDL_GPUViewport viewport = {
    .x = 0.0f, .y = 0.0f,
    .w = (float) self->swapchain_w, .h = (float) self->swapchain_h,
    .min_depth = 0.0f, .max_depth = 1.0f,
  };
  SDL_SetGPUViewport(render_pass, &viewport);

  // Orthographic projection: logical (CSS) coords → clip space.
  // Vertices use logical window points; the viewport handles the physical-pixel
  // scaling automatically. Negate Y so screen-Y (down) maps to clip-Y (up).
  int win_w, win_h;
  SDL_GetWindowSize(self->window, &win_w, &win_h);
  const float lw = (float) win_w;
  const float lh = (float) win_h;
  const float projection[16] = {
     2.0f / lw,  0.0f,       0.0f,  0.0f,
     0.0f,      -2.0f / lh,  0.0f,  0.0f,
     0.0f,       0.0f,      -1.0f,  0.0f,
    -1.0f,       1.0f,       0.0f,  1.0f,
  };
  SDL_PushGPUVertexUniformData(self->cmd, 0, projection, sizeof(projection));

  // Bind the vertex buffer once; it's shared for all draw calls.
  SDL_GPUBufferBinding vb_binding = {
    .buffer = self->vertex_buffer,
    .offset = 0,
  };

  SDL_GPUGraphicsPipeline *current_pipeline = NULL;

  for (Uint32 i = 0; i < self->draw_call_count; i++) {
    const MVC_DrawCall *dc = &self->draw_calls[i];

    SDL_GPUGraphicsPipeline *pipeline = dc->fill ? self->fill_pipeline : self->line_pipeline;
    if (pipeline != current_pipeline) {
      SDL_BindGPUGraphicsPipeline(render_pass, pipeline);
      SDL_BindGPUVertexBuffers(render_pass, 0, &vb_binding, 1);
      current_pipeline = pipeline;
    }

    if (dc->has_scissor) {
      SDL_SetGPUScissor(render_pass, &dc->scissor);
    } else {
      SDL_Rect full = MakeRect(0, 0, (int) self->swapchain_w, (int) self->swapchain_h);
      SDL_SetGPUScissor(render_pass, &full);
    }

    SDL_GPUTextureSamplerBinding tex_bind = {
      .texture = dc->texture,
      .sampler = self->sampler,
    };
    SDL_BindGPUFragmentSamplers(render_pass, 0, &tex_bind, 1);

    SDL_PushGPUFragmentUniformData(self->cmd, 0, dc->color, sizeof(dc->color));

    SDL_DrawGPUPrimitives(render_pass, dc->vertex_count, 1, dc->first_vertex, 0);
  }

  SDL_EndGPURenderPass(render_pass);
  SDL_SubmitGPUCommandBuffer(self->cmd);

  self->cmd               = NULL;
  self->swapchain_texture = NULL;

  $(self, setDrawColor, &Colors.White);
}

/**
 * @fn Renderer *Renderer::init(Renderer *self)
 * @memberof Renderer
 */
static Renderer *init(Renderer *self) {

  self = (Renderer *) super(Object, self, init);
  if (self) {
    self->draw_calls = malloc(MVC_MAX_DRAW_CALLS * sizeof(MVC_DrawCall));
    assert(self->draw_calls);
    self->clear       = true;
    self->clear_color = (SDL_FColor) { 0.f, 0.f, 0.f, 1.f };
  }
  return self;
}

/**
 * @fn void Renderer::renderDeviceDidReset(Renderer *self)
 * @memberof Renderer
 */
static void renderDeviceDidReset(Renderer *self) {

  $(self, renderDeviceWillReset);

  if (!self->window) {
    MVC_LogError("renderDeviceDidReset: no window set\n");
    return;
  }

  // Create the GPU device, preferring MSL on Metal platforms.
  SDL_GPUShaderFormat formats =
    SDL_GPU_SHADERFORMAT_MSL |
    SDL_GPU_SHADERFORMAT_SPIRV |
    SDL_GPU_SHADERFORMAT_DXIL;

  self->device = SDL_CreateGPUDevice(formats, false, NULL);
  if (!self->device) {
    MVC_LogError("renderDeviceDidReset: SDL_CreateGPUDevice: %s\n", SDL_GetError());
    return;
  }

  mvc_gpu_device = self->device;

  if (!SDL_ClaimWindowForGPUDevice(self->device, self->window)) {
    MVC_LogError("renderDeviceDidReset: SDL_ClaimWindowForGPUDevice: %s\n", SDL_GetError());
    SDL_DestroyGPUDevice(self->device);
    self->device = NULL;
    mvc_gpu_device = NULL;
    return;
  }

  // Determine which shader format to use.
  const SDL_GPUShaderFormat supported = SDL_GetGPUShaderFormats(self->device);
  SDL_GPUShaderFormat fmt;
  const char *vs_code, *fs_code;
  const char *vs_entry, *fs_entry;

  if (supported & SDL_GPU_SHADERFORMAT_MSL) {
    fmt      = SDL_GPU_SHADERFORMAT_MSL;
    vs_code  = MVC_VertexShaderMSL;
    fs_code  = MVC_FragmentShaderMSL;
    vs_entry = "vs_main";
    fs_entry = "fs_main";
  } else {
    MVC_LogError("renderDeviceDidReset: no supported shader format (need MSL, SPIRV, or DXIL)\n");
    SDL_ReleaseWindowFromGPUDevice(self->device, self->window);
    SDL_DestroyGPUDevice(self->device);
    self->device = NULL;
    mvc_gpu_device = NULL;
    return;
  }

  SDL_GPUShaderCreateInfo vs_info = {
    .code              = (const Uint8 *) vs_code,
    .code_size         = strlen(vs_code),
    .entrypoint        = vs_entry,
    .format            = fmt,
    .stage             = SDL_GPU_SHADERSTAGE_VERTEX,
    .num_uniform_buffers = 1,
  };

  SDL_GPUShaderCreateInfo fs_info = {
    .code              = (const Uint8 *) fs_code,
    .code_size         = strlen(fs_code),
    .entrypoint        = fs_entry,
    .format            = fmt,
    .stage             = SDL_GPU_SHADERSTAGE_FRAGMENT,
    .num_samplers      = 1,
    .num_uniform_buffers = 1,
  };

  SDL_GPUShader *vs = SDL_CreateGPUShader(self->device, &vs_info);
  SDL_GPUShader *fs = SDL_CreateGPUShader(self->device, &fs_info);

  if (!vs || !fs) {
    MVC_LogError("renderDeviceDidReset: shader creation failed: %s\n", SDL_GetError());
    if (vs) SDL_ReleaseGPUShader(self->device, vs);
    if (fs) SDL_ReleaseGPUShader(self->device, fs);
    SDL_ReleaseWindowFromGPUDevice(self->device, self->window);
    SDL_DestroyGPUDevice(self->device);
    self->device = NULL;
    mvc_gpu_device = NULL;
    return;
  }

  // Vertex input: interleaved float2 position + float2 texcoord.
  SDL_GPUVertexBufferDescription vb_desc = {
    .slot            = 0,
    .pitch           = sizeof(MVC_Vertex),
    .input_rate      = SDL_GPU_VERTEXINPUTRATE_VERTEX,
    .instance_step_rate = 0,
  };
  SDL_GPUVertexAttribute vb_attrs[2] = {
    { .location = 0, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, .offset = 0 },
    { .location = 1, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, .offset = 8 },
  };

  // Alpha-blended colour target matching the swapchain format.
  const SDL_GPUTextureFormat swapchain_fmt = SDL_GetGPUSwapchainTextureFormat(self->device, self->window);
  SDL_GPUColorTargetDescription color_target = {
    .format = swapchain_fmt,
    .blend_state = {
      .enable_blend           = true,
      .color_blend_op         = SDL_GPU_BLENDOP_ADD,
      .alpha_blend_op         = SDL_GPU_BLENDOP_ADD,
      .src_color_blendfactor  = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
      .dst_color_blendfactor  = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
      .src_alpha_blendfactor  = SDL_GPU_BLENDFACTOR_ONE,
      .dst_alpha_blendfactor  = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
    },
  };

  // Fill pipeline: TRIANGLESTRIP for solid rects and textured quads.
  SDL_GPUGraphicsPipelineCreateInfo fill_info = {
    .vertex_shader   = vs,
    .fragment_shader = fs,
    .vertex_input_state = {
      .vertex_buffer_descriptions = &vb_desc,
      .num_vertex_buffers         = 1,
      .vertex_attributes          = vb_attrs,
      .num_vertex_attributes      = 2,
    },
    .primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP,
    .rasterizer_state = {
      .fill_mode  = SDL_GPU_FILLMODE_FILL,
      .cull_mode  = SDL_GPU_CULLMODE_NONE,
      .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
    },
    .target_info = {
      .color_target_descriptions = &color_target,
      .num_color_targets         = 1,
    },
  };

  self->fill_pipeline = SDL_CreateGPUGraphicsPipeline(self->device, &fill_info);

  // Line pipeline: LINESTRIP for outlines and polylines.
  SDL_GPUGraphicsPipelineCreateInfo line_info = fill_info;
  line_info.primitive_type = SDL_GPU_PRIMITIVETYPE_LINESTRIP;

  self->line_pipeline = SDL_CreateGPUGraphicsPipeline(self->device, &line_info);

  // Shaders are consumed by the pipelines; release them now.
  SDL_ReleaseGPUShader(self->device, vs);
  SDL_ReleaseGPUShader(self->device, fs);

  if (!self->fill_pipeline || !self->line_pipeline) {
    MVC_LogError("renderDeviceDidReset: pipeline creation failed: %s\n", SDL_GetError());
    $(self, renderDeviceWillReset);
    return;
  }

  // GPU vertex buffer: large enough for one full frame.
  SDL_GPUBufferCreateInfo vbuf_info = {
    .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
    .size  = MVC_MAX_VERTICES * sizeof(MVC_Vertex),
  };
  self->vertex_buffer = SDL_CreateGPUBuffer(self->device, &vbuf_info);

  // CPU-to-GPU transfer buffer for streaming vertex data.
  SDL_GPUTransferBufferCreateInfo tbuf_info = {
    .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    .size  = MVC_MAX_VERTICES * sizeof(MVC_Vertex),
  };
  self->vertex_transfer = SDL_CreateGPUTransferBuffer(self->device, &tbuf_info);

  // Linear sampler for textures.
  SDL_GPUSamplerCreateInfo sampler_info = {
    .min_filter        = SDL_GPU_FILTER_LINEAR,
    .mag_filter        = SDL_GPU_FILTER_LINEAR,
    .address_mode_u    = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    .address_mode_v    = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    .address_mode_w    = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
  };
  self->sampler = SDL_CreateGPUSampler(self->device, &sampler_info);

  // 1×1 white fallback texture (used for solid-colour primitives).
  const Uint8 white_px[4] = { 255, 255, 255, 255 };
  SDL_Surface *white_surface = SDL_CreateSurfaceFrom(1, 1, SDL_PIXELFORMAT_RGBA32,
                                                     (void *) white_px, 4);
  if (white_surface) {
    self->white = $(self, createTexture, white_surface);
    SDL_DestroySurface(white_surface);
  }

  if (!self->vertex_buffer || !self->vertex_transfer || !self->sampler || !self->white) {
    MVC_LogError("renderDeviceDidReset: resource creation failed: %s\n", SDL_GetError());
    $(self, renderDeviceWillReset);
  }
}

/**
 * @fn void Renderer::renderDeviceWillReset(Renderer *self)
 * @memberof Renderer
 */
static void renderDeviceWillReset(Renderer *self) {

  if (!self->device) {
    return;
  }

  if (self->white) {
    SDL_ReleaseGPUTexture(self->device, self->white);
    self->white = NULL;
  }

  if (self->sampler) {
    SDL_ReleaseGPUSampler(self->device, self->sampler);
    self->sampler = NULL;
  }

  if (self->vertex_transfer) {
    SDL_ReleaseGPUTransferBuffer(self->device, self->vertex_transfer);
    self->vertex_transfer = NULL;
  }

  if (self->vertex_buffer) {
    SDL_ReleaseGPUBuffer(self->device, self->vertex_buffer);
    self->vertex_buffer = NULL;
  }

  if (self->line_pipeline) {
    SDL_ReleaseGPUGraphicsPipeline(self->device, self->line_pipeline);
    self->line_pipeline = NULL;
  }

  if (self->fill_pipeline) {
    SDL_ReleaseGPUGraphicsPipeline(self->device, self->fill_pipeline);
    self->fill_pipeline = NULL;
  }

  if (self->window) {
    SDL_ReleaseWindowFromGPUDevice(self->device, self->window);
  }

  SDL_DestroyGPUDevice(self->device);
  self->device = NULL;

  mvc_gpu_device = NULL;
}

/**
 * @fn void Renderer::setClippingFrame(Renderer *self, const SDL_Rect *clippingFrame)
 * @memberof Renderer
 */
static void setClippingFrame(Renderer *self, const SDL_Rect *clippingFrame) {

  if (clippingFrame) {
    self->current_scissor = MVC_TransformToWindow(self->window, clippingFrame);
    self->has_scissor = true;
  } else {
    self->current_scissor = MakeRect(0, 0, (int) self->swapchain_w, (int) self->swapchain_h);
    self->has_scissor = false;
  }
}

/**
 * @fn void Renderer::setDrawColor(Renderer *self, const SDL_Color *color)
 * @memberof Renderer
 */
static void setDrawColor(Renderer *self, const SDL_Color *color) {
  self->color = *color;
}

/**
 * @fn void Renderer::setWindow(Renderer *self, SDL_Window *window)
 * @memberof Renderer
 */
static void setWindow(Renderer *self, SDL_Window *window) {
  self->window = window;
}

#pragma mark - Object lifecycle

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Renderer *this = (Renderer *) self;

  $(this, renderDeviceWillReset);

  free(this->draw_calls);

  super(Object, self, dealloc);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((RendererInterface *) clazz->interface)->beginFrame            = beginFrame;
  ((RendererInterface *) clazz->interface)->createTexture         = createTexture;
  ((RendererInterface *) clazz->interface)->drawLine              = drawLine;
  ((RendererInterface *) clazz->interface)->drawLines             = drawLines;
  ((RendererInterface *) clazz->interface)->drawRect              = drawRect;
  ((RendererInterface *) clazz->interface)->drawRectFilled        = drawRectFilled;
  ((RendererInterface *) clazz->interface)->drawTexture           = drawTexture;
  ((RendererInterface *) clazz->interface)->drawView              = drawView;
  ((RendererInterface *) clazz->interface)->endFrame              = endFrame;
  ((RendererInterface *) clazz->interface)->init                  = init;
  ((RendererInterface *) clazz->interface)->renderDeviceDidReset  = renderDeviceDidReset;
  ((RendererInterface *) clazz->interface)->renderDeviceWillReset = renderDeviceWillReset;
  ((RendererInterface *) clazz->interface)->setClippingFrame      = setClippingFrame;
  ((RendererInterface *) clazz->interface)->setDrawColor          = setDrawColor;
  ((RendererInterface *) clazz->interface)->setWindow             = setWindow;
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
      .name           = "Renderer",
      .superclass     = _Object(),
      .instanceSize   = sizeof(Renderer),
      .interfaceOffset = offsetof(Renderer, interface),
      .interfaceSize  = sizeof(RendererInterface),
      .initialize     = initialize,
    });
  });

  return clazz;
}

#undef _Class
