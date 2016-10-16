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
#include <ObjectivelyMVC/Shader.h>

#define _Class _Shader

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Shader *this = (Shader *) self;

	free(this->info);

	glDeleteShader(this->name);
	
	super(Object, self, dealloc);
}

#pragma mark - Shader

/**
 * @fn Shader *Shader::initWithSource(Shader *self, GLenum type, const GLchar *source)
 * @memberof Shader
 */
static Shader *initWithSource(Shader *self, GLenum type, const GLchar *source) {
	
	self = (Shader *) super(Object, self, init);
	if (self) {
		assert(source);

		self->name = glCreateShader(type);
		assert(self->name);

		GLint i = strlen(source);

		glShaderSource(self->name, 1, (const GLchar **) &source, &i);
		glCompileShader(self->name);

		glGetShaderiv(self->name, GL_COMPILE_STATUS, &i);
		if (i != GL_TRUE) {

			glGetShaderiv(self->name, GL_INFO_LOG_LENGTH, &i);

			self->info = malloc(i);
			assert(self->info);

			glGetShaderInfoLog(self->name, i, NULL, self->info);
			MVC_LogError("Failed to compile shader: %s\n", self->info);

			glDeleteShader(self->name);
			self->name = 0;
		}
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	
	((ShaderInterface *) clazz->def->interface)->initWithSource = initWithSource;
}

Class _Shader = {
	.name = "Shader",
	.superclass = &_Object,
	.instanceSize = sizeof(Shader),
	.interfaceOffset = offsetof(Shader, interface),
	.interfaceSize = sizeof(ShaderInterface),
	.initialize = initialize,
};

#undef _Class

