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

#include <ObjectivelyMVC.h>

#include "HelloViewController.h"

#define _Class _HelloViewController

#pragma mark - Actions and delegate callbacks

/**
 * @brief ActionFunction for Button.
 */
static void buttonAction(Control *control, const SDL_Event *event, ident data) {
	printf("Button clicked\n");
}

/**
 * @brief ActionFunction for Checkbox.
 */
static void checkboxAction(Control *control, const SDL_Event *event, ident data) {
	if ($(control, selected)) {
		printf("Checkbox selected\n");
	} else {
		printf("Checkbox deselected\n");
	}
}

/**
 * @brief SliderDelegate callback.
 */
static void sliderDidSetValue(Slider *slider) {
	char chars[8];

	snprintf(chars, sizeof(chars), "%.1f", slider->value);
	$((Label *) slider->delegate.data, setText, chars);
}

#pragma mark - ViewController

/**
 * @see ViewController::loadView(ViewController *)
 */
static void loadView(ViewController *self) {

	super(ViewController, self, loadView);

	const SDL_Rect frame = { .x = 50, .y = 50, .w = 420, .h = 240 };
	Panel *panel = $(alloc(Panel), initWithFrame, &frame);

	panel->view.autoresizingMask = ViewAutoresizingContain;

	$(self->view, addSubview, (View *) panel);
	
	StackView *stackView = $(alloc(StackView), initWithFrame, NULL);
	stackView->view.autoresizingMask = ViewAutoresizingContain;
	stackView->spacing = 10;
	
	$((View *) panel, addSubview, (View *) stackView);
	
	Button *button = $(alloc(Button), initWithFrame, NULL, ControlStyleDefault);
	$(button->title, setText, "This is a bigass super long button");
	$((Control *) button, addActionForEventType, SDL_MOUSEBUTTONUP, buttonAction, NULL);
	
	$((View *) stackView, addSubview, (View *) button);

	TextView *textView = $(alloc(TextView), initWithFrame, NULL, ControlStyleDefault);
	textView->defaultText = "This is a textview";
	
	$((View *) stackView, addSubview, (View *) textView);

	Control *checkbox = (Control *) $(alloc(Checkbox), initWithFrame, NULL, ControlStyleDefault);
	Label *checkboxLabel = $(alloc(Label), initWithText, "This is a checkbox:", NULL);
	Input *checkboxInput = $(alloc(Input), initWithOrientation, InputOrientationLeft, checkbox, checkboxLabel);
	$(checkbox, addActionForEventType, SDL_MOUSEBUTTONUP, checkboxAction, NULL);

	$((View *) stackView, addSubview, (View *) checkboxInput);

	Select *select = $(alloc(Select), initWithFrame, NULL, ControlStyleDefault);
	$(select, addOption, "This is a select", NULL, (ident) 1);
	$(select, addOption, "This is an option", NULL, (ident) 2);
	$(select, addOption, "This is another", NULL, (ident) 3);
	$((View *) select, sizeToFit);
	
	$((View *) stackView, addSubview, (View *) select);

	Slider *slider = $(alloc(Slider), initWithFrame, NULL, ControlStyleDefault);
	Label *sliderLabel = $(alloc(Label), initWithText, "", NULL);
	slider->delegate.data = sliderLabel;
	slider->delegate.didSetValue = sliderDidSetValue;
	slider->min = 0.1, slider->max = 10.0, slider->step = 0.1;
	$(slider, setValue, 5.0);

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
	release(panel);
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
