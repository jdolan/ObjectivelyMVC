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
#include <ObjectivelyMVC/Renderer.h>
#include <ObjectivelyMVC/View.h>

#define _Class _Renderer

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Renderer *this = (Renderer *) self;

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
 * @fn void Renderer::beginFrame(const Renderer *self)
 * @memberof Renderer
 */
static void beginFrame(const Renderer *self) {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_SCISSOR_TEST);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
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

	glVertexPointer(2, GL_INT, 0, points);
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

	glVertexPointer(2, GL_INT, 0, verts);
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

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glVertexPointer(2, GL_INT, 0, verts);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisable(GL_TEXTURE_2D);
}

/**
 * @fn void Renderer::endFrame(const Renderer *self)
 * @memberof Renderer
 */
static void endFrame(const Renderer *self) {

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	SDL_Window *window = SDL_GL_GetCurrentWindow();

	int dw, dh;
	SDL_GL_GetDrawableSize(window, &dw, &dh);

	glScissor(0, 0, dw, dh);

	glDisable(GL_SCISSOR_TEST);

	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);

	glColor3f(1.0, 1.0, 1.0);

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

	glRecti(rect->x - 1, rect->y, rect->x + rect->w, rect->y + rect->h + 1);
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

#pragma mark - Class lifecycle

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
}

Class _Renderer = {
	.name = "Renderer",
	.superclass = &_Object,
	.instanceSize = sizeof(Renderer),
	.interfaceOffset = offsetof(Renderer, interface),
	.interfaceSize = sizeof(RendererInterface),
	.initialize = initialize,
};

#undef _Class

