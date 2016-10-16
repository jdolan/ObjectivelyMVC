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

	glDeleteShader(this->name);
	
	super(Object, self, dealloc);
}

#pragma mark - Shader

/**
 * @fn Shader *Shader::initWithSource(Shader *self, GLenum type, const GLchar *src)
 * @memberof Shader
 */
static Shader *initWithSource(Shader *self, GLenum type, const GLchar *src) {
	
	self = (Shader *) super(Object, self, init);
	if (self) {

		assert(src);

		self->name = glCreateShader(type);
		assert(self->name);

		GLint length = strlen(src);

		glShaderSource(self->name, 1, (const GLchar **) &src, &length);
		glCompileShader(self->name);

		GLint compiled = GL_FALSE;
		glGetShaderiv(self->name, GL_COMPILE_STATUS, &compiled);
		if (compiled != GL_TRUE) {

			glGetShaderiv(self->name, GL_INFO_LOG_LENGTH, &length);
			GLchar *log = calloc(length, 1);

			glGetShaderInfoLog(self->name, length - 1, NULL, log);
			MVC_LogError("Failed to compile shader: %s\n", log);

			free(log);
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

