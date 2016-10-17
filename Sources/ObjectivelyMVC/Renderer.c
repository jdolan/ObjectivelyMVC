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
#include <ObjectivelyMVC/Log.h>
#include <ObjectivelyMVC/OpenGL.h>
#include <ObjectivelyMVC/Renderer.h>
#include <ObjectivelyMVC/View.h>

#define _Class _Renderer

static Image *_null;
static const unsigned char _nullData[] = {
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

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Renderer *this = (Renderer *) self;

	glDeleteBuffers(1, &this->buffers.texcoord);
	glDeleteBuffers(1, &this->buffers.vertex);

	glDeleteTextures(1, &this->textures.null);

	release(this->program);
	release(this->views);

	super(Object, self, dealloc);
}

#pragma mark - Renderer

/**
 * @fn void Renderer::addView(Renderer *self, View *view)
 * @memberof Renderer
 */
static void addView(Renderer *self, View *view) {
	$(self->views, addObject, view);
}

/**
 * @fn void Renderer::beginFrame(Renderer *self)
 * @memberof Renderer
 */
static void beginFrame(Renderer *self) {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_SCISSOR_TEST);

	glBindTexture(GL_TEXTURE_2D, self->textures.null);

	glUseProgram(self->program->name);

	glEnableVertexAttribArray(self->attributes.texcoord);
	glEnableVertexAttribArray(self->attributes.vertex);

	$(self, setDrawColor, &Colors.White);
}

/**
 * @fn void Renderer::createTexture(const Renderer *self, const SDL_Surface *surface)
 * @memberof Renderer
 */
static GLuint createTexture(const Renderer *self, const SDL_Surface *surface) {

	assert(surface);

	GLenum format;
	switch (surface->format->BytesPerPixel) {
		case 1:
			format = GL_LUMINANCE;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			MVC_LogError("Invalid surface format: %s\n",
					 SDL_GetPixelFormatName(surface->format->format));
			return -1;
	}

	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

	glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format,
				 GL_UNSIGNED_BYTE, surface->pixels);

	return texture;
}

/**
 * @fn void Renderer::drawLine(const Renderer *self, const SDL_Point *points)
 * @memberof Renderer
 */
static void drawLine(const Renderer *self, const SDL_Point *points) {

	assert(points);

	$(self, drawLines, points, 2);
}

/**
 * @fn void Renderer::drawLines(const Renderer *self, const SDL_Point *points, GLuint count)
 * @memberof Renderer
 */
static void drawLines(const Renderer *self, const SDL_Point *points, GLuint count) {

	assert(points);

	glBindBuffer(GL_ARRAY_BUFFER, self->buffers.vertex);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(SDL_Point), points);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_LINE_STRIP, 0, count);
}

/**
 * @fn void Renderer::drawRect(const Renderer *self, const SDL_Rect *rect)
 * @memberof Renderer
 */
static void drawRect(const Renderer *self, const SDL_Rect *rect) {

	assert(rect);

	GLint verts[8];

	verts[0] = rect->x;
	verts[1] = rect->y;

	verts[2] = rect->x + rect->w;
	verts[3] = rect->y;

	verts[4] = rect->x + rect->w;
	verts[5] = rect->y + rect->h;

	verts[6] = rect->x;
	verts[7] = rect->y + rect->h;

	glBindBuffer(GL_ARRAY_BUFFER, self->buffers.vertex);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

/**
 * @fn void Renderer::drawTexture(const Renderer *self, GLuint texture, const SDL_Rect *dest)
 * @memberof Renderer
 */
static void drawTexture(const Renderer *self, GLuint texture, const SDL_Rect *rect) {

	assert(rect);

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

	glBindTexture(GL_TEXTURE_2D, texture);

	glBindBuffer(GL_ARRAY_BUFFER, self->buffers.vertex);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, self->buffers.texcoord);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texcoords), texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindTexture(GL_TEXTURE_2D, self->textures.null);
}

/**
 * @fn void Renderer::endFrame(Renderer *self)
 * @memberof Renderer
 */
static void endFrame(Renderer *self) {

	glDisableVertexAttribArray(self->attributes.texcoord);
	glDisableVertexAttribArray(self->attributes.vertex);

	glUseProgram(0);

	SDL_Window *window = SDL_GL_GetCurrentWindow();

	int dw, dh;
	SDL_GL_GetDrawableSize(window, &dw, &dh);

	glScissor(0, 0, dw, dh);

	glDisable(GL_SCISSOR_TEST);

	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);

	const GLenum err = glGetError();
	if (err) {
		MVC_LogError("GL error: %d\n", err);
	}
}

/**
 * @fn void Renderer::fillRect(const Renderer *self, const SDL_Rect *rect)
 * @memberof Renderer
 */
static void fillRect(const Renderer *self, const SDL_Rect *rect) {

	assert(rect);

	GLint verts[8];

	verts[0] = rect->x - 1;
	verts[1] = rect->y;

	verts[2] = rect->x + rect->w;
	verts[3] = rect->y;

	verts[4] = rect->x + rect->w;
	verts[5] = rect->y + rect->h + 1;

	verts[6] = rect->x - 1;
	verts[7] = rect->y + rect->h + 1;

	glBindBuffer(GL_ARRAY_BUFFER, self->buffers.vertex);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

/**
 * @fn Renderer *Renderer::init(Renderer *self)
 * @memberof Renderer
 */
static Renderer *init(Renderer *self) {
	
	self = (Renderer *) super(Object, self, init);
	if (self) {

		self->views = $$(MutableArray, array);
		assert(self->views);

		$(self, renderDeviceDidReset);
	}

	return self;
}

/**
 * @brief Comparator for sorting Views by depth (Painter's Algorithm).
 */
static Order render_sort(const ident a, const ident b) {

	const int depthA = $((const View *) a, depth);
	const int depthB = $((const View *) b, depth);

	return (Order) depthA - depthB;
}

/**
 * @brief ArrayEnumerator for rendering Views.
 */
static void render_renderView(const Array *array, ident obj, ident data) {

	View *view = (View *) obj;

	const SDL_Rect frame = $(view, clippingFrame);
	if (frame.w && frame.h) {

		const SDL_Rect scissor = MVC_TransformToWindow($(view, window), &frame);

		glScissor(scissor.x - 1, scissor.y - 1, scissor.w + 1, scissor.h + 1);

		$(view, render, (Renderer *) data);
	}
}

/**
 * @fn void Renderer::drawViews(Renderer *self)
 * @memberof Renderer
 */
static void render(Renderer *self) {

	$(self->views, sort, render_sort);

	$((Array *) self->views, enumerateObjects, render_renderView, self);

	$(self->views, removeAllObjects);
}

/**
 * @fn void Renderer::renderDeviceDidReset(Renderer *self)
 * @memberof Renderer
 */
static void renderDeviceDidReset(Renderer *self) {

	Shader *vertexShader = $(alloc(Shader), initWithSource, GL_VERTEX_SHADER, "\
		#version 120 \n\
		attribute vec2 texcoord; \
		attribute vec2 vertex; \
		varying vec2 st; \
		void main() { \
			gl_Position = gl_ProjectionMatrix * vec4(vertex.x, vertex.y, 0.0, 1.0); \
			st = texcoord; \
		}");

	Shader *fragmentShader = $(alloc(Shader), initWithSource, GL_FRAGMENT_SHADER, "\
		#version 120 \n\
		uniform vec4 color; \
		uniform sampler2D sampler; \
		varying vec2 st; \
		void main() { \
			gl_FragColor = color * texture2D(sampler, st); \
		}");

	self->program = (Program *) $(alloc(Program), init);
	assert(self->program);

	$(self->program, attachShader, vertexShader);
	$(self->program, attachShader, fragmentShader);

	release(vertexShader);
	release(fragmentShader);

	$(self->program, link);
	assert(self->program->name);

	self->attributes.texcoord = $(self->program, getAttributeLocation, "texcoord");
	self->attributes.vertex = $(self->program, getAttributeLocation, "vertex");
	self->uniforms.color = $(self->program, getUniformLocation, "color");

	const size_t len = 64;

	glGenBuffers(1, &self->buffers.texcoord);
	assert(self->buffers.texcoord);

	glBindBuffer(GL_ARRAY_BUFFER, self->buffers.texcoord);
	glBufferData(GL_ARRAY_BUFFER, len * sizeof(GLfloat) * 2, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(self->attributes.texcoord, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &self->buffers.vertex);
	assert(self->buffers.vertex);

	glBindBuffer(GL_ARRAY_BUFFER, self->buffers.vertex);
	glBufferData(GL_ARRAY_BUFFER, len * sizeof(GLint) * 2, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(self->attributes.vertex, 2, GL_INT, GL_FALSE, 0, NULL);

	self->textures.null = $(self, createTexture, _null->surface);
	assert(self->textures.null);

	assert(glGetError() == GL_NO_ERROR);
}

/**
 * @fn void Renderer::setDrawColor(Renderer *self, const SDL_Color *color)
 * @memberof Renderer
 */
static void setDrawColor(Renderer *self, const SDL_Color *color) {
	glUniform4fv(self->uniforms.color, 1, (const GLfloat []) {
		color->r / 255.0,
		color->g / 255.0,
		color->b / 255.0,
		color->a / 255.0
	});
}

#pragma mark - Class lifecycle

#include <ObjectivelyMVC/Program.h>

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((RendererInterface *) clazz->def->interface)->addView = addView;
	((RendererInterface *) clazz->def->interface)->beginFrame = beginFrame;
	((RendererInterface *) clazz->def->interface)->createTexture = createTexture;
	((RendererInterface *) clazz->def->interface)->drawLine = drawLine;
	((RendererInterface *) clazz->def->interface)->drawLines = drawLines;
	((RendererInterface *) clazz->def->interface)->drawRect = drawRect;
	((RendererInterface *) clazz->def->interface)->drawTexture = drawTexture;
	((RendererInterface *) clazz->def->interface)->endFrame = endFrame;
	((RendererInterface *) clazz->def->interface)->fillRect = fillRect;
	((RendererInterface *) clazz->def->interface)->init = init;
	((RendererInterface *) clazz->def->interface)->render = render;
	((RendererInterface *) clazz->def->interface)->renderDeviceDidReset = renderDeviceDidReset;
	((RendererInterface *) clazz->def->interface)->setDrawColor = setDrawColor;

	initializeOpenGL();

	_null = $(alloc(Image), initWithBytes, _nullData, lengthof(_nullData));
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_null);
}

Class _Renderer = {
	.name = "Renderer",
	.superclass = &_Object,
	.instanceSize = sizeof(Renderer),
	.interfaceOffset = offsetof(Renderer, interface),
	.interfaceSize = sizeof(RendererInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class

