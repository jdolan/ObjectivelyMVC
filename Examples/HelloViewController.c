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
static void action(ident sender, const SDL_Event *event, ident data) {
	printf("Hello World!\n");
}

#pragma mark - ViewController

/**
 * @see ViewController::loadView(ViewController *)
 */
static void loadView(ViewController *self) {

	super(ViewController, self, loadView);
	
	self->view->frame.x = 50, self->view->frame.y = 50;
	self->view->frame.w = 420, self->view->frame.h = 240;
	self->view->backgroundColor = Colors.DefaultColor;
	self->view->backgroundColor.a = 192;
	self->view->borderWidth = 1;
	
	StackView *stackView = $(alloc(StackView), initWithFrame, NULL);
	stackView->view.autoresizingMask = ViewAutoresizingFill;
	stackView->spacing = 10;
	stackView->view.padding.top = stackView->view.padding.bottom = 10;
	stackView->view.padding.left = stackView->view.padding.right = 10;
	
	$(self->view, addSubview, (View *) stackView);
	
	Button *button = $(alloc(Button), initWithFrame, NULL, ControlStyleDefault);
	$(button->title, setText, "This is a bigass super long button");
	$((Control *) button, addActionForEventType, SDL_MOUSEBUTTONUP, action, NULL);
	$((View *) stackView, addSubview, (View *) button);

	TextView *textView = $(alloc(TextView), initWithFrame, NULL, ControlStyleDefault);
	textView->defaultText = "This is a textview";
	$((View *) stackView, addSubview, (View *) textView);

	Control *checkbox = (Control *) $(alloc(Checkbox), initWithFrame, NULL, ControlStyleDefault);
	Label *checkboxLabel = $(alloc(Label), initWithText, "This is a checkbox:", NULL);
	Input *checkboxInput = $(alloc(Input), initWithOrientation, InputOrientationLeft, checkbox, checkboxLabel);
	$(checkbox, addActionForEventType, SDL_MOUSEBUTTONUP, action, NULL);

	$((View *) stackView, addSubview, (View *) checkboxInput);

	Select *select = $(alloc(Select), initWithFrame, NULL, ControlStyleDefault);
	$(select, addOption, "This is a select", NULL, (ident) 1);
	$(select, addOption, "This is an option", NULL, (ident) 2);
	$(select, addOption, "This is another", NULL, (ident) 3);
	$((View *) select, sizeToFit);
	
	$((View *) stackView, addSubview, (View *) select);

	Slider *slider = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);
	slider->min = 0.1, slider->max = 10.0, slider->step = 0.1, slider->value = 5.0;
	Label *sliderLabel = $(alloc(Label), initWithText, "5.0", NULL);
	Input *sliderInput = $(alloc(Input), initWithOrientation, InputOrientationRight, (Control *) slider, sliderLabel);
	
	$((View *) stackView, addSubview, (View *) sliderInput);

	release(button);
	release(textView);
	release(checkbox);
	release(checkboxInput);
	release(select);
	release(slider);
	release(sliderInput);
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
