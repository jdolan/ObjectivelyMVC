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

#include <ObjectivelyMVC/ScrollView.h>

#define _Class _ScrollView

#pragma mark - ScrollView

/**
 * @fn ScrollView *ScrollView::initWithFrame(ScrollView *self, const SDL_Rect *frame, ControlStyle style)
 *
 * @memberof ScrollView
 */
static ScrollView *initWithFrame(ScrollView *self, const SDL_Rect *frame, ControlStyle style) {
	
	self = (ScrollView *) super(Control, self, initWithFrame, frame, style);
	if (self) {
		self->scrollEnabled = true;
	}
	
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ScrollViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

Class _ScrollView = {
	.name = "ScrollView",
	.superclass = &_Control,
	.instanceSize = sizeof(ScrollView),
	.interfaceOffset = offsetof(ScrollView, interface),
	.interfaceSize = sizeof(ScrollViewInterface),
	.initialize = initialize,
};

#undef _Class
