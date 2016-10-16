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

#include <ObjectivelyMVC/Program.h>

/**
 * @file
 * @brief The default GLSL Program.
 */

typedef struct DefaultProgram DefaultProgram;
typedef struct DefaultProgramInterface DefaultProgramInterface;

/**
 * @brief The default GLSL Program.
 * @extends Program
 */
struct DefaultProgram {
	
	/**
	 * @brief The parent.
	 */
	Program program;
	
	/**
	 * @brief The typed interface.
	 * @protected
	 */
	DefaultProgramInterface *interface;

	/**
	 * @brief The color Uniform.
	 */
	Uniform *color;

	/**
	 * @brief The sampler Uniform.
	 */
	Uniform *sampler;

	/**
	 * @brief The texture coordinate Attribute.
	 */
	Attribute *texcoord;

	/**
	 * @brief The vertex Attribute.
	 */
	Attribute *vertex;
};

/**
 * @brief The DefaultProgram interface.
 */
struct DefaultProgramInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ProgramInterface programInterface;
	
	/**
	 * @fn DefaultProgram *DefaultProgram::init(DefaultProgram *self)
	 * @brief Initializes this DefaultProgram.
	 * @param The DefaultProgram.
	 * @return The initialized DefaultProgram, or `NULL` on error.
	 * @memberof DefaultProgram
	 */
	DefaultProgram *(*init)(DefaultProgram *self);
};

/**
 * @brief The DefaultProgram Class.
 */
extern Class _DefaultProgram;

