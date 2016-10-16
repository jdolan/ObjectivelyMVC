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

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/Shader.h>

/**
 * @file
 * @brief An abstraction for OpenGL programs (GLSL).
 */

typedef struct Attribute Attribute;

/**
 * @brief Vertex Attributes.
 */
struct Attribute {

	/**
	 * @brief The Attribute location.
	 */
	GLint location;

	/**
	 * @brief The Attribute name.
	 */
	const GLchar *name;
};

typedef struct Uniform Uniform;

/**
 * @brief Uniform Program variables.
 */
struct Uniform {

	/**
	 * @brief The Uniform location.
	 */
	GLint location;

	/**
	 * @brief The Uniform name.
	 */
	const GLchar *name;
};

typedef struct Program Program;
typedef struct ProgramInterface ProgramInterface;

/**
 * @brief An abstraction for OpenGL programs (GLSL).
 * @extends Object
 */
struct Program {
	
	/**
	 * @brief The parent.
	 */
	Object object;
	
	/**
	 * @brief The typed interface.
	 * @protected
	 */
	ProgramInterface *interface;

	/**
	 * @brief The program Attributes.
	 */
	MutableArray *attributes;

	/**
	 * @brief The program name.
	 */
	GLuint name;

	/**
	 * @brief The Shaders comprising this Program.
	 */
	MutableArray *shaders;

	/**
	 * @brief The program Uniforms.
	 */
	MutableArray *uniforms;
};

/**
 * @brief The Program interface.
 */
struct ProgramInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Program::attachShader(Program *self, Shader *shader)
	 * @brief Attaches the specified Shader to this Program.
	 * @param self The Program.
	 * @param shader The Shader.
	 * @memberof Program
	 */
	void (*attachShader)(Program *self, Shader *shader);

	/**
	 * @static
	 * @fn Program *Program::defaultProgram(void)
	 * @return The default Program.
	 * @memberof Program
	 */
	Program *(*defaultProgram)(void);

	/**
	 * @fn Attribute *Program::getAttribute(Program *sef, const GLchar *name)
	 * @param self The Program.
	 * @param name The Attribute name.
	 * @return The Attribute by the given name.
	 * @memberof Program
	 */
	Attribute *(*getAttribute)(Program *self, const GLchar *name);

	/**
	 * @fn Uniform *Program::getUniform(Program *sef, const GLchar *name)
	 * @param self The Program.
	 * @param name The Uniform name.
	 * @return The Uniform by the given name.
	 * @memberof Program
	 */
	Uniform *(*getUniform)(Program *self, const GLchar *name);
	
	/**
	 * @fn Program *Program::init(Program *self)
	 * @brief Initializes this Program.
	 * @param The Program.
	 * @return The initialized Program, or `NULL` on error.
	 * @memberof Program
	 */
	Program *(*init)(Program *self);

	/**
	 * @fn void Program::link(Program *self)
	 * @brief Links this Program, resolving the specified required Attributes and Uniforms.
	 * @param self The Program.
	 * @param attributes A NULL-terminated list of required Attribute names.
	 * @param uniforms A NULL-terminated list of required Uniform names.
	 * @remarks This method raises an assertion error if any of the named variables fail to resolve.
	 * @memberof Program
	 */
	void (*link)(Program *self, const GLchar **attributes, const GLchar **uniforms);

	/**
	 * @fn void Program::use(Program *self)
	 * @brief Uses this Program.
	 * @memberof Program
	 */
	void (*use)(Program *self);
};

/**
 * @brief The Program Class.
 */
extern Class _Program;

