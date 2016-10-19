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
#include <ObjectivelyMVC/Types.h>
#include <ObjectivelyMVC/RendererGL10.h>
#include <ObjectivelyMVC/View.h>

#define _Class _RendererGL10

#pragma mark - RendererGL10

/**
 * @fn void RendererGL10::beginFrame(Renderer *self)
 * @memberof RendererGL10
 */
static void beginFrame(Renderer *self) {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_SCISSOR_TEST);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	super(Renderer, self, beginFrame);
}

/**
 * @fn void RendererGL10::createTexture(const Renderer *self, const SDL_Surface *surface)
 * @memberof RendererGL10
 */
static ident createTexture(const Renderer *self, const SDL_Surface *surface) {

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
			return NULL;
	}

	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format,
				 GL_UNSIGNED_BYTE, surface->pixels);

	return (ident)(ptrdiff_t)texture;
}

/**
 * @fn void RendererGL10::destroyTexture(const Renderer *self, ident texture)
 * @memberof RendererGL10
 */
static void destroyTexture(const Renderer *self, ident texture) {

	GLuint id = (GLuint)texture;
	glDeleteTextures(1, &id);
}

/**
 * @fn void RendererGL10::drawLines(const Renderer *self, const SDL_Point *points, size_t count)
 * @memberof RendererGL10
 */
static void drawLines(const Renderer *self, const SDL_Point *points, size_t count) {
	
	assert(points);
	
	glVertexPointer(2, GL_INT, 0, points);
	glDrawArrays(GL_LINE_STRIP, 0, count);
}

/**
 * @fn void RendererGL10::drawRect(const Renderer *self, const SDL_Rect *rect)
 * @memberof RendererGL10
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
	
	glVertexPointer(2, GL_INT, 0, verts);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

/**
 * @fn void RendererGL10::drawRectFilled(const Renderer *self, const SDL_Rect *rect)
 * @memberof RendererGL10
 */
static void drawRectFilled(const Renderer *self, const SDL_Rect *rect) {

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
	
	glRecti(rect->x - 1, rect->y, rect->x + rect->w, rect->y + rect->h + 1);
}

/**
 * @fn void RendererGL10::drawTexture(const Renderer *self, ident texture, const SDL_Rect *dest)
 * @memberof RendererGL10
 */
static void drawTexture(const Renderer *self, ident texture, const SDL_Rect *rect) {

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

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, (GLuint)texture);

	glVertexPointer(2, GL_INT, 0, verts);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisable(GL_TEXTURE_2D);
}

/**
 * @fn void RendererGL10::endFrame(RendererGL10 *self)
 * @memberof RendererGL10
 */
static void endFrame(Renderer *self) {
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	SDL_Window *window = SDL_GL_GetCurrentWindow();

	SDL_Rect rect = { 0, 0 };
	SDL_GL_GetDrawableSize(window, &rect.w, &rect.h);

	$(self, setScissor, &rect);

	glDisable(GL_SCISSOR_TEST);

	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);

	const GLenum err = glGetError();
	if (err) {
		MVC_LogError("GL error: %d\n", err);
	}
}

/**
 * @fn void RendererGL10::setDrawColor(Renderer *self, const SDL_Color *color)
 * @memberof RendererGL10
 */
static void setDrawColor(Renderer *self, const SDL_Color *color) {

	glColor4ubv((const GLubyte *) color);
}

/**
 * @fn void RendererGL10::setScissor(Renderer *self, const SDL_Rect *rect)
 * @memberof RendererGL10
 */
static void setScissor(Renderer *self, const SDL_Rect *rect) {
	glScissor(rect->x - 1, rect->y - 1, rect->w + 1, rect->h + 1);
}

/**
 * @fn RendererGL10 *RendererGL10::init(RendererGL10 *self)
 * @memberof RendererGL10
 */
static RendererGL10 *init(RendererGL10 *self) {
	
	self = (RendererGL10 *) super(Renderer, self, init);

	if (self) {

	}
	
	return self;
}

#pragma mark - Class lifecycle

#include <ObjectivelyMVC/Program.h>

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((RendererInterface *) clazz->def->interface)->beginFrame = beginFrame;
	((RendererInterface *) clazz->def->interface)->createTexture = createTexture;
	((RendererInterface *) clazz->def->interface)->destroyTexture = destroyTexture;
	((RendererInterface *) clazz->def->interface)->drawLines = drawLines;
	((RendererInterface *) clazz->def->interface)->drawRect = drawRect;
	((RendererInterface *) clazz->def->interface)->drawRectFilled = drawRectFilled;
	((RendererInterface *) clazz->def->interface)->drawTexture = drawTexture;
	((RendererInterface *) clazz->def->interface)->endFrame = endFrame;
	((RendererInterface *) clazz->def->interface)->setDrawColor = setDrawColor;
	((RendererInterface *) clazz->def->interface)->setScissor = setScissor;

	((RendererGL10Interface *) clazz->def->interface)->init = init;
}

Class _RendererGL10 = {
	.name = "RendererGL10",
	.superclass = &_Renderer,
	.instanceSize = sizeof(RendererGL10),
	.interfaceOffset = offsetof(RendererGL10, interface),
	.interfaceSize = sizeof(RendererGL10Interface),
	.initialize = initialize
};

#undef _Class

