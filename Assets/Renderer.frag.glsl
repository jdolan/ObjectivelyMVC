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

/**
 * @file Renderer.frag.glsl
 * @brief ObjectivelyMVC Renderer fragment shader.
 *
 * Inputs  (location 0): vec2 texcoord from vertex stage
 *         (location 1): vec4 color from vertex stage
 *         (location 2): vec4 rect (center.xy, halfExtent.xy; halfExtent.x 0 = plain quad)
 *         (location 3): vec2 shape (corner radius, stroke width; stroke 0 = filled)
 *         (location 4): vec2 position in logical coordinates
 * Sampler (set=2, b=0): sampler2D tex (combined texture + sampler)
 * Output              : vec4 RGBA
 */

#version 450

layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec4 in_color;
layout(location = 2) flat in vec4 in_rect;
layout(location = 3) flat in vec2 in_shape;
layout(location = 4) in vec2 in_position;

layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform sampler2D tex;

/**
 * @brief Signed distance from `p` to a rounded rectangle centered at the origin with
 * half-extent `b` and corner radius `r`. Negative inside, positive outside.
 */
float roundedRect(vec2 p, vec2 b, float r) {
  vec2 q = abs(p) - b + r;
  return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

void main() {
  out_color = in_color * texture(tex, in_texcoord);

  if (in_rect.z > 0.0) {
    float r = min(in_shape.x, min(in_rect.z, in_rect.w));
    float d = roundedRect(in_position - in_rect.xy, in_rect.zw, r);
    if (in_shape.y > 0.0) {
      d = max(d, -(d + in_shape.y));
    }
    // fwidth scales the one pixel anti-aliasing band with the window's pixel density
    float aa = max(fwidth(d), 1e-4);
    out_color.a *= clamp(0.5 - d / aa, 0.0, 1.0);
  }
}
