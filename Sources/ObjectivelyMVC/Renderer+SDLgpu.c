/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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

#include "Renderer+SDLgpu.h"

SDL_GPUDevice *mvc_gpu_device;

SDL_Window *mvc_current_window;

void MVC_ReleaseGPUTexture(SDL_GPUTexture *texture) {

  if (texture && mvc_gpu_device) {
    SDL_ReleaseGPUTexture(mvc_gpu_device, texture);
  }
}

// MSL vertex shader: transforms 2D position + texcoord using an orthographic
// projection pushed as vertex uniform buffer 0.
const char *MVC_VertexShaderMSL =
  "#include <metal_stdlib>\n"
  "using namespace metal;\n"
  "\n"
  "struct VertexIn {\n"
  "  float2 position [[attribute(0)]];\n"
  "  float2 texcoord [[attribute(1)]];\n"
  "};\n"
  "\n"
  "struct VertexOut {\n"
  "  float4 position [[position]];\n"
  "  float2 texcoord;\n"
  "};\n"
  "\n"
  "struct Projection {\n"
  "  float4x4 matrix;\n"
  "};\n"
  "\n"
  "vertex VertexOut vs_main(\n"
  "  VertexIn in [[stage_in]],\n"
  "  constant Projection &proj [[buffer(0)]])\n"
  "{\n"
  "  VertexOut out;\n"
  "  out.position = proj.matrix * float4(in.position, 0.0, 1.0);\n"
  "  out.texcoord = in.texcoord;\n"
  "  return out;\n"
  "}\n";

// MSL fragment shader: samples a texture and multiplies by a tint color pushed
// as fragment uniform buffer 0.
const char *MVC_FragmentShaderMSL =
  "#include <metal_stdlib>\n"
  "using namespace metal;\n"
  "\n"
  "struct FragIn {\n"
  "  float2 texcoord;\n"
  "};\n"
  "\n"
  "struct Color {\n"
  "  float4 rgba;\n"
  "};\n"
  "\n"
  "fragment float4 fs_main(\n"
  "  FragIn in [[stage_in]],\n"
  "  texture2d<float> tex [[texture(0)]],\n"
  "  sampler s [[sampler(0)]],\n"
  "  constant Color &color [[buffer(0)]])\n"
  "{\n"
  "  return color.rgba * tex.sample(s, in.texcoord);\n"
  "}\n";
