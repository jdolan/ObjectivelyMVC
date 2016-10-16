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

#include <SDL2/SDL_video.h>

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/Program.h>

/**
 * @file
 * @brief The Renderer is responsible for rasterizing the View hierarchy of a WindowController.
 */

typedef struct Renderer Renderer;
typedef struct RendererInterface RendererInterface;

/**
 * @brief The Renderer is responsible for rasterizing the View hierarchy of a WindowController.
 * @extends Object
 */
struct Renderer {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 * @protected
	 */
	RendererInterface *interface;

	/**
	 * @brief The GLSL Program to be bound for rendering.
	 */
	Program *program;

	/**
	 * @brief The Views to be drawn each frame.
	 */
	MutableArray *views;
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
	 * @fn void Renderer::addView(Renderer *self, View *view)
	 * @brief Adds the View to the Renderer for the current frame.
	 * @param self The Renderer.
	 * @param view The View.
	 * @memberof Renderer
	 */
	void (*addView)(Renderer *self, View *view);

	/**
	 * @fn void Renderer::beginFrame(const Renderer *self)
	 * @brief Sets up OpenGL state.
	 * @param self The Renderer.
	 * @remarks This method is called by the WindowController to begin rendering. Override this 
	 * method for custom OpenGL state setup, if desired.
	 * @memberof Renderer
	 */
	void (*beginFrame)(const Renderer *self);

	/**
	 * @fn void Renderer::createTexture(const Renderer *self, const SDL_Surface *surface)
	 * @brief Generates and binds to an OpenGL texture object, uploading the given surface.
	 * @param self The Renderer.
	 * @param surface The surface.
	 * @return The OpenGL texture name, or `0` on error.
	 * @memberof Renderer
	 */
	GLuint (*createTexture)(const Renderer *self, const SDL_Surface *surface);

	/**
	 * @fn void Renderer::drawLine(const Renderer *self, const SDL_Point *points)
	 * @brief Draws a line segment between two points using `GL_LINE_STRIP`.
	 * @param self The Renderer.
	 * @param points The points.
	 * @memberof Renderer
	 */
	void (*drawLine)(const Renderer *self, const SDL_Point *points);

	/**
	 * @fn void Renderer::drawLines(const Renderer *self, const SDL_Point *points, GLuint count)
	 * @brief Draws line segments between adjacent points using `GL_LINE_STRIP`.
	 * @param self The Renderer.
	 * @param points The points.
	 * @param count The length of points.
	 * @memberof Renderer
	 */
	void (*drawLines)(const Renderer *self, const SDL_Point *points, GLuint count);

	/**
	 * @fn void Renderer::drawRect(const Renderer *self, const SDL_Rect *rect)
	 * @brief Draws a rectangle using `GL_LINE_LOOP`.
	 * @param self The Renderer.
	 * @param rect The rectangle.
	 * @memberof Renderer
	 */
	void (*drawRect)(const Renderer *self, const SDL_Rect *rect);

	/**
	 * @fn void Renderer::drawTexture(const Renderer *self, GLuint texture, const SDL_Rect *dest)
	 * @brief Draws textured `GL_QUAD` in the given rectangle.
	 * @param self The Renderer.
	 * @param texture The texture.
	 * @param dest The destination in screen coordinates.
	 * @memberof Renderer
	 */
	void (*drawTexture)(const Renderer *self, GLuint texture, const SDL_Rect *dest);

	/**
	 * @fn void Renderer::endFrame(const Renderer *self)
	 * @brief Resets OpenGL state. Does *not* swap buffers.
	 * @param self The Renderer.
	 * @remarks This method is called by the WindowController to end rendering. Override this
	 * method for custom OpenGL state teardown, if desired.
	 * @memberof Renderer
	 */
	void (*endFrame)(const Renderer *self);

	/**
	 * @fn void Renderer::fillRect(const Renderer *self, const SDL_Rect *rect)
	 * @brief Fills a rectangle using `glRecti`.
	 * @param self The Renderer.
	 * @param rect The rectangle.
	 * @memberof Renderer
	 */
	void (*fillRect)(const Renderer *self, const SDL_Rect *rect);

	/**
	 * @fn Renderer *Renderer::init(Renderer *self)
	 * @brief Initializes this Renderer.
	 * @param self The Renderer.
	 * @return The initialized Renderer, or `NULL` on error.
	 * @memberof Renderer
	 */
	Renderer *(*init)(Renderer *self);

	/**
	 * @fn void Renderer::render(Renderer *self)
	 * @brief Renders all Views added for the current frame, sorted by depth.
	 * @param self The Renderer.
	 * @memberof Renderer
	 */
	void (*render)(Renderer *self);
};

/**
 * @brief The Renderer Class.
 */
extern Class _Renderer;

