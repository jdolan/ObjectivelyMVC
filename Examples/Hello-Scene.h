/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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

#include <ObjectivelyMVC.h>

/**
 * @brief Initializes the scene GPU resources.
 * @param renderer The MVC Renderer (provides RenderDevice and swapchain format).
 * Call once after the Renderer has been initialized.
 */
void initScene(Renderer *renderer);

/**
 * @brief Draws one frame of the rotating cube scene into renderer->cmd.
 * @param renderer The MVC Renderer (cmd and swapchain must be valid).
 * Call after beginFrame but before endFrame.
 */
void drawScene(Renderer *renderer);
