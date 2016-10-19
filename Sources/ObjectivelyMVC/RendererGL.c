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
#include <ObjectivelyMVC/RendererGL.h>

#define _Class _RendererGL

#pragma mark - Renderer

/**
 * @see Renderer::beginFrame(Renderer *self)
 */
static void beginFrame(Renderer *self) {

	super(Renderer, self, beginFrame);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_SCISSOR_TEST);
}

/**
 * @see Renderer::createTexture(const Renderer *self, const SDL_Surface *surface)
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
			MVC_LogError("Invalid surface format: %s\n", SDL_GetPixelFormatName(surface->format->format));
			return NULL;
	}

	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

	return (ident) (ptrdiff_t) texture;
}

/**
 * @see Renderer::destroyTexture(const Renderer *self, ident texture)
 */
static void destroyTexture(const Renderer *self, ident texture) {

	GLuint name = (GLuint) texture;

	glDeleteTextures(1, &name);
}

/**
 * @see Renderer::endFrame(Renderer *self)
 */
static void endFrame(Renderer *self) {

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

	super(Renderer, self, endFrame);
}

/**
 * @see Renderer::setScissor(Renderer *self, const SDL_Rect *rect)
 */
static void setScissor(Renderer *self, const SDL_Rect *rect) {
	glScissor(rect->x, rect->y, rect->w, rect->h);
}

#pragma mark - RendererGL

/**
 * @fn RendererGL *RendererGL::init(RendererGL *self)
 * @memberof RendererGL
 */
static RendererGL *init(RendererGL *self) {
	return (RendererGL *) super(Renderer, self, init);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((RendererInterface *) clazz->def->interface)->beginFrame = beginFrame;
	((RendererInterface *) clazz->def->interface)->createTexture = createTexture;
	((RendererInterface *) clazz->def->interface)->destroyTexture = destroyTexture;
	((RendererInterface *) clazz->def->interface)->endFrame = endFrame;
	((RendererInterface *) clazz->def->interface)->setScissor = setScissor;

	((RendererGLInterface *) clazz->def->interface)->init = init;
}

Class _RendererGL = {
	.name = "RendererGL",
	.superclass = &_Renderer,
	.instanceSize = sizeof(RendererGL),
	.interfaceOffset = offsetof(RendererGL, interface),
	.interfaceSize = sizeof(RendererGLInterface),
	.initialize = initialize,
};

#undef _Class

