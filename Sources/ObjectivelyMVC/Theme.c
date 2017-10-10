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

#include <ObjectivelyMVC/Theme.h>

struct _Theme Theme = {
	.alternateBackgroundColor = MakeColor(80, 80, 80, 240),
	.backgroundColor = MakeColor(64, 64, 64, 240),
	.borderColor = MakeColor(152, 152, 152, 96),
	.containerPadding = MakePadding(10, 10, 10, 10),
	.containerSpacing = 10,
	.controlHeight = 32,
	.controlPadding = MakePadding(4, 10, 4, 10),
	.darkBackgroundColor = MakeColor(48, 48, 48, 240),
	.darkBorderColor = MakeColor(32, 32, 32, 224),
	.darkForegroundColor = MakeColor(128, 128, 128, 255),
	.disabledbackgroundColor = MakeColor(128, 128, 128, 128),
	.disabledForegroundColor = MakeColor(96, 96, 96, 255),
	.focusedBackgroundColor = MakeColor(72, 72, 72, 255),
	.focusedForegroundColor = MakeColor(255, 255, 255, 255),
	.foregroundColor = MakeColor(255, 255, 255, 255),
	.highlightedBackgroundColor = MakeColor(105, 105, 105, 255),
	.highlightedForegroundColor = MakeColor(255, 255, 255, 255),
	.lightBackgroundColor = MakeColor(96, 96, 96, 240),
	.lightBorderColor = MakeColor(192, 192, 192, 224),
	.lightForegroundColor = MakeColor(192, 192, 192, 255),
	.selectedBackgroundColor = MakeColor(96, 96, 96, 240),
	.selectedForegroundColor = MakeColor(255, 255, 255, 255),
	.selectionBackgroundColor = MakeColor(224, 224, 224, 16),
	.selectionForegroundColor = MakeColor(64, 64, 64, 255),
};
