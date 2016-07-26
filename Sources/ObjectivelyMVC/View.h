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

#pragma once

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/Colors.h>
#include <ObjectivelyMVC/Types.h>

/**
 * @file
 *
 * @brief View implementation.
 */

#define ViewAlignmentMaskTop      0x1
#define ViewAlignmentMaskMiddle   0x2
#define ViewAlignmentMaskBottom   0x4
#define ViewAlignmentMaskLeft     0x8
#define ViewAlignmentMaskCenter   0x10
#define ViewAlignmentMaskRight    0x20
#define ViewAlignmentMaskInternal 0x100

/**
 * @brief Alignment constants, used to align a View within its superview.
 */
typedef enum {
	ViewAlignmentNone,
	ViewAlignmentTopLeft = (ViewAlignmentMaskTop | ViewAlignmentMaskLeft),
	ViewAlignmentTopCenter = (ViewAlignmentMaskTop | ViewAlignmentMaskCenter),
	ViewAlignmentTopRight = (ViewAlignmentMaskTop | ViewAlignmentMaskRight),
	ViewAlignmentMiddleLeft = (ViewAlignmentMaskMiddle | ViewAlignmentMaskLeft),
	ViewAlignmentMiddleCenter = (ViewAlignmentMaskMiddle | ViewAlignmentMaskCenter),
	ViewAlignmentMiddleRight = (ViewAlignmentMaskMiddle | ViewAlignmentMaskRight),
	ViewAlignmentBottomLeft = (ViewAlignmentMaskBottom | ViewAlignmentMaskLeft),
	ViewAlignmentBottomCenter = (ViewAlignmentMaskBottom | ViewAlignmentMaskCenter),
	ViewAlignmentBottomRight = (ViewAlignmentMaskBottom | ViewAlignmentMaskRight),
	ViewAlignmentInternal = ViewAlignmentMaskInternal
} ViewAlignment;

/**
 * @brief Auto-resizing constants, which are bitmasked.
 */
typedef enum {
	ViewAutoresizingNone = 0,
	ViewAutoresizingWidth = 0x1,
	ViewAutoresizingHeight = 0x2,
	ViewAutoresizingFill = 0x3,
	ViewAutoresizingContain = 0x4
} ViewAutoresizing;

/**
 * @brief Padding: like bounds, but easier.
 */
typedef struct {
	int top, right, bottom, left;
} Padding;

typedef struct View View;
typedef struct ViewInterface ViewInterface;

/**
 * @brief The View type.
 *
 * @extends Object
 */
struct View {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	ViewInterface *interface;
	
	/**
	 * @brief The alignment.
	 */
	ViewAlignment alignment;
	
	/**
	 * @brief The ViewAutoresizing bitmask.
	 */
	int autoresizingMask;
	
	/**
	 * @brief The background color.
	 */
	SDL_Color backgroundColor;
	
	/**
	 * @brief The border color.
	 */
	SDL_Color borderColor;
	
	/**
	 * @brief The border width.
	 */
	int borderWidth;
	
	/**
	 * @brief The frame, relative to the superview.
	 */
	SDL_Rect frame;
	
	/**
	 * @brief If `true`, this View is not drawn.
	 */
	_Bool hidden;
	
	/**
	 * @brief If true,
	 */
	_Bool needsLayout;
	
	/**
	 * @brief The padding.
	 */
	Padding padding;
	
	/**
	 * @brief All contained views.
	 */
	MutableArray *subviews;

	/**
	 * @brief The containing View.
	 */
	View *superview;
};

/**
 * @brief The View interface.
 */
struct ViewInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface parentInterface;
	
	/**
	 * @fn void View::addSubview(View *self, View *subview)
	 *
	 * @brief Adds a subview to this view, to be drawn above its siblings.
	 *
	 * @param subview The subview.
	 *
	 * @memberof View
	 */
	void (*addSubview)(View *self, View *subview);
	
	/**
	 * @fn SDL_Rect View::bounds(const View *self)
	 *
	 * @return The bounds (frame minus padding) of this View.
	 *
	 * @memberof View
	 */
	SDL_Rect (*bounds)(const View *self);

	/**
	 * @fn _Bool View::containsPoint(const View *self, const SDL_Point *point)
	 *
	 * @param point A point in window space.
	 *
	 * @return True if the point falls within this Views frame.
	 *
	 * @memberof View
	 */
	_Bool (*containsPoint)(const View *self, const SDL_Point *point);
	
	/**
	 * @fn _Bool View::didReceiveEvent(const View *self, const SDL_Event *event)
	 *
	 * @param event The event.
	 *
	 * @return True if this View received the event, false otherwise.
	 *
	 * @memberof View
	 */
	_Bool (*didReceiveEvent)(const View *self, const SDL_Event *event);
	
	/**
	 * @fn void View::draw(View *self, SDL_Renderer *renderer)
	 *
	 * @brief Draws this View.
	 *
	 * @param renderer The SDL_Renderer with which to draw.
	 *
	 * @remarks The ViewController will call this method when drawing its View hierarchy.
	 *
	 * @memberof View
	 */
	void (*draw)(View *self, SDL_Renderer *renderer);

	/**
	 * @fn View *View::initWithFrame(View *self, const SDL_Rect *frame)
	 *
	 * @brief Initializes this View with the specified frame.
	 *
	 * @param frame The frame.
	 *
	 * @return The initialized View, or NULL on error.
	 *
	 * @memberof View
	 */
	View *(*initWithFrame)(View *self, const SDL_Rect *frame);
	
	/**
	 * @fn _Bool View::isDescendantOfView(const View *self, const View *view)
	 *
	 * @param view The View to test against this View's hierarchy.
	 *
	 * @return True if this View is a descendant of, or equal to, the given View.
	 *
	 * @memberof View
	 */
	_Bool (*isDescendantOfView)(const View *self, const View *view);

	/**
	 * @fn _Bool View::isVisible(const View *self)
	 *
	 * @return True if this View is visible, false if it, or an ancestor, is hidden.
	 *
	 * @memberof View
	 */
	_Bool (*isVisible)(const View *self);

	/**
	 * @fn void View::layoutIfNeeded(View *self)
	 *
	 * @brief Recursively updates the layout of this View and its subviews.
	 *
	 * @memberof View
	 */
	void (*layoutIfNeeded)(View *self);
	
	/**
	 * @fn View::layoutSubviews(View *self)
	 *
	 * @brief Updates the frame of this View's hierarchy using the installed Constraints.
	 *
	 * @memberof View
	 */
	void (*layoutSubviews)(View *self);
	
	/**
	 * @fn void View::removeFromSuperview(View *self)
	 *
	 * @brief Removes this View from its superview.
	 *
	 * @memberof View
	 */
	void (*removeFromSuperview)(View *self);

	/**
	 * @fn void View::removeSubview(View *self, View *subview)
	 *
	 * @brief Removes the given subview from this View.
	 *
	 * @param subview The subview.
	 *
	 * @memberof View
	 */
	void (*removeSubview)(View *self, View *subview);
	
	/**
	 * @fn void View::render(View *self, SDL_Renderer *renderer)
	 *
	 * @brief Renders this View using the given renderer.
	 *
	 * @remarks Subclasses should override this method to perform actual drawing operations.
	 * This method is called by `draw` as the View hierarchy is drawn.
	 *
	 * @memberof View
	 */
	void (*render)(View *self, SDL_Renderer *renderer);

	/**
	 * @fn void View::renderDeviceDidReset(View *self)
	 *
	 * @brief This method is invoked from `respondToEvent` for `SDL_RENDER_DEVICE_RESET`.
	 *
	 * @remarks Subclasses should override this method to recreate any texture resources
	 * or other OpenGL objects they own.
	 *
	 * @memberof View
	 */
	void (*renderDeviceDidReset)(View *self);

	/**
	 * @fn SDL_Frame View::renderFrame(const View *self)
	 *
	 * @return This View's absolute frame in the View hierarchy.
	 *
	 * @memberof View
	 */
	SDL_Rect (*renderFrame)(const View *self);
	
	/**
	 * @fn void View::respondToEvent(View *self, const SDL_Event *event)
	 *
	 * @brief Respond to an event.
	 *
	 * @param event The SDL_Event.
	 *
	 * @memberof View
	 */
	void (*respondToEvent)(View *self, const SDL_Event *event);
	
	/**
	 * @fn void View::sizeThatFits(const View *self, int *w, int *h)
	 *
	 * @param w The width return value.
	 * @param h The height return value.
	 *
	 * @return The minimum size that fits this View with margin.
	 *
	 * @memberof View
	 */
	void (*sizeThatFits)(const View *self, int *w, int *h);
	
	/**
	 * @fn void View::sizeToFit(View *self)
	 *
	 * @brief Resizes this View to contain its subviews.
	 *
	 * @memberof View
	 */
	void (*sizeToFit)(View *self);

	/**
	 * @fn void View::updateBindings(View *self)
	 *
	 * @brief Updates data bindings, prompting the appropriate layout changes.
	 *
	 * @remarks Subclasses should override this method to update any data sources
	 * they rely on.
	 *
	 * @memberof View
	 */
	void (*updateBindings)(View *self);

	/**
	 * @fn Array *View::visibleSubviews(const View *self)
	 *
	 * @return An Array of this View's visible subviews.
	 *
	 * @memberof View
	 */
	Array *(*visibleSubviews)(const View *self);

	/**
	 * @fn SDL_Window *View::window(const View *self)
	 *
	 * @return The window containing this View.
	 *
	 * @memberof View
	 */
	SDL_Window *(*window)(const View *self);
};

extern Class _View;
