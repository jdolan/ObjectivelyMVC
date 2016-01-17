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

#include <ObjectivelyMVC/Colors.h>

const Colors_t Colors = {
	
	.white = { 255, 255, 255, 255 },
	.black = { 0, 0, 0, 255 },
	.gray =  { 128, 128, 128, 255 },
	.red = { 255, 0, 0, 255 },
	.green = { 0, 255, 0, 255 },
	.blue = { 0, 0, 255, 255 },
	.clear = { 255, 255, 255, 0 }
};
