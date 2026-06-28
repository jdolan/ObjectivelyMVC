/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and GNU C.
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
 * @file Hello-Scene.vert.glsl
 * @brief Hello-Scene example vertex shader.
 *
 * Inputs  (location 0): vec3 position
 *         (location 1): vec3 color
 * Uniform (set=1, b=0): mat4 ModelViewProj
 * Output              : vec4 gl_Position, vec4 color
 */

#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec4 out_color;

layout(set = 1, binding = 0, std140) uniform UBO {
	mat4 ModelViewProj;
};

void main() {
	out_color   = vec4(in_color, 1.0);
	gl_Position = ModelViewProj * vec4(in_position, 1.0);
}
