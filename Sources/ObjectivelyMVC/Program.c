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

#include <assert.h>

#include <ObjectivelyMVC/Log.h>
#include <ObjectivelyMVC/OpenGL.h>
#include <ObjectivelyMVC/Program.h>

#define _Class _Program

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	Program *this = (Program *) self;

	free(this->info);

	glDeleteProgram(this->name);

	release(this->shaders);

	super(Object, self, dealloc);
}

#pragma mark - Program

/**
 * @fn void Program::attachShader(Program *self, Shader *shader)
 * @memberof Program
 */
static void attachShader(Program *self, Shader *shader) {

	assert(shader);
	assert(shader->name);

	glAttachShader(self->name, shader->name);
	assert(glGetError() == GL_NO_ERROR);

	$(self->shaders, addObject, shader);
}

/**
 * @fn GLint Program::getAttributeLocation(Program *sef, const GLchar *name)
 * @memberof Program
 */
static GLint getAttributeLocation(Program *self, const GLchar *name) {

	GLint attribute = glGetAttribLocation(self->name, name);
	assert(attribute != -1);

	return attribute;
}

/**
 * @fn GLint Program::getUniformLocation(Program *sef, const GLchar *name)
 * @memberof Program
 */
static GLint getUniformLocation(Program *self, const GLchar *name) {

	GLint uniform = glGetUniformLocation(self->name, name);
	assert(uniform != -1);

	return uniform;
}

/**
 * @fn Program *Program::init(Program *self)
 * @memberof Program
 */
static Program *init(Program *self) {
	
	self = (Program *) super(Object, self, init);
	if (self) {

		self->name = glCreateProgram();
		assert(self->name);

		self->shaders = $$(MutableArray, array);
		assert(self->shaders);
	}
	
	return self;
}

/**
 * @fn void Program::link(Program *self)
 * @memberof Program
 */
static void link(Program *self) {

	glLinkProgram(self->name);

	GLint i;
	glGetProgramiv(self->name, GL_LINK_STATUS, &i);
	if (i != GL_TRUE) {

		glGetProgramiv(self->name, GL_INFO_LOG_LENGTH, &i);

		free(self->info);

		self->info = malloc(i);
		assert(self->info);

		glGetProgramInfoLog(self->name, i, NULL, self->info);
		MVC_LogError("Failed to link program: %s\n", self->info);

		glDeleteProgram(self->name);
		self->name = 0;
	}
}

/**
 * @fn void Program::use(Program *self)
 * @memberof Program
 */
static void use(Program *self) {
	glUseProgram(self->name);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ProgramInterface *) clazz->def->interface)->attachShader = attachShader;
	((ProgramInterface *) clazz->def->interface)->getAttributeLocation = getAttributeLocation;
	((ProgramInterface *) clazz->def->interface)->getUniformLocation = getUniformLocation;
	((ProgramInterface *) clazz->def->interface)->init = init;
	((ProgramInterface *) clazz->def->interface)->link = link;
	((ProgramInterface *) clazz->def->interface)->use = use;
}

Class _Program = {
	.name = "Program",
	.superclass = &_Object,
	.instanceSize = sizeof(Program),
	.interfaceOffset = offsetof(Program, interface),
	.interfaceSize = sizeof(ProgramInterface),
	.initialize = initialize,
};

#undef _Class

