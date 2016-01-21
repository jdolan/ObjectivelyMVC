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

#ifndef _ObjectivelyMVC_View_h_
#define _ObjectivelyMVC_View_h_

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/Colors.h>
#include <ObjectivelyMVC/Constraint.h>

/**
 * @file
 *
 * @brief View implementation.
 */

/**
 * @brief The View type.
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
	 * @brief Constraints.
	 */
	MutableArray *constraints;
	
	/**
	 * @brief The frame, relative to the superview.
	 */
	SDL_Rect frame;
	
	/**
	 * @brief If `true`, this View is not drawn.
	 */
	_Bool hidden;
	
	/**
	 * @brief Margin.
	 */
	struct {
		int top, right, bottom, left;
	} margin;
	
	/**
	 * @brief If true, this View will be layed out before being draw.
	 */
	_Bool needsLayout;
	
	/**
	 * @brief Padding.
	 */
	struct {
		int top, right, bottom, left;
	} padding;
	
	/**
	 * @brief All contained views.
	 */
	MutableArray *subviews;

	/**
	 * @brief The containing View.
	 */
	View *superview;
	
	/**
	 * @brief Constraints targeting this View.
	 */
	MutableArray *targetConstraints;
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
	 * @fn Constraint *View::constrain(View *self, Attribute sourceAttribute, int min, int max)
	 *
	 * @brief Creates or updates a Constraint, limiting the specified Attribute.
	 *
	 * @return The resulting Constraint.
	 *
	 * @memberof View
	 */
	Constraint *(*constrain)(View *self, Attribute sourceAttribute, int min, int max);
	
	/**
	 * @fn Constraint *View::constrainTo(View *self, Attribute sourceAttribute, View *target, Attribute targetAttribute, int offset)
	 *
	 * @brief Creates or updates a Constraint, binding this View to the given target.
	 *
	 * @param sourceAttribute The source Attribute.
	 * @param target The target View.
	 * @param targetAttribute The target Attribute.
	 * @param offset The offset.
	 *
	 * @return The resulting Constraint.
	 *
	 * @memberof View
	 */
	Constraint *(*constrainTo)(View *self, Attribute sourceAttribute, View *target, Attribute targetAttribute, int offset);
	
	/**
	 * @fn Constraint *View::constraintForAttribute(View *self, Attribute sourceAttribute)
	 *
	 * @brief Retrieves or creates a Constraint for the given Attribute.
	 
	 * @param sourceAttribute The source Attribute.
	 *
	 * @return The Constraint for the given Attribute.
	 *
	 * @memberof View
	 */
	Constraint *(*constraintForAttribute)(View *self, Attribute sourceAttribute);
	
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
	 * @fn View::deleteConstraint(View *self, Attribute sourceAttribute)
	 *
	 * @brief Deletes the Constraint for the specified Attribute.
	 *
	 * @param sourceAttribute The Attribute.
	 *
	 * @memberof View
	 */
	void (*deleteConstraint)(View *self, Attribute sourceAttribute);
	
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
	 * @brief Initializes this view with the specified frame.
	 *
	 * @param frame The frame.
	 *
	 * @return The initialized view, or NULL on error.
	 *
	 * @memberof View
	 */
	View *(*initWithFrame)(View *self, const SDL_Rect *frame);
	
	/**
	 * @fn View::layoutSubviews(View *self)
	 *
	 * @brief Updates the frames for subviews which specify a AnchorAttribute.
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
	 * @fn SDL_Frame View::renderFrame(const View *self)
	 *
	 * @return This View's absolute frame in the View hierarchy.
	 *
	 * @memberof View
	 */
	SDL_Rect (*renderFrame)(const View *self);
	
	/**
	 * @fn _Bool View:respondToEvent(View *self, SDL_Event *event)
	 *
	 * @brief Responds to the given event.
	 *
	 * @param event The SDL_Event.
	 *
	 * @return True if the event was responded to, false otherwise.
	 *
	 * @memberof View
	 */
	_Bool (*respondToEvent)(View *self, const SDL_Event *event);
	
	/**
	 * @fn void View::sizeThatFits(const View *self, int *w, int *h)
	 *
	 * @memberof View
	 */
	void (*sizeThatFits)(const View *self, int *w, int *h);
	
	/**
	 * @fn void View::sizeToFit(View *self)
	 *
	 * @memberof View
	 */
	void (*sizeToFit)(View *self);
	
	/**
	 * @fn void View::updateConstraint(View *self, const Constraint *constraint)
	 *
	 * @brief Invalidates the layout of Views dependent upon the given Constraint.
	 *
	 * @param constraint The Constraint that was updated.
	 *
	 * @remarks Call this method after modifying a Constraint.
	 *
	 * @memberof View
	 */
	void (*updateConstraint)(View *self, const Constraint *constraint);
};

extern Class _View;

#endif
