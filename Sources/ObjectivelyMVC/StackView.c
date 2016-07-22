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

#include <ObjectivelyMVC/StackView.h>

#define _Class _StackView

#pragma mark - View

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {
	
	super(View, self, layoutSubviews);

	StackView *this = (StackView *) self;
	
	const SDL_Rect bounds = $(self, bounds);
	
	int availableSize, requestedSize = 0;
	switch (this->axis) {
		case StackViewAxisVertical:
			availableSize = bounds.h;
			break;
		case StackViewAxisHorizontal:
			availableSize = bounds.w;
			break;
	}
	
	Array *subviews = $(self, visibleSubviews);
	availableSize -= this->spacing * subviews->count - 1;
	
	for (size_t i = 0; i < subviews->count; i++) {
		
		const View *subview = $(subviews, objectAtIndex, i);
		
		int sw, sh;
		$(subview, sizeThatFits, &sw, &sh);
		
		switch (this->axis) {
			case StackViewAxisVertical:
				requestedSize += sh;
				break;
			case StackViewAxisHorizontal:
				requestedSize += sw;
				break;
		}
	}
	
	int pos;
	switch (this->axis) {
		case StackViewAxisVertical:
			pos = bounds.y;
			break;
		case StackViewAxisHorizontal:
			pos = bounds.x;
			break;
	}
	
	const float scale = requestedSize ? availableSize / (float) requestedSize : 1.0;
	
	for (size_t i = 0; i < subviews->count; i++) {
		
		View *subview = $(subviews, objectAtIndex, i);
		
		switch (this->axis) {
			case StackViewAxisVertical:
				subview->frame.y = pos;
				break;
			case StackViewAxisHorizontal:
				subview->frame.x = pos;
				break;
		}
		
		switch (this->distribution) {
			case StackViewDistributionDefault:
				break;
			
			case StackViewDistributionFill:
				switch (this->axis) {
					case StackViewAxisVertical:
						subview->frame.h *= scale;
						break;
					case StackViewAxisHorizontal:
						subview->frame.w *= scale;
						break;
				}
				break;
			
			case StackViewDistributionFillEqually:
				switch (this->axis) {
					case StackViewAxisVertical:
						subview->frame.h = availableSize / (float) subviews->count;
						break;
					case StackViewAxisHorizontal:
						subview->frame.w = availableSize / (float) subviews->count;
						break;
				}
				break;
		}
		
		switch (this->axis) {
			case StackViewAxisVertical:
				pos += subview->frame.h;
				break;
			case StackViewAxisHorizontal:
				pos += subview->frame.w;
				break;
		}
		
		pos += this->spacing;
	}
	
	release(subviews);
}

/**
 * @see View::sizeThatFits(const View *, int *, int *)
 */
static void sizeThatFits(const View *self, int *w, int *h) {
	
	const StackView *this = (StackView *) self;
	
	*w = *h = 0;
	
	Array *subviews = $(self, visibleSubviews);
	if (subviews->count) {
		
		for (size_t i = 0; i < subviews->count; i++) {
			
			const View *subview = $(subviews, objectAtIndex, i);
			
			int sw, sh;
			$(subview, sizeThatFits, &sw, &sh);
			
			switch (this->axis) {
				case StackViewAxisVertical:
					*w = max(*w, sw);
					*h += sh;
					break;
				case StackViewAxisHorizontal:
					*h = max(*h, sh);
					*w += sw;
					break;
			}
		}
		
		switch (this->axis) {
			case StackViewAxisVertical:
				*h += this->spacing * subviews->count - 1;
				break;
			case StackViewAxisHorizontal:
				*w += this->spacing * subviews->count - 1;
				break;
		}
	}

	*w += self->padding.left + self->padding.right;
	*h += self->padding.top + self->padding.bottom;

	release(subviews);
}

#pragma mark - StackView

/**
 * @fn StackView *StackView::initWithFrame(StackView *self, const SDL_Rect *frame)
 *
 * @memberof StackView
 */
static StackView *initWithFrame(StackView *self, const SDL_Rect *frame) {

	return (StackView *) super(View, self, initWithFrame, frame);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
	((ViewInterface *) clazz->interface)->sizeThatFits = sizeThatFits;
	
	((StackViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

Class _StackView = {
	.name = "StackView",
	.superclass = &_View,
	.instanceSize = sizeof(StackView),
	.interfaceOffset = offsetof(StackView, interface),
	.interfaceSize = sizeof(StackViewInterface),
	.initialize = initialize,
};

#undef _Class
