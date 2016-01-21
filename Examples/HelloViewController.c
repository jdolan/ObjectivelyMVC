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

#include <ObjectivelyMVC.h>

#include "HelloViewController.h"

#define _Class _HelloViewController

#pragma mark - Actions

/**
 * @brief ActionFunction for "Click Me" Button.
 */
static void buttonAction(ident sender, const SDL_Event *event, ident data) {
	printf("Hello World!\n");
}

#pragma mark - ViewController

/**
 * @see ViewController::loadView(ViewController *)
 */
static void loadView(ViewController *self) {

	super(ViewController, self, loadView);
	
	self->view->backgroundColor = Colors.Black;
	
	View *container = $(alloc(View), initWithFrame, NULL);
	$(self->view, addSubview, container);
	
	Label *label = $(alloc(Label), initWithText, "Hello World!", NULL);
	$(container, addSubview, (View *) label);
	
	Control *button = (Control *) $(alloc(Button), initWithType, ButtonTypeDefault);
	$(button->label, setText, "Button");
	$(button, addActionForEventType, SDL_MOUSEBUTTONUP, buttonAction, NULL);
	$(container, addSubview, (View *) button);
	
	$((View *) button, constrainTo, Bottom, container, Bottom, 10);
	$((View *) button, constrainTo, Right, container, Right, 10);
	
	$(container, constrain, Width, 400, INT32_MAX);
	$(container, sizeToFit);
	$(container, layoutSubviews);
	
	release(label);
	release(container);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ViewControllerInterface *) clazz->interface)->loadView = loadView;
}

Class _HelloViewController = {
	.name = "HelloViewController",
	.superclass = &_ViewController,
	.instanceSize = sizeof(HelloViewController),
	.interfaceOffset = offsetof(HelloViewController, interface),
	.interfaceSize = sizeof(HelloViewControllerInterface),
	.initialize = initialize,
};

#undef _Class
