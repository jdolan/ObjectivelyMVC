/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
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


#include <Objectively/Object.h>

#include <ObjectivelyMVC/OpenGL.h>
#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief An abstraction for OpenGL shaders (GLSL).
 */

typedef struct Shader Shader;
typedef struct ShaderInterface ShaderInterface;

/**
 * @brief An abstraction for OpenGL shaders (GLSL).
 * @extends Object
 */
struct Shader {
	
	/**
	 * @brief The parent.
	 */
	Object object;
	
	/**
	 * @brief The typed interface.
	 * @protected
	 */
	ShaderInterface *interface;

	/**
	 * @brief The shader information log.
	 */
	GLchar *info;

	/**
	 * @brief The shader name.
	 */
	GLuint name;

	/**
	 * @brief The shader type.
	 */
	GLenum type;
};

/**
 * @brief The Shader interface.
 */
struct ShaderInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn Shader *Shader::initWithSource(Shader *self, GLenum type, const GLchar *src)
	 * @brief Initializes this Shader with the specified type and source.
	 * @param The Shader.
	 * @param type The shader type (`GL_VERTEX_SHADER` or `GL_FRAGMENT_SHADER`).
	 * @param source A NULL-terminated GLSL source string.
	 * @return The initialized Shader, or `NULL` on error.
	 * @memberof Shader
	 */
	Shader *(*initWithSource)(Shader *self, GLenum type, const GLchar *source);
};

/**
 * @brief The Shader Class.
 */
extern Class _Shader;
