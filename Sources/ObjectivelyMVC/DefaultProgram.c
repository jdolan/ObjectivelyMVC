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

#include <ObjectivelyMVC/DefaultProgram.h>

#define _Class _DefaultProgram

#pragma mark - Object

static void dealloc(Object *self) {

	DefaultProgram *this = (DefaultProgram *) self;

	free(this->color);
	free(this->sampler);
	free(this->texcoord);
	free(this->vertex);

	super(Object, self, dealloc);
}

#pragma mark - DefaultProgram

/**
 * @fn DefaultProgram *DefaultProgram::init(DefaultProgram *self)
 * @memberof DefaultProgram
 */
static DefaultProgram *init(DefaultProgram *self) {
	
	self = (DefaultProgram *) super(Program, self, init);
	if (self) {
		
		Shader *vs = $(alloc(Shader), initWithSource, GL_VERTEX_SHADER, "\
			#version 120 \n\
			attribute vec2 vertex; \
			attribute vec2 texcoord; \
			varying vec2 position; \
			varying vec2 texel; \
			void main() { \
				position = vertex; \
				texel = texcoord; \
			}");

		Shader *fs = $(alloc(Shader), initWithSource, GL_FRAGMENT_SHADER, "\
			#version 120 \n\
			uniform vec4 color; \
			uniform sampler2D sampler; \
			varying vec2 position; \
			varying vec2 texel; \
			void main() { \
				gl_FragColor = color * texture2D(sampler, texel); \
			}");

		Program *this = (Program *) self;

		$(this, attachShader, vs);
		$(this, attachShader, fs);

		release(vs);
		release(fs);

		$(this, link);
		assert(this->name);

		self->color = $(this, getUniform, "color");
		self->sampler = $(this, getUniform, "sampler");
		self->texcoord = $(this, getAttribute, "texcoord");
		self->vertex = $(this, getAttribute, "vertex");
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	
	((DefaultProgramInterface *) clazz->def->interface)->init = init;
	
	//..
}

Class _DefaultProgram = {
	.name = "DefaultProgram",
	.superclass = &_Program,
	.instanceSize = sizeof(DefaultProgram),
	.interfaceOffset = offsetof(DefaultProgram, interface),
	.interfaceSize = sizeof(DefaultProgramInterface),
	.initialize = initialize,
};

#undef _Class

