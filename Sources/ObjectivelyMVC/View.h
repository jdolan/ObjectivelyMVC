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

#include <Objectively/Data.h>
#include <Objectively/Enum.h>
#include <Objectively/Dictionary.h>
#include <Objectively/MutableArray.h>

#include <ObjectivelyMVC/Colors.h>
#include <ObjectivelyMVC/Renderer.h>
#include <ObjectivelyMVC/View+JSON.h>

/**
 * @file
 * @brief Views are the fundamental building blocks of ObjectivelyMVC user interfaces.
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

OBJECTIVELYMVC_EXPORT const EnumName ViewAlignmentNames[];

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

OBJECTIVELYMVC_EXPORT const EnumName ViewAutoresizingNames[];

/**
 * @brief Spacing applied to the inside of a View's frame.
 */
typedef struct {
	int top, right, bottom, left;
} ViewPadding;

/**
 * @brief Relative positioning of subviews within their superview.
 */
typedef enum {
	ViewPositionBefore = -1,
	ViewPositionAfter = 1
} ViewPosition;

typedef struct ViewInterface ViewInterface;

/**
 * @brief Views are the fundamental building blocks of ObjectivelyMVC user interfaces.
 * @extends Object
 * @details Views provide organization and coordination of layout, drawing and event handling.
 * Views maintain hierarchical relationships among other Views, whereby each View's parent, or 
 * `superview`, dictates where it will be drawn on the screen, which events it will be eligible to 
 * receive, etc.
 */
struct View {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
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
	 * @brief If true, subviews will be clipped to this View's frame.
	 */
	_Bool clipsSubviews;

	/**
	 * @brief The frame, relative to the superview.
	 */
	SDL_Rect frame;

	/**
	 * @brief If `true`, this View is not drawn.
	 */
	_Bool hidden;

	/**
	 * @brief An optional identifier.
	 * @remarks Identifiers are commonly used to resolve Outlets when loading Views via JSON.
	 */
	char *identifier;

	/**
	 * @brief If true,
	 */
	_Bool needsLayout;

	/**
	 * @brief The padding.
	 */
	ViewPadding padding;

	/**
	 * @brief All contained views.
	 */
	MutableArray *subviews;

	/**
	 * @brief The containing View.
	 */
	View *superview;

	/**
	 * @brief The z-index.
	 * @remarks
	 */
	int zIndex;
};

/**
 * @brief The View interface.
 */
struct ViewInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface parentInterface;

	/**
	 * @fn void View::addSubview(View *self, View *subview)
	 * @brief Adds a subview to this view, to be drawn above its siblings.
	 * @param self The View.
	 * @param subview The subview to add.
	 * @remarks This method is equivalent to `$(view, addSubviewRelativeTo, subview, NULL, OrderSame)`.
	 * @memberof View
	 */
	void (*addSubview)(View *self, View *subview);

	/**
	 * @fn void View::addSubviewRelativeTo(View *self, View *subview, View *other, ViewPosition position)
	 * @brief Adds a subview to this view, positioned relatively to `other`.
	 * @param self The View.
	 * @param subview The subview to add.
	 * @param other An optional View to position `subview` relative to.
	 * @param position The relative position.
	 * @memberof View
	 */
	void (*addSubviewRelativeTo)(View *self, View *subview, View *other, ViewPosition position);

	/**
	 * @fn void View::awakeWithDictionary(View *self, const Dictionary *dictionary)
	 * @brief Wakes this View with the specified Dictionary.
	 * @param self The View.
	 * @param dictionary A Dictionary of properties describing this View.
	 * @remarks This method is invoked when loading via View. Subclasses should override this method
	 * to perform any customization based on the contents of `dictionary`.
	 * @memberof View
	 */
	void (*awakeWithDictionary)(View *self, const Dictionary *dictionary);

	/**
	 * @fn void View::becomeFirstResponder(View *self)
	 * @brief Become the first responder in the View hierarchy.
	 * @param self The View.
	 * @remarks Becoming the first responder gives a View priority when handling
	 * events.
	 * @memberof View
	 */
	void (*becomeFirstResponder)(View *self);

	/**
	 * @fn void View::bind(View *self, const Dictionary *dictionary, const Inlet *inlets)
	 * @brief Performs data binding for the Inlets described in `dictionary`.
	 * @param self The View.
	 * @param dictionary A Dictionary describing this View.
	 * @param inlets The Inlets to bind.
	 * @remarks Subclasses will typically call this method from View::awakeWithDictionary.
	 * @memberof View
	 */
	void (*bind)(View *self, const Dictionary *dictionary, const Inlet *inlets);

	/**
	 * @fn SDL_Rect View::bounds(const View *self)
	 * @param self The View.
	 * @return The bounds (frame minus padding) of this View.
	 * @memberof View
	 */
	SDL_Rect (*bounds)(const View *self);

	/**
	 * @fn SDL_Rect View::clippingFrame(const View *self)
	 * @param self The View.
	 * @return The visible portion of this View's frame, in window coordinates.
	 * @remarks This is equivalent to the View's `renderFrame`, expanded for border width, and
	 * clipped to all ancestors.
	 * @memberof View
	 */
	SDL_Rect (*clippingFrame)(const View *self);

	/**
	 * @fn _Bool View::canBecomeFirstResponder(const View *self)
	 * @param self The View.
	 * @return True if this View can become the first responder, false otherwise.
	 * @memberof View
	 */
	_Bool (*canBecomeFirstResponder)(const View *self);

	/**
	 * @fn _Bool View::containsPoint(const View *self, const SDL_Point *point)
	 * @param self The View.
	 * @param point A point in object space.
	 * @return True if the point falls within this View's clipped frame.
	 * @memberof View
	 */
	_Bool (*containsPoint)(const View *self, const SDL_Point *point);

	/**
	 * @fn int View::depth(const View *self)
	 * @param self The View.
	 * @return The depth of this View (`ancestor depth + zIndex + 1`).
	 * @remarks This method is called by Renderer::render to sort Views and enforce draw order.
	 * @memberof View
	 */
	int (*depth)(const View *self);

	/**
	 * @fn _Bool View::didReceiveEvent(const View *self, const SDL_Event *event)
	 * @param self The View.
	 * @param event The event.
	 * @return True if this View received the event, false otherwise.
	 * @memberof View
	 */
	_Bool (*didReceiveEvent)(const View *self, const SDL_Event *event);

	/**
	 * @fn void View::draw(View *self, Renderer *renderer)
	 * @brief Draws this View.
	 * @param self The View.
	 * @param renderer The Renderer with which to draw.
	 * @remarks The default implementation of this method adds the View to the Renderer for the
	 * current frame, and recurses its subviews. Rasterization is performed in View::render.
	 * @see View::render(View *, Renderer *)
	 * @memberof View
	 */
	void (*draw)(View *self, Renderer *renderer);

	/**
	 * @static
	 * @fn View *View::firstResponder(void)
	 * @return The first responder, or `NULL`.
	 * @memberof View
	 */
	View *(*firstResponder)(void);

	/**
	 * @fn View *View::init(View *self)
	 * @brief Initializes this View.
	 * @param self The View.
	 * @return The initialized View, or `NULL` on error.
	 * @remarks View::viewWithDictionary invokes this initializer when loading Views. Subclasses
	 * wishing to support JSON binding _must_ override this method to call their designated
	 * initializer.
	 * @memberof View
	 */
	View *(*init)(View *self);

	/**
	 * @fn View *View::initWithFrame(View *self, const SDL_Rect *frame)
	 * @brief Initializes this View with the specified frame.
	 * @param self The View.
	 * @param frame The frame.
	 * @return The initialized View, or `NULL` on error.
	 * @remarks Designated initializer.
	 * @memberof View
	 */
	View *(*initWithFrame)(View *self, const SDL_Rect *frame);

	/**
	 * @fn _Bool View::isDescendantOfView(const View *self, const View *view)
	 * @param self The View.
	 * @param view The View to test against this View's hierarchy.
	 * @return True if this View is a descendant of, or equal to, the given View.
	 * @memberof View
	 */
	_Bool (*isDescendantOfView)(const View *self, const View *view);

	/**
	 * @fn _Bool View::isFirstResponder(const View *self)
	 * @param self The View.
	 * @return True if this View is the first responder, false otherwise.
	 * @memberof View
	 */
	_Bool (*isFirstResponder)(const View *self);

	/**
	 * @fn _Bool View::isVisible(const View *self)
	 * @param self The View.
	 * @return True if this View is visible, false if it, or an ancestor, is hidden.
	 * @memberof View
	 */
	_Bool (*isVisible)(const View *self);

	/**
	 * @fn void View::layoutIfNeeded(View *self)
	 * @brief Recursively updates the layout of this View and its subviews.
	 * @param self The View.
	 * @memberof View
	 */
	void (*layoutIfNeeded)(View *self);

	/**
	 * @fn View::layoutSubviews(View *self)
	 * @brief Updates the frame of this View's hierarchy using the installed Constraints.
	 * @param self The View.
	 * @memberof View
	 */
	void (*layoutSubviews)(View *self);

	/**
	 * @fn void View::removeFromSuperview(View *self)
	 * @brief Removes this View from its superview.
	 * @param self The View.
	 * @memberof View
	 */
	void (*removeFromSuperview)(View *self);

	/**
	 * @fn void View::removeSubview(View *self, View *subview)
	 * @brief Removes the given subview from this View.
	 * @param self The View.
	 * @param subview The subview.
	 * @memberof View
	 */
	void (*removeSubview)(View *self, View *subview);

	/**
	 * @fn void View::render(View *self, Renderer *renderer)
	 * @brief Renders this View using the given renderer.
	 * @param self The View.
	 * @param renderer The Renderer.
	 * @remarks Subclasses should override this method to perform actual drawing operations. This
	 * method is called by `draw` as the View hierarchy is drawn.
	 * @memberof View
	 */
	void (*render)(View *self, Renderer *renderer);

	/**
	 * @fn void View::renderDeviceDidReset(View *self)
	 * @brief This method is invoked when the render context is invalidated.
	 * @param self The View.
	 * @remarks Subclasses should override this method to recreate any texture resources or other
	 * OpenGL objects they own.
	 * @memberof View
	 */
	void (*renderDeviceDidReset)(View *self);

	/**
	 * @fn SDL_Frame View::renderFrame(const View *self)
	 * @param self The View.
	 * @return This View's absolute frame in the View hierarchy, in object space.
	 * @memberof View
	 */
	SDL_Rect (*renderFrame)(const View *self);

	/**
	 * @fn void View::resignFirstResponder(View *self)
	 * @brief Resigns first responder priority.
	 * @param self The View.
	 * @memberof View
	 */
	void (*resignFirstResponder)(View *self);

	/**
	 * @fn void View::resize(View *self, const SDL_Size *size)
	 * @brief Resizes this View to the specified size.
	 * @param self The View.
	 * @param size The size to set.
	 * @memberof View
	 */
	void (*resize)(View *self, const SDL_Size *size);

	/**
	 * @fn void View::respondToEvent(View *self, const SDL_Event *event)
	 * @brief Responds to the specified event.
	 * @param self The View.
	 * @param event The SDL_Event.
	 * @memberof View
	 */
	void (*respondToEvent)(View *self, const SDL_Event *event);

	/**
	 * @fn SDL_Size View::size(const View *self)
	 * @param self The View.
	 * @return The size of this View's frame.
	 * @memberof View
	 */
	SDL_Size (*size)(const View *self);

	/**
	 * @fn SDL_Size View::sizeThatContains(const View *self)
	 * @param self The View.
	 * @return An SDL_Size that contains this View's subviews.
	 * @remarks The returned size is greater than or equal to View::sizeThatFits.
	 * @memberof View
	 */
	SDL_Size (*sizeThatContains)(const View *self);

	/**
	 * @fn SDL_Size View::sizeThatFits(const View *self)
	 * @param self The View.
	 * @return An SDL_Size that fits this View's subviews.
	 * @remarks The default implementation of this method returns the View's current size, or an
	 * appropriate value based on this View's `autoresizingMask`.
	 * @memberof View
	 */
	SDL_Size (*sizeThatFits)(const View *self);

	/**
	 * @fn void View::sizeToContain(View *self)
	 * @brief Resizes this View to contain its subviews.
	 * @param self The View.
	 * @see View::sizeThatContains(const View *)
	 * @memberof View
	 */
	void (*sizeToContain)(View *self);

	/**
	 * @fn void View::sizeToFit(View *self)
	 * @brief Resizes this View to fit its subviews.
	 * @param self The View.
	 * @see View::sizeThatFits(const View *)
	 * @memberof View
	 */
	void (*sizeToFit)(View *self);

	/**
	 * @fn void View::updateBindings(View *self)
	 * @brief Updates data bindings, prompting the appropriate layout changes.
	 * @param self The View.
	 * @remarks Subclasses should override this method to refresh any data sources they rely on.
	 * @memberof View
	 */
	void (*updateBindings)(View *self);

	/**
	 * @fn SDL_Rect View::viewport(const View *self)
	 * @param self The View.
	 * @return The OpenGL viewport for this View.
	 * @memberof View
	 */
	SDL_Rect (*viewport)(const View *self);

	/**
	 * @static
	 * @fn View *View::viewWithContentsOfFile(const char *path, Outlet *outlets)
	 * @brief Instantiates a View initialized with the contents of the JSON file at `path`.
	 * @param path A path to a JSON file describing a View.
	 * @param outlets An optional array of Outlets to resolve.
	 * @return The initialized View, or `NULL` on error.
	 * @memberof View
	 */
	View *(*viewWithContentsOfFile)(const char *path, Outlet *outlets);

	/**
	 * @static
	 * @fn View *View::viewWithData(const Data *data, Outlet *outlets)
	 * @brief Instantiates a View initialized with the contents of `data`.
	 * @param data A Data containing JSON describing a View.
	 * @param outlets An optional array of Outlets to resolve.
	 * @return The initialized View, or `NULL` on error.
	 * @memberof View
	 */
	View *(*viewWithData)(const Data *data, Outlet *outlets);

	/**
	 * @static
	 * @fn View *View::viewWithDictionary(const Dictionary *dictionary, Outlet *outlets)
	 * @brief Instantiates a View initialized with the attributes described in `dictionary`.
	 * @param dictionary A Dictionary describing a View.
	 * @param outlets An optional array of Outlets to resolve.
	 * @return The initialized View, or `NULL` on error.
	 * @see View::initWithDictionary(View *, const Dictionary *, Outlet *outlets)
	 * @memberof View
	 */
	View *(*viewWithDictionary)(const Dictionary *dictionary, Outlet *outlets);

	/**
	 * @fn Array *View::visibleSubviews(const View *self)
	 * @param self The View.
	 * @return An Array of this View's visible subviews.
	 * @remarks The returned Array excludes that specify ViewAlignmentInternal.
	 * @memberof View
	 */
	Array *(*visibleSubviews)(const View *self);

	/**
	 * @fn SDL_Window *View::window(const View *self)
	 * @param self The View.
	 * @return The window containing this View.
	 * @memberof View
	 */
	SDL_Window *(*window)(const View *self);
};

OBJECTIVELYMVC_EXPORT Class *_View(void);

/**
 * @brief Transforms the specified rectangle to normalized device coordinates in `window`.
 * @param window The window.
 * @param rect A rectangle defined in object space.
 * @return The transformed rectangle.
 */
OBJECTIVELYMVC_EXPORT SDL_Rect MVC_TransformToWindow(SDL_Window *window, const SDL_Rect *rect);

/**
 * @brief Resolves the scale factor of the specified window for High-DPI support.
 * @param window The window, or `NULL` for the current OpenGL window.
 * @param height An optional output parameter to retrieve the window height.
 * @param drawableHeight AN optional output parameter to retrieve the window drawable height.
 * @return The scale factor of the specified window.
 * @remarks Views and other classes should invoke this method to alter their rendering behavior for
 * High-DPI displays. This is particularly relevant for Views that render textures.
 */
OBJECTIVELYMVC_EXPORT double MVC_WindowScale(SDL_Window *window, int *height, int *drawableHeight);

