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

#include <ObjectivelyMVC/OpenGL.h>
#include <ObjectivelyMVC/Renderer.h>

/**
 * @file
 * @brief A base class for OpenGL Renderer implementations.
 */

typedef struct RendererGL RendererGL;
typedef struct RendererGLInterface RendererGLInterface;

/**
 * @brief A base class for OpenGL Renderer implementations.
 * @extends Renderer
 */
struct RendererGL {
	
	/**
	 * @brief The parent.
	 */
	Renderer renderer;
	
	/**
	 * @brief The typed interface.
	 * @protected
	 */
	RendererGLInterface *interface;
};

/**
 * @brief The RendererGL interface.
 */
struct RendererGLInterface {
	
	/**
	 * @brief The parent interface.
	 */
	RendererInterface rendererInterface;
	
	/**
	 * @protected
	 * @fn RendererGL *RendererGL::init(RendererGL *self)
	 * @brief Initializes this OpenGL Renderer.
	 * @param The RendererGL.
	 * @return The initialized RendererGL, or `NULL` on error.
	 * @memberof RendererGL
	 */
	RendererGL *(*init)(RendererGL *self);
};

/**
 * @brief The RendererGL Class.
 */
extern Class _RendererGL;

