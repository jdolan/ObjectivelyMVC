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

#include <ObjectivelyMVC/Image.h>
#include <ObjectivelyMVC/RendererGL20.h>

#pragma mark - OpenGL entry points

static PFNGLBINDBUFFERPROC glBindBuffer;
static PFNGLBUFFERDATAPROC glBufferData;
static PFNGLBUFFERSUBDATAPROC glBufferSubData;
static PFNGLDELETEBUFFERSPROC glDeleteBuffers;
static PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
static PFNGLGENBUFFERSPROC glGenBuffers;
static PFNGLUNIFORM4FVPROC glUniform4fv;
static PFNGLUSEPROGRAMPROC glUseProgram;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

#pragma mark - Image data

static Image *_nullTexture;
static const unsigned char _nullTextureData[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
	0x08, 0x02, 0x00, 0x00, 0x00, 0x90, 0x77, 0x53, 0xde, 0x00, 0x00, 0x00,
	0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0b, 0x13, 0x00, 0x00, 0x0b,
	0x13, 0x01, 0x00, 0x9a, 0x9c, 0x18, 0x00, 0x00, 0x00, 0x07, 0x74, 0x49,
	0x4d, 0x45, 0x07, 0xe0, 0x0a, 0x10, 0x14, 0x18, 0x17, 0xda, 0xde, 0xe8,
	0x2e, 0x00, 0x00, 0x00, 0x1d, 0x69, 0x54, 0x58, 0x74, 0x43, 0x6f, 0x6d,
	0x6d, 0x65, 0x6e, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x72, 0x65,
	0x61, 0x74, 0x65, 0x64, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x47, 0x49,
	0x4d, 0x50, 0x64, 0x2e, 0x65, 0x07, 0x00, 0x00, 0x00, 0x0c, 0x49, 0x44,
	0x41, 0x54, 0x08, 0xd7, 0x63, 0xf8, 0xff, 0xff, 0x3f, 0x00, 0x05, 0xfe,
	0x02, 0xfe, 0xdc, 0xcc, 0x59, 0xe7, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
	0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

#define _Class _RendererGL20

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	RendererGL20 *this = (RendererGL20 *) self;

	glDeleteTextures(1, &this->nullTexture);

	glDeleteBuffers(1, &this->texcoordBuffer);
	glDeleteBuffers(1, &this->vertexBuffer);

	release(this->program);

	super(Object, self, dealloc);
}

#pragma mark - Renderer

/**
 * @see Renderer::beginFrame(Renderer *self)
 */
static void beginFrame(Renderer *self) {

	super(Renderer, self, beginFrame);

	RendererGL20 *this = (RendererGL20 *) self;

	glUseProgram(this->program->name);

	$(self, setDrawColor, &Colors.White);

	glBindTexture(GL_TEXTURE_2D, this->nullTexture);

	glEnableVertexAttribArray(this->vertexAttribute);
	glEnableVertexAttribArray(this->texcoordAttribute);
}

/**
 * @see Renderer::drawLines(const Renderer *self, const SDL_Point *points, size_t count)
 */
static void drawLines(const Renderer *self, const SDL_Point *points, size_t count) {
	
	assert(points);

	const RendererGL20 *this = (RendererGL20 *) self;

	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(SDL_Point), points);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_LINE_STRIP, 0, count);
}

/**
 * @see Renderer::drawRect(const Renderer *self, const SDL_Rect *rect)
 */
static void drawRect(const Renderer *self, const SDL_Rect *rect) {

	assert(rect);

	const RendererGL20 *this = (RendererGL20 *) self;

	GLint verts[8];

	verts[0] = rect->x;
	verts[1] = rect->y;

	verts[2] = rect->x + rect->w;
	verts[3] = rect->y;

	verts[4] = rect->x + rect->w;
	verts[5] = rect->y + rect->h;

	verts[6] = rect->x;
	verts[7] = rect->y + rect->h;

	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

/**
 * @see Renderer::drawRectFilled(const Renderer *self, const SDL_Rect *rect)
 */
static void drawRectFilled(const Renderer *self, const SDL_Rect *rect) {

	assert(rect);

	const RendererGL20 *this = (RendererGL20 *) self;

	GLint verts[8];

	verts[0] = rect->x - 1;
	verts[1] = rect->y;

	verts[2] = rect->x + rect->w;
	verts[3] = rect->y;

	verts[4] = rect->x + rect->w;
	verts[5] = rect->y + rect->h + 1;

	verts[6] = rect->x - 1;
	verts[7] = rect->y + rect->h + 1;

	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

/**
 * @see Renderer::drawTexture(const Renderer *self, ident texture, const SDL_Rect *dest)
 */
static void drawTexture(const Renderer *self, ident texture, const SDL_Rect *rect) {

	assert(rect);

	const RendererGL20 *this = (RendererGL20 *) self;

	const GLfloat texcoords[] = {
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0
	};

	GLint verts[8];

	verts[0] = rect->x;
	verts[1] = rect->y;

	verts[2] = rect->x + rect->w;
	verts[3] = rect->y;

	verts[4] = rect->x + rect->w;
	verts[5] = rect->y + rect->h;

	verts[6] = rect->x;
	verts[7] = rect->y + rect->h;

	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->texcoordBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texcoords), texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, (GLuint) texture);

	glDrawArrays(GL_QUADS, 0, 4);

	glBindTexture(GL_TEXTURE_2D, this->nullTexture);
}

/**
 * @see Renderer::endFrame(Renderer *self)
 */
static void endFrame(Renderer *self) {

	RendererGL20 *this = (RendererGL20 *) self;

	glDisableVertexAttribArray(this->texcoordAttribute);
	glDisableVertexAttribArray(this->vertexAttribute);

	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

	super(Renderer, self, endFrame);
}

/**
 * @see Renderer::renderDeviceDidReset(Renderer *self)
 */
static void renderDeviceDidReset(Renderer *self) {

	super(Renderer, self, renderDeviceDidReset);

	RendererGL20 *this = (RendererGL20 *) self;

	glGenBuffers(1, &this->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 0x10000, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &this->texcoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->texcoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, 0x10000, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	this->nullTexture = (GLuint)$(self, createTexture, _nullTexture->surface);

	this->program = (Program *) $(alloc(Program), init);
	assert(this->program);

	$(this->program, attachShaderSource, GL_VERTEX_SHADER, "\
		#version 120 \n\
		attribute vec2 vertex; \
		attribute vec2 texcoord; \
		varying vec2 st; \
		void main() { \
			gl_Position = gl_ProjectionMatrix * vec4(vertex, 0.0, 1.0); \
			st = texcoord; \
		}");

	$(this->program, attachShaderSource, GL_FRAGMENT_SHADER, "\
		#version 120 \n\
		uniform vec4 color; \
		uniform sampler2D sampler; \
		varying vec2 st; \
		void main() { \
			gl_FragColor = color * texture2D(sampler, st); \
		}");

	$(this->program, link);
	assert(this->program->name);

	this->vertexAttribute = $(this->program, getAttributeLocation, "vertex");
	this->texcoordAttribute = $(this->program, getAttributeLocation, "texcoord");
	this->colorUniform = $(this->program, getUniformLocation, "color");

	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glVertexAttribPointer(this->vertexAttribute, 2, GL_INT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->texcoordBuffer);
	glVertexAttribPointer(this->texcoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	assert(glGetError() == GL_NO_ERROR);
}

/**
 * @see Renderer::setDrawColor(Renderer *self, const SDL_Color *color)
 */
static void setDrawColor(Renderer *self, const SDL_Color *color) {

	RendererGL20 *this = (RendererGL20 *) self;

	glUniform4fv(this->colorUniform, 1, (const GLfloat []) {
		color->r / 255.0,
		color->g / 255.0,
		color->b / 255.0,
		color->a / 255.0
	});
}

#pragma mark - RendererGL20

/**
 * @fn RendererGL20 *RendererGL20::init(RendererGL20 *self)
 * @memberof RendererGL20
 */
static RendererGL20 *init(RendererGL20 *self) {
	
	self = (RendererGL20 *) super(RendererGL, self, init);
	if (self) {
		$((Renderer *) self, renderDeviceDidReset);
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((RendererInterface *) clazz->def->interface)->beginFrame = beginFrame;
	((RendererInterface *) clazz->def->interface)->drawLines = drawLines;
	((RendererInterface *) clazz->def->interface)->drawRect = drawRect;
	((RendererInterface *) clazz->def->interface)->drawRectFilled = drawRectFilled;
	((RendererInterface *) clazz->def->interface)->drawTexture = drawTexture;
	((RendererInterface *) clazz->def->interface)->endFrame = endFrame;
	((RendererInterface *) clazz->def->interface)->renderDeviceDidReset = renderDeviceDidReset;
	((RendererInterface *) clazz->def->interface)->setDrawColor = setDrawColor;

	((RendererGL20Interface *) clazz->def->interface)->init = init;

	_nullTexture = $(alloc(Image), initWithBytes, _nullTextureData, lengthof(_nullTextureData));

	MVC_GL_GetProcAddress(glBindBuffer);
	MVC_GL_GetProcAddress(glBufferData);
	MVC_GL_GetProcAddress(glBufferSubData);
	MVC_GL_GetProcAddress(glDeleteBuffers);
	MVC_GL_GetProcAddress(glDisableVertexAttribArray);
	MVC_GL_GetProcAddress(glEnableVertexAttribArray);
	MVC_GL_GetProcAddress(glGenBuffers);
	MVC_GL_GetProcAddress(glUniform4fv);
	MVC_GL_GetProcAddress(glUseProgram);
	MVC_GL_GetProcAddress(glVertexAttribPointer);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_nullTexture);
}

Class _RendererGL20 = {
	.name = "RendererGL20",
	.superclass = &_RendererGL,
	.instanceSize = sizeof(RendererGL20),
	.interfaceOffset = offsetof(RendererGL20, interface),
	.interfaceSize = sizeof(RendererGL20Interface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class

