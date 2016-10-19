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

#include <ObjectivelyMVC/Image.h>
#include <ObjectivelyMVC/Log.h>
#include <ObjectivelyMVC/Renderer.h>
#include <ObjectivelyMVC/View.h>

#define _Class _Renderer

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Renderer *this = (Renderer *) self;

	release(this->views);

	super(Object, self, dealloc);
}

#pragma mark - Renderer

/**
 * @fn void Renderer::addView(Renderer *self, View *view)
 * @memberof Renderer
 */
static void addView(Renderer *self, View *view) {
	$(self->views, addObject, view);
}

/**
 * @fn void Renderer::beginFrame(Renderer *self)
 * @memberof Renderer
 */
static void beginFrame(Renderer *self) {

	$(self, setDrawColor, &Colors.White);
}

/**
 * @fn void Renderer::createTexture(const Renderer *self, const SDL_Surface *surface)
 * @memberof Renderer
 */
static ident createTexture(const Renderer *self, const SDL_Surface *surface) {

	assert(surface);

	return NULL;
}

/**
 * @fn void Renderer::destroyTexture(const Renderer *self, ident texture)
 * @memberof Renderer
 */
static void destroyTexture(const Renderer *self, ident texture) {

}

/**
 * @fn void Renderer::drawLine(const Renderer *self, const SDL_Point *points)
 * @memberof Renderer
 */
static void drawLine(const Renderer *self, const SDL_Point *points) {

	assert(points);

	$(self, drawLines, points, 2);
}

/**
 * @fn void Renderer::drawLines(const Renderer *self, const SDL_Point *points, size_t count)
 * @memberof Renderer
 */
static void drawLines(const Renderer *self, const SDL_Point *points, size_t count) {

	assert(points);
}

/**
 * @fn void Renderer::drawRect(const Renderer *self, const SDL_Rect *rect)
 * @memberof Renderer
 */
static void drawRect(const Renderer *self, const SDL_Rect *rect) {

	assert(rect);
}

/**
 * @fn void Renderer::drawRectFilled(const Renderer *self, const SDL_Rect *rect)
 * @memberof Renderer
 */
static void drawRectFilled(const Renderer *self, const SDL_Rect *rect) {

	assert(rect);
}

/**
 * @fn void Renderer::drawTexture(const Renderer *self, ident texture, const SDL_Rect *dest)
 * @memberof Renderer
 */
static void drawTexture(const Renderer *self, ident texture, const SDL_Rect *rect) {

	assert(rect);
}

/**
 * @fn void Renderer::endFrame(Renderer *self)
 * @memberof Renderer
 */
static void endFrame(Renderer *self) {

}

/**
 * @fn Renderer *Renderer::init(Renderer *self)
 * @memberof Renderer
 */
static Renderer *init(Renderer *self) {
	
	self = (Renderer *) super(Object, self, init);
	if (self) {

		self->views = $$(MutableArray, array);
		assert(self->views);

		$(self, renderDeviceDidReset);
	}

	return self;
}

/**
 * @brief Comparator for sorting Views by depth (Painter's Algorithm).
 */
static Order render_sort(const ident a, const ident b) {

	const int depthA = $((const View *) a, depth);
	const int depthB = $((const View *) b, depth);

	return (Order) depthA - depthB;
}

/**
 * @brief ArrayEnumerator for rendering Views.
 */
static void render_renderView(const Array *array, ident obj, ident data) {

	View *view = (View *) obj;
	Renderer *renderer = (Renderer *) data;

	const SDL_Rect frame = $(view, clippingFrame);
	if (frame.w && frame.h) {

		SDL_Rect scissor = MVC_TransformToWindow($(view, window), &frame);
		scissor.x--;
		scissor.y--;
		scissor.w++;
		scissor.h++;

		$(renderer, setScissor, &scissor);

		$(view, render, renderer);
	}
}

/**
 * @fn void Renderer::drawViews(Renderer *self)
 * @memberof Renderer
 */
static void render(Renderer *self) {

	$(self->views, sort, render_sort);

	$((Array *) self->views, enumerateObjects, render_renderView, self);

	$(self->views, removeAllObjects);
}

/**
 * @fn void Renderer::renderDeviceDidReset(Renderer *self)
 * @memberof Renderer
 */
static void renderDeviceDidReset(Renderer *self) {

}

/**
 * @fn void Renderer::setDrawColor(Renderer *self, const SDL_Color *color)
 * @memberof Renderer
 */
static void setDrawColor(Renderer *self, const SDL_Color *color) {

}

/**
 * @fn void Renderer::setScissor(Renderer *self, const SDL_Rect *rect)
 * @memberof Renderer
 */
static void setScissor(Renderer *self, const SDL_Rect *rect) {

}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((RendererInterface *) clazz->def->interface)->addView = addView;
	((RendererInterface *) clazz->def->interface)->beginFrame = beginFrame;
	((RendererInterface *) clazz->def->interface)->createTexture = createTexture;
	((RendererInterface *) clazz->def->interface)->destroyTexture = destroyTexture;
	((RendererInterface *) clazz->def->interface)->drawLine = drawLine;
	((RendererInterface *) clazz->def->interface)->drawLines = drawLines;
	((RendererInterface *) clazz->def->interface)->drawRect = drawRect;
	((RendererInterface *) clazz->def->interface)->drawRectFilled = drawRectFilled;
	((RendererInterface *) clazz->def->interface)->drawTexture = drawTexture;
	((RendererInterface *) clazz->def->interface)->endFrame = endFrame;
	((RendererInterface *) clazz->def->interface)->init = init;
	((RendererInterface *) clazz->def->interface)->render = render;
	((RendererInterface *) clazz->def->interface)->renderDeviceDidReset = renderDeviceDidReset;
	((RendererInterface *) clazz->def->interface)->setDrawColor = setDrawColor;
	((RendererInterface *) clazz->def->interface)->setScissor = setScissor;
}

Class _Renderer = {
	.name = "Renderer",
	.superclass = &_Object,
	.instanceSize = sizeof(Renderer),
	.interfaceOffset = offsetof(Renderer, interface),
	.interfaceSize = sizeof(RendererInterface),
	.initialize = initialize
};

#undef _Class

