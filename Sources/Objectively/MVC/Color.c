/*
 * Color.h
 *
 *  Created on: Nov 19, 2014
 *      Author: jdolan
 */

#include <assert.h>

#include <Objectively/Once.h>

#include <Objectively/MVC/Color.h>

#define _Class _Color

#pragma mark - Color

static Color *_black;

/**
 * @fn Color *Color::black(void)
 *
 * @memberof Color
 */
static Color *black(void) {
	static Once once;
	
	DispatchOnce(once, {
		_black = $(alloc(Color), initWithComponents, 0, 0, 0, 255);
	});
	
	return _black;
}

static Color *_gray;

/**
 * @fn Color *Color::gray(void)
 *
 * @memberof Color
 */
static Color *gray(void) {
	static Once once;
	
	DispatchOnce(once, {
		_gray = $(alloc(Color), initWithComponents, 128, 128, 128, 255);
	});
	
	return _gray;
}

/**
 * @fn Color *Color::initWithComponents(Color *self, byte r, byte g, byte b, byte a)
 *
 * @memberof Color
 */
static Color *initWithComponents(Color *self, byte r, byte g, byte b, byte a) {
	
	self = (Color *) super(Object, self, init);
	if (self) {
		self->color.r = r;
		self->color.g = g;
		self->color.b = b;
		self->color.a = a;
	}
	
	return self;
}

static Color *_white;

/**
 * @fn Color *Color::white(void)
 *
 * @memberof Color
 */
static Color *white(void) {
	static Once once;
	
	DispatchOnce(once, {
		_white = $(alloc(Color), initWithComponents, 255, 255, 255, 255);
	});
	
	return _white;
}

static Color *_transparent;

/**
 * @fn Color *Color::transparent(void)
 *
 * @memberof Color
 */
static Color *transparent(void) {
	static Once once;
	
	DispatchOnce(once, {
		_transparent = $(alloc(Color), initWithComponents, 255, 255, 255, 0);
	});
	
	return _transparent;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	ColorInterface *color = (ColorInterface *) clazz->interface;
	
	color->black = black;
	color->gray = gray;
	color->initWithComponents = initWithComponents;
	color->transparent = transparent;
	color->white = white;
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	
	release(_black);
	release(_gray);
	release(_transparent);
	release(_white);
}

Class _Color = {
	.name = "Color",
	.superclass = &_Object,
	.instanceSize = sizeof(Color),
	.interfaceOffset = offsetof(Color, interface),
	.interfaceSize = sizeof(ColorInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class