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
 * Sampler (set=2, b=0): sampler2D tex (combined texture + sampler)
 * Output              : vec4 RGBA
 */

#version 450

layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec4 in_color;

layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform sampler2D tex;

void main() {
  out_color = in_color * texture(tex, in_texcoord);
}
