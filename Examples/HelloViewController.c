/*
 * HelloViewController.c
 *
 *  Created on: Jan 19, 2016
 *      Author: jdolan
 */

#include <assert.h>

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
	
	const SDL_Rect frame = { .x = 100, .y = 100 };
	
	Font *font = $(alloc(Font), initWithName, "Helvetica Neue-16");
	
	Label *label = $(alloc(Label), initWithText, "Hello World!", font);
	$((View *) label, sizeToFit);
	
	Button *button = $(alloc(Button), initWithFrame, &frame);
	
	$(button->label, setText, "Click Me");
	$(button->label, setFont, font);
	
	$((View *) button, sizeToFit);
	
	$((Control *) button, addActionForEventType, SDL_MOUSEBUTTONUP, buttonAction, NULL);
	
	View *container = $(alloc(View), initWithFrame, &frame);
	container->backgroundColor = Colors.red;
	
	$(container, addSubview, (View *) label);
	$(container, addSubview, (View *) button);
	
	$(container, sizeToFit);
	
	$(self->view, addSubview, container);
	
	release(font);
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
