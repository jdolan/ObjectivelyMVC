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

#pragma once

#include <Objectively/Object.h>

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>

/**
 * @file
 *
 * @brief The OpenGL renderer.
 */

typedef struct Renderer Renderer;
typedef struct RendererInterface RendererInterface;

/**
 * @brief The Renderer type.
 *
 * @extends Object
 *
 * @ingroup
 */
struct Renderer {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Object object;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	RendererInterface *interface;
};

/**
 * @brief The Renderer interface.
 */
struct RendererInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Renderer::beginFrame(const Renderer *self)
	 *
	 * @brief Sets up OpenGL state.
	 *
	 * @remarks This method is called by the WindowController, and you generally should
	 * not be concerned with it.
	 *
	 * @memberof Renderer
	 */
	void (*beginFrame)(const Renderer *self);

	/**
	 * @fn void Renderer::createTexture(const Renderer *self, const SDL_Surface *surface)
	 *
	 * @brief Generates and binds to an OpenGL texture object, uploading the given surface.
	 *
	 * @param surface The surface.
	 *
	 * @return The OpenGL texture name, or `0` on error.
	 *
	 * @memberof Renderer
	 */
	GLuint (*createTexture)(const Renderer *self, const SDL_Surface *surface);

	/**
	 * @fn void Renderer::drawLine(const Renderer *self, const SDL_Point *points)
	 *
	 * @brief Draws a line segment between two points using `GL_LINE_STRIP`.
	 *
	 * @param points The points.
	 *
	 * @memberof Renderer
	 */
	void (*drawLine)(const Renderer *self, const SDL_Point *points);

	/**
	 * @fn void Renderer::drawLines(const Renderer *self, const SDL_Point *points, GLuint count)
	 *
	 * @brief Draws line segments between adjacent points using `GL_LINE_STRIP`.
	 *
	 * @param points The points.
	 * @param count The length of points.
	 *
	 * @memberof Renderer
	 */
	void (*drawLines)(const Renderer *self, const SDL_Point *points, GLuint count);

	/**
	 * @fn void Renderer::drawRect(const Renderer *self, const SDL_Rect *rect)
	 *
	 * @brief Draws a rectangle using `GL_LINE_LOOP`.
	 *
	 * @param rect The rectangle.
	 *
	 * @memberof Renderer
	 */
	void (*drawRect)(const Renderer *self, const SDL_Rect *rect);

	/**
	 * @fn void Renderer::drawTexture(const Renderer *self, GLuint texture, const SDL_Rect *dest)
	 *
	 * @brief Draws textured `GL_QUAD` in the given rectangle.
	 *
	 * @param texture The texture.
	 * @param dest The destination in screen coordinates.
	 *
	 * @memberof Renderer
	 */
	void (*drawTexture)(const Renderer *self, GLuint texture, const SDL_Rect *dest);

	/**
	 * @fn void Renderer::endFrame(const Renderer *self)
	 *
	 * @brief Resets OpenGL state. Does *not* swap buffers.
	 *
	 * @remarks This method is called by the WindowController, and you generally should
	 * not be concerned with it.
	 *
	 * @memberof Renderer
	 */
	void (*endFrame)(const Renderer *self);

	/**
	 * @fn void Renderer::fillRect(const Renderer *self, const SDL_Rect *rect)
	 *
	 * @brief Fills a rectangle using `glRecti`.
	 *
	 * @param rect The rectangle.
	 *
	 * @memberof Renderer
	 */
	void (*fillRect)(const Renderer *self, const SDL_Rect *rect);

	/**
	 * @fn Renderer *Renderer::init(Renderer *self)
	 *
	 * @brief Initializes this Renderer.
	 *
	 * @return The initialized Renderer, or `NULL` on error.
	 *
	 * @memberof Renderer
	 */
	Renderer *(*init)(Renderer *self);
};

/**
 * @brief The Renderer Class.
 */
extern Class _Renderer;

