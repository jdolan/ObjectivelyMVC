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

#include <SDL_opengl.h>

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief The Renderer is responsible for rasterizing the View hierarchy of a WindowController.
 * @details This class provides an OpenGL 1.x implementation of the RendererInterface. Applications
 * may extend this class and provide an implementation that meets their own OpenGL version
 * requirements.
 */

typedef struct Renderer Renderer;
typedef struct RendererInterface RendererInterface;

/**
 * @brief The Renderer is responsible for rasterizing the View hierarchy of a WindowController.
 * @details This class provides an OpenGL 1.x implementation of the RendererInterface. Applications
 * may extend this class and provide an implementation that meets their own OpenGL version
 * requirements.
 * @extends Object
 */
struct Renderer {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	RendererInterface *interface;
};

/**
 * @brief The Renderer interface.
 */
struct RendererInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Renderer::beginFrame(Renderer *self)
	 * @brief Sets up OpenGL state.
	 * @param self The Renderer.
	 * @remarks This method is called by the WindowController to begin rendering. Override this
	 * method for custom OpenGL state setup, if desired.
	 * @memberof Renderer
	 */
	void (*beginFrame)(Renderer *self);

	/**
	 * @fn GLuint Renderer::createTexture(const Renderer *self, const SDL_Surface *surface)
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
	 * @fn void Renderer::drawLines(const Renderer *self, const SDL_Point *points, size_t count)
	 * @brief Draws line segments between adjacent points using `GL_LINE_STRIP`.
	 * @param self The Renderer.
	 * @param points The points.
	 * @param count The length of points.
	 * @memberof Renderer
	 */
	void (*drawLines)(const Renderer *self, const SDL_Point *points, size_t count);

	/**
	 * @fn void Renderer::drawRect(const Renderer *self, const SDL_Rect *rect)
	 * @brief Draws a rectangle using `GL_LINE_LOOP`.
	 * @param self The Renderer.
	 * @param rect The rectangle.
	 * @memberof Renderer
	 */
	void (*drawRect)(const Renderer *self, const SDL_Rect *rect);

	/**
	 * @fn void Renderer::drawRectFilled(const Renderer *self, const SDL_Rect *rect)
	 * @brief Fills a rectangle using `glRecti`.
	 * @param self The Renderer.
	 * @param rect The rectangle.
	 * @memberof Renderer
	 */
	void (*drawRectFilled)(const Renderer *self, const SDL_Rect *rect);

	/**
	 * @fn void Renderer::drawTexture(const Renderer *self, GLuint texture, const SDL_Rect *dest)
	 * @brief Draws a textured `GL_QUAD` in the given rectangle.
	 * @param self The Renderer.
	 * @param texture The texture.
	 * @param dest The destination in screen coordinates.
	 * @memberof Renderer
	 */
	void (*drawTexture)(const Renderer *self, GLuint texture, const SDL_Rect *dest);

	/**
	 * @fn void Renderer::drawView(Renderer *self, View *view)
	 * @brief Draws the given View, setting the clipping frame and invoking View::render.
	 * @param self The Renderer.
	 * @param view The View.
	 * @memberof Renderer
	 */
	void (*drawView)(Renderer *self, View *view);

	/**
	 * @fn void Renderer::endFrame(const Renderer *self)
	 * @brief Resets OpenGL state. Does *not* swap buffers.
	 * @param self The Renderer.
	 * @remarks This method is called by the WindowController to end rendering. Override this
	 * method for custom OpenGL state teardown, if desired.
	 * @memberof Renderer
	 */
	void (*endFrame)(Renderer *self);

	/**
	 * @protected
	 * @fn Renderer *Renderer::init(Renderer *self)
	 * @brief Initializes this Renderer.
	 * @param self The Renderer.
	 * @return The initialized Renderer, or `NULL` on error.
	 * @memberof Renderer
	 */
	Renderer *(*init)(Renderer *self);

	/**
	 * @fn void Renderer::renderDeviceDidReset(Renderer *self)
	 * @brief This method is invoked when the render device has reset.
	 * @param self The Renderer.
	 * @remarks Subclasses may override this method to allocate any OpenGL objects they require.
	 * @memberof Renderer
	 */
	void (*renderDeviceDidReset)(Renderer *self);

	/**
	 * @fn void Renderer::renderDeviceWillReset(Renderer *self)
	 * @brief This method is invoked when the render device will become reset.
	 * @param self The Renderer.
	 * @remarks Subclasses should override this method to free any OpenGL objects they own.
	 * @memberof Renderer
	 */
	void (*renderDeviceWillReset)(Renderer *self);

	/**
	 * @fn void Renderer::setClippingFrame(Renderer *self, const SDL_Rect *clippingFrame)
	 * @brief Sets the clipping frame for draw operations.
	 * @details Primitives which fall outside of the clipping frame will not be visible.
	 * @param self The Renderer.
	 * @param clippingFrame The clipping frame, or `NULL` to disable clipping.
	 * @memberof Renderer
	 */
	void (*setClippingFrame)(Renderer *self, const SDL_Rect *clippingFrame);

	/**
	 * @fn void Renderer::setDrawColor(Renderer *self, const SDL_Color *color)
	 * @brief Sets the primary color for drawing operations.
	 * @param self The Renderer.
	 * @param color The color.
	 * @memberof Renderer
	 */
	void (*setDrawColor)(Renderer *self, const SDL_Color *color);
};

/**
 * @fn Class *Renderer::_Renderer(void)
 * @brief The Renderer archetype.
 * @return The Renderer Class.
 * @memberof Renderer
 */
OBJECTIVELYMVC_EXPORT Class *_Renderer(void);

