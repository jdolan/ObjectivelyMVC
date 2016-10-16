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

#include <Objectively/Once.h>
#include <Objectively/Value.h>

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

	$(self->shaders, addObject, shader);

	glAttachShader(self->name, shader->name);
	assert(glGetError() == GL_NO_ERROR);
}

static Program *_defaultProgram;

/**
 * @fn Program *Program::defaultProgram(void)
 * @memberof Program
 */
static Program *defaultProgram(void) {
	static Once once;

	do_once(&once, ({
		Shader *vs = $(alloc(Shader), initWithSource, GL_VERTEX_SHADER, "\
			#version 120 \n\
			attribute vec2 vertex; \
			void main() { \
				gl_Position = vec4(vertex.x, vertex.y, 0.0, 1.0); \
			}");

		Shader *fs = $(alloc(Shader), initWithSource, GL_FRAGMENT_SHADER, "\
			#version 120 \n\
			uniform vec4 color; \
			void main() { \
				gl_FragColor = color; \
			}");

		_defaultProgram = $(alloc(Program), init);

		$(_defaultProgram, attachShader, vs);
		$(_defaultProgram, attachShader, fs);

		const GLchar *attributes[] = { "vertex", NULL };
		const GLchar *uniforms[] = { "color", NULL };

		$(_defaultProgram, link, attributes, uniforms);

		release(vs);
		release(fs);
	}));

	return _defaultProgram;
}

/**
 * @brief Predicate for getAttribute.
 */
static _Bool getAttribute_predicate(const ident obj, ident data) {
	const Attribute *attribute = ((Value *) obj)->value;
	return strcmp((const char *) attribute->name, (const char *) data) == 0;
}

/**
 * @fn GLint Program::getAttribute(Program *sef, const GLchar *name)
 * @memberof Program
 */
static Attribute *getAttribute(Program *self, const GLchar *name) {

	assert(name);

	Value *value = $((Array *) self->attributes, findObject, getAttribute_predicate, (ident) name);
	if (value == NULL) {

		Attribute *attribute = calloc(1, sizeof(*attribute));
		assert(attribute);

		attribute->name = name;
		attribute->location = glGetAttribLocation(self->name, attribute->name);

		if (attribute->location == -1) {
			MVC_LogWarn("Failed to resolve Attribute %s\n", attribute->name);
		}

		value = $(alloc(Value), initWithValue, attribute);
		value->destroy = free;

		$(self->attributes, addObject, value);
	}

	return (Attribute *) value->value;
}

/**
 * @brief Predicate for getUniform.
 */
static _Bool getUniform_predicate(const ident obj, ident data) {
	const Attribute *attribute = ((Value *) obj)->value;
	return strcmp((const char *) attribute->name, (const char *) data) == 0;
}

/**
 * @fn Uniform *Program::getUniform(Program *sef, const GLchar *name)
 * @memberof Program
 */
static Uniform *getUniform(Program *self, const GLchar *name) {

	assert(name);

	Value *value = $((Array *) self->uniforms, findObject, getUniform_predicate, (ident) name);
	if (value == NULL) {

		Uniform *uniform = calloc(1, sizeof(*uniform));
		assert(uniform);

		uniform->name = name;
		uniform->location = glGetUniformLocation(self->name, uniform->name);

		if (uniform->location == -1) {
			MVC_LogWarn("Failed to resolve Uniform %s\n", uniform->name);
		}

		value = $(alloc(Value), initWithValue, uniform);
		value->destroy = free;

		$(self->uniforms, addObject, value);
	}

	return (Uniform *) value->value;
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

		self->attributes = $$(MutableArray, array);
		assert(self->attributes);

		self->shaders = $$(MutableArray, array);
		assert(self->shaders);

		self->uniforms = $$(MutableArray, array);
		assert(self->uniforms);
	}
	
	return self;
}

/**
 * @fn void Program::link(Program *self, const char **attributes, const char **uniforms)
 * @memberof Program
 */
static void link(Program *self, const char **attributes, const char **uniforms) {

	glLinkProgram(self->name);

	const char **attr = attributes;
	while (*attr) {
		Attribute *attribute = $(self, getAttribute, *attr++);
		assert(attribute->location != -1);
	}

	const char **unif = uniforms;
	while (*unif) {
		Uniform *uniform = $(self, getUniform, *unif++);
		assert(uniform->location != -1);
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
	((ProgramInterface *) clazz->def->interface)->defaultProgram = defaultProgram;
	((ProgramInterface *) clazz->def->interface)->getAttribute = getAttribute;
	((ProgramInterface *) clazz->def->interface)->getUniform = getUniform;
	((ProgramInterface *) clazz->def->interface)->init = init;
	((ProgramInterface *) clazz->def->interface)->link = link;
	((ProgramInterface *) clazz->def->interface)->use = use;
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_defaultProgram);
}

Class _Program = {
	.name = "Program",
	.superclass = &_Object,
	.instanceSize = sizeof(Program),
	.interfaceOffset = offsetof(Program, interface),
	.interfaceSize = sizeof(ProgramInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class

