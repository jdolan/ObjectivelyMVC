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

typedef struct RendererGL10 RendererGL10;
typedef struct RendererGL10Interface RendererGL10Interface;

/**
 * @brief The Renderer is responsible for rasterizing the View hierarchy of a WindowController.
 * @extends Object
 */
struct RendererGL10 {

	/**
	 * @brief The parent.
	 */
	Renderer renderer;

	/**
	 * @brief The typed interface.
	 * @protected
	 */
	RendererGL10Interface *interface;
};

/**
 * @brief The Renderer interface.
 */
struct RendererGL10Interface {

	/**
	 * @brief The parent interface.
	 */
	RendererInterface rendererInterface;

	/**
	 * @fn RendererGL10 *RendererGL10::init(RendererGL10 *self)
	 * @brief Initializes this RendererGL10.
	 * @param self The RendererGL10.
	 * @return The initialized RendererGL10, or `NULL` on error.
	 * @memberof RendererGL10
	 */
	RendererGL10 *(*init) (RendererGL10 *self);
};

/**
 * @brief The RendererGL10 Class.
 */
extern Class _RendererGL10;

