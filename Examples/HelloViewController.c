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
	
	const SDL_Rect frame = { .x = 50, .y = 50, .w = 400, .h = 300 };
	self->view->frame = frame;
	self->view->backgroundColor.a = 128;
	
	StackView *stackView = $(alloc(StackView), initWithFrame, NULL);
	stackView->view.autoresizingMask = ViewAutoresizingWidth | ViewAutoresizingHeight;
	stackView->spacing = 10;
	stackView->view.padding.top = stackView->view.padding.bottom = 10;
	stackView->view.padding.left = stackView->view.padding.right = 10;
	
	$(self->view, addSubview, (View *) stackView);
	
	Label *label = $(alloc(Label), initWithText, "Hello World!", NULL);
	$((View *) stackView, addSubview, (View *) label);
	
	Control *button = (Control *) $(alloc(Button), initWithType, ButtonTypeDefault);
	$(button->label, setText, "This is a bigass super long Button");
	$(button, addActionForEventType, SDL_MOUSEBUTTONUP, buttonAction, NULL);
	$((View *) stackView, addSubview, (View *) button);
	
	release(label);
	release(stackView);
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
