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

/**
 * @file Renderer.vert.glsl
 * @brief ObjectivelyMVC Renderer vertex shader.
 *
 * Inputs  (location 0): vec2 position
 *         (location 1): vec2 texcoord
 * Uniform (set=1, b=0): mat4 proj (orthographic projection)
 * Output              : vec4 gl_Position, vec2 texcoord
 */

#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texcoord;

layout(location = 0) out vec2 out_texcoord;

layout(set = 1, binding = 0, std140) uniform Projection {
	mat4 proj;
};

void main() {
	gl_Position  = proj * vec4(in_position, 0.0, 1.0);
	out_texcoord = in_texcoord;
}
