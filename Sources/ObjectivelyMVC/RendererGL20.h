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

#include <ObjectivelyMVC/Renderer.h>
#include <ObjectivelyMVC/Program.h>

/**
 * @file
 * @brief The Renderer is responsible for rasterizing the View hierarchy of a WindowController.
 */

typedef struct RendererGL20 RendererGL20;
typedef struct RendererGL20Interface RendererGL20Interface;

/**
 * @brief The Renderer is responsible for rasterizing the View hierarchy of a WindowController.
 * @extends Object
 */
struct RendererGL20 {

	/**
	 * @brief The parent.
	 */
	Renderer renderer;

	/**
	 * @brief The typed interface.
	 * @protected
	 */
	RendererGL20Interface *interface;

	/**
	 * @brief The color uniform.
	 */
	GLint colorUniform;

	/**
	 * @brief The null texture object.
	 */
	GLuint nullTexture;

	/**
	 * @brief The GLSL Program to use for rendering.
	 */
	Program *program;

	/**
	 * @brief The texture coordinate attribute.
	 */
	GLint texcoordAttribute;

	/**
	 * @brief The texture coordinate buffer.
	 */
	GLuint texcoordBuffer;

	/**
	 * @brief The vertex attribute.
	 */
	GLint vertexAttribute;

	/**
	 * @brief The vertex buffer.
	 */
	GLuint vertexBuffer;
};

/**
 * @brief The Renderer interface.
 */
struct RendererGL20Interface {

	/**
	 * @brief The parent interface.
	 */
	RendererInterface rendererInterface;

	/**
	 * @fn RendererGL20 *RendererGL20::init(RendererGL20 *self)
	 * @brief Initializes this RendererGL20.
	 * @param self The RendererGL20.
	 * @return The initialized RendererGL20, or `NULL` on error.
	 * @memberof RendererGL20
	 */
	RendererGL20 *(*init) (RendererGL20 *self);
};

/**
 * @brief The RendererGL20 Class.
 */
extern Class _RendererGL20;

