/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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
#include <Objectively/MutableDictionary.h>
#include <Objectively/MutableArray.h>
#include <Objectively/MutableSet.h>
#include <Objectively/Resource.h>

#include <ObjectivelyMVC/Colors.h>
#include <ObjectivelyMVC/Renderer.h>
#include <ObjectivelyMVC/Theme.h>
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

#define ViewAlignmentMaskVertical \
	(ViewAlignmentMaskTop | ViewAlignmentMaskMiddle | ViewAlignmentMaskBottom)

#define ViewAlignmentMaskHorizontal \
	(ViewAlignmentMaskLeft | ViewAlignmentMaskCenter | ViewAlignmentMaskRight)

/**
 * @brief Alignment constants, used to align a View within its superview.
 */
typedef enum {
	ViewAlignmentNone,
	ViewAlignmentTop = ViewAlignmentMaskTop,
	ViewAlignmentMiddle = ViewAlignmentMaskMiddle,
	ViewAlignmentBottom = ViewAlignmentMaskBottom,
	ViewAlignmentLeft = ViewAlignmentMaskLeft,
	ViewAlignmentCenter = ViewAlignmentMaskCenter,
	ViewAlignmentRight = ViewAlignmentMaskRight,
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
	ViewAutoresizingFit = 0x4,
	ViewAutoresizingContain = 0x8
} ViewAutoresizing;

OBJECTIVELYMVC_EXPORT const EnumName ViewAutoresizingNames[];

/**
 * @brief Spacing applied to the inside of a View's frame.
 */
typedef struct {
	int top, right, bottom, left;
} ViewPadding;

/**
 * @brief Creates a ViewPadding with the given dimensions.
 */
#define MakePadding(top, right, bottom, left) \
	(ViewPadding) { (top), (right), (bottom), (left) }

/**
 * @return A ViewPadding that is the sum of `a` and `b`.
 */
#define AddPadding(a, b) \
	MakePadding(a.top + b.top, a.right + b.right, a.bottom + b.bottom, a.left + b.left)

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
	 * @brief The class names.
	 * @see Style
	 */
	MutableSet *classNames;

	/**
	 * @brief If true, subviews will be clipped to this View's frame.
	 */
	bool clipsSubviews;

	/**
	 * @brief The computed Style of this View.
	 */
	Style *computedStyle;

	/**
	 * @brief The frame, relative to the superview.
	 */
	SDL_Rect frame;

	/**
	 * @brief If `true`, this View is not drawn.
	 */
	bool hidden;

	/**
	 * @brief An optional identifier.
	 * @remarks Identifiers are commonly used to resolve Outlets when loading Views via JSON.
	 */
	char *identifier;

	/**
	 * @brief The maximum size this View may be resized to during layout.
	 */
	SDL_Size maxSize;

	/**
	 * @brief The minimum size this View may be resized to during layout.
	 */
	SDL_Size minSize;

	/**
	 * @brief If true, this View will apply the Theme before it is drawn.
	 */
	bool needsApplyTheme;

	/**
	 * @brief If true, this View will layout its subviews before it is drawn.
	 */
	bool needsLayout;

	/**
	 * @brief The next responder, or event handler, in the chain.
	 * @remarks By default, Views propagate events to their superview. If this member is not `NULL`,
	 * events will instead be propagated to this View.
	 */
	View *nextResponder;

	/**
	 * @brief The padding.
	 */
	ViewPadding padding;

	/**
	 * @brief The element-level Style of this View.
	 * @remarks Attributes in this Style are local to this View, and override any Attributes matched
	 * via Selector. That is, it is always the last Style added to the computed Style.
	 */
	Style *style;

	/**
	 * @brief An optional Stylesheet.
	 * @remarks If set, this Stylesheet is added to or removed from the current Theme when this
	 * View is added to or removed from a valid View hierarchy.
	 */
	Stylesheet *stylesheet;

	/**
	 * @brief The immediate subviews.
	 */
	MutableArray *subviews;

	/**
	 * @brief The super View.
	 * @remarks This reference is not retained.
	 */
	View *superview;

	/**
	 * @brief The ViewController.
	 * @remarks This is `NULL` unless the View is the immediate `view` of a ViewController.
	 */
	ViewController *viewController;

	/**
	 * @brief The warnings this View generated.
	 * @remarks These are optionally displayed by the DebugViewController.
	 */
	MutableArray *warnings;

	/**
	 * @brief The window.
	 * @remarks This is `NULL` until the View has been added to a WindowController.
	 */
	SDL_Window *window;
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
	 * @fn bool View::acceptsFirstResponder(const View *self)
	 * @param self The View.
	 * @return True if this View can become the first responder, false otherwise.
	 * @memberof View
	 */
	bool (*acceptsFirstResponder)(const View *self);

	/**
	 * @fn void View::addClassName(View *self, const char *className)
	 * @brief Adds the given class name to this View.
	 * @param self The View.
	 * @param className The class name.
	 * @memberof View
	 */
	void (*addClassName)(View *self, const char *className);

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
	 * @fn View::ancestorWithIdentifier(const View *self, const char *identifier)
	 * @param self The View.
	 * @param identifier The identifier.
	 * @return The nearest ancestor View matching the given identifier.
	 * @memberof View
	 */
	View *(*ancestorWithIdentifier)(const View *self, const char *identifier);

	/**
	 * @fn void View::applyStyle(View *self, const Style *style)
	 * @brief Applies the given Style to this View.
	 * @param self The View.
	 * @param style The Style.
	 * @memberof View
	 */
	void (*applyStyle)(View *self, const Style *style);

	/**
	 * @fn void View::applyTheme(View *self, const Theme *theme)
	 * @brief Applies the given Theme to this View.
	 * @param self The View.
	 * @param theme The Theme.
	 * @memberof View
	 */
	void (*applyTheme)(View *self, const Theme *theme);

	/**
	 * @fn void View::applyThemeIfNeeded(View *self, const Theme *theme)
	 * @brief Recursively applies the Theme to this View and its subviews.
	 * @param self The View.
	 * @param theme The Theme.
	 * @memberof View
	 */
	void (*applyThemeIfNeeded)(View *self, const Theme *theme);

	/**
	 * @fn void View::attachStylesheet(View *self, SDL_Window *window)
	 * @brief Attaches this View's Stylesheet to the Theme associated with the given window.
	 * @param self The View.
	 * @param window The window.
	 * @memberof View
	 */
	void (*attachStylesheet)(View *self, SDL_Window *window);

	/**
	 * @fn void View::awakeWithCharacters(View *self, const char *chars)
	 * @brief Wakes this View with the given null-terminated JSON C string.
	 * @param self The View.
	 * @param chars A null-terminated JSON C string describing this View.
	 * @remarks This is a convenience method for View::awakeWithDictionary.
	 * @memberof View
	 */
	void (*awakeWithCharacters)(View *self, const char *chars);

	/**
	 * @fn void View::awakeWithData(View *self, const Data *data)
	 * @brief Wakes this View with the specified JSON Data.
	 * @param self The View.
	 * @param data The JSON Data containing properties describing this View.
	 * @remarks This is a convenience method for View::awakeWithDictionary.
	 * @memberof View
	 */
	void (*awakeWithData)(View *self, const Data *data);

	/**
	 * @fn void View::awakeWithDictionary(View *self, const Dictionary *dictionary)
	 * @brief Wakes this View with the specified Dictionary.
	 * @param self The View.
	 * @param dictionary A Dictionary of properties describing this View.
	 * @remarks This method is invoked when loading via JSON. Subclasses should override this method
	 * to perform any customization based on the contents of `dictionary`.
	 * @memberof View
	 */
	void (*awakeWithDictionary)(View *self, const Dictionary *dictionary);

	/**
	 * @fn void View::awakeWithResource(View *self, const Resource *resource)
	 * @brief Wakes this View with the specified Resource.
	 * @param self The View.
	 * @param resource A Resource providing JSON data describing this View.
	 * @memberof View
	 */
	void (*awakeWithResource)(View *self, const Resource *resource);

	/**
	 * @fn void View::awakeWithResourceName(View *self, const char *name)
	 * @brief Wakes this View with the Resource by the specified name.
	 * @param self The View.
	 * @param name The name of a Resource providing JSON data describing this View.
	 * @memberof View
	 */
	void (*awakeWithResourceName)(View *self, const char *name);

	/**
	 * @fn void View::becomeFirstResponder(View *self)
	 * @brief Become the first responder in the View hierarchy.
	 * @param self The View.
	 * @remarks Becoming the first responder gives a View priority when handling events.
	 * @memberof View
	 */
	void (*becomeFirstResponder)(View *self);

	/**
	 * @fn bool View::bind(View *self, const Inlet *inlets, const Dictionary *dictionary)
	 * @brief Performs data binding for the Inlets described in `dictionary`.
	 * @param self The View.
	 * @param inlets The Inlets to bind.
	 * @param dictionary A Dictionary describing this View.
	 * @return True if one or more Inlet was bound, false otherwise.
	 * @remarks Subclasses will typically call this method from View::awakeWithDictionary.
	 * @memberof View
	 */
	bool (*bind)(View *self, const Inlet *inlets, const Dictionary *dictionary);

	/**
	 * @fn SDL_Rect View::bounds(const View *self)
	 * @param self The View.
	 * @return The bounds (frame minus padding) of this View.
	 * @memberof View
	 */
	SDL_Rect (*bounds)(const View *self);

	/**
	 * @fn void View::bringSubviewToFront(View *self, View *subview)
	 * @brief Brings the specified subview to the front.
	 * @param self The View.
	 * @param subview The subview.
	 * @memberof View
	 */
	void (*bringSubviewToFront)(View *self, View *subview);

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
	 * @fn bool View::containsPoint(const View *self, const SDL_Point *point)
	 * @param self The View.
	 * @param point A point in object space.
	 * @return True if the point falls within this View's clipped frame.
	 * @memberof View
	 */
	bool (*containsPoint)(const View *self, const SDL_Point *point);

	/**
	 * @fn int View::depth(const View *self)
	 * @param self The View.
	 * @return The depth of this View (`ancestor depth + 1`).
	 * @memberof View
	 */
	int (*depth)(const View *self);

	/**
	 * @fn View::descendantWithIdentifier(const View *self, const char *identifier)
	 * @param self The View.
	 * @param identifier The identifier.
	 * @return The nearest descendant View matching the given identifier.
	 * @memberof View
	 */
	View *(*descendantWithIdentifier)(const View *self, const char *identifier);

	/**
	 * @fn void View::detachStylesheet(View *self, SDL_Window *window)
	 * @brief Detaches this View's Stylesheet from the Theme associated with the given window.
	 * @param self The View.
	 * @param window The window.
	 * @memberof View
	 */
	void (*detachStylesheet)(View *self, SDL_Window *window);

	/**
	 * @fn void View::didMoveToWindow(View *self, SDL_Window *window)
	 * @brief Informs this View that it has been added to the View hierachy of the given window.
	 * @param self The View.
	 * @param window The window, or `NULL` if this View has been removed from the window.
	 * @memberof View
	 */
	void (*didMoveToWindow)(View *self, SDL_Window *window);

	/**
	 * @fn bool View::didReceiveEvent(const View *self, const SDL_Event *event)
	 * @param self The View.
	 * @param event The event.
	 * @return True if this View received the event, false otherwise.
	 * @memberof View
	 */
	bool (*didReceiveEvent)(const View *self, const SDL_Event *event);

	/**
	 * @fn void View::draw(View *self, Renderer *renderer)
	 * @brief Draws this View.
	 * @param self The View.
	 * @param renderer The Renderer.
	 * @remarks This method determines if the View is visible and dispatches Renderer::drawView
	 * before recursing down the View hierarchy. Rasterization is performed in View::render.
	 * @see View::render(View *, Renderer *)
	 * @memberof View
	 */
	void (*draw)(View *self, Renderer *renderer);

	/**
	 * @fn void View::emitViewEvent(View *self, ViewEvent code, ident data)
	 * @brief Emits a ViewEvent originating from this View.
	 * @param self The View.
	 * @param code The ViewEvent code.
	 * @param data The ViewEvent data.
	 * @remarks This method is used for emitting events from Views (as opposed to responding to
	 * user or system generated events). These events are propagated up the View hierarchy.
	 * @memberof View
	 */
	void (*emitViewEvent)(View *self, ViewEvent code, ident data);

	/**
	 * @fn void View::enumerate(View *self, ViewEnumerator enumerator, ident data)
	 * @brief Enumerates this View and its descendants, applying `enumerator` to each.
	 * @param self The View.
	 * @param enumerator The ViewEnumerator.
	 * @param data User data.
	 * @memberof View
	 */
	void (*enumerate)(View *self, ViewEnumerator enumerator, ident data);

	/**
	 * @fn void View::enumerateAdjacent(const View *self, ViewEnumerator enumerator, ident data)
	 * @brief Enumerates adjacent siblings of this View, applying `enumerator` to each.
	 * @param self The View.
	 * @param enumerator The ViewEnumerator.
	 * @param data User data.
	 * @memberof View
	 */
	void (*enumerateAdjacent)(const View *self, ViewEnumerator enumerator, ident data);

	/**
	 * @fn void View::enumerateAncestors(const View *self, ViewEnumerator enumerator, ident data)
	 * @brief Enumerates all ancestors of this View, applying `enumerator` to each.
	 * @param self The View.
	 * @param enumerator The ViewEnumerator.
	 * @param data User data.
	 * @memberof View
	 */
	void (*enumerateAncestors)(const View *self, ViewEnumerator enumerator, ident data);

	/**
	 * @fn void View::enumerateDescendants(const View *self, ViewEnumerator enumerator, ident data)
	 * @brief Enumerates all descendants of this View, applying `enumerator` to each.
	 * @param self The View.
	 * @param enumerator The ViewEnumerator.
	 * @param data User data.
	 * @memberof View
	 */
	void (*enumerateDescendants)(const View *self, ViewEnumerator enumerator, ident data);

	/**
	 * @fn void View::enumerateSelection(View *self, const char *rule, ViewEnumerator enumerator, ident data)
	 * @brief Enumerates all Views in the selection matched by `rule`, applying `enumerator` to each.
	 * @param self The View.
	 * @param rule The Selector rule.
	 * @param enumerator The ViewEnumerator.
	 * @param data User data.
	 * @memberof View
	 */
	void (*enumerateSelection)(View *self, const char *rule, ViewEnumerator enumerator, ident data);

	/**
	 * @fn void View::enumerateSiblings(const View *self, ViewEnumerator enumerator, ident data)
	 * @brief Enumerates all siblings of this View, applying `enumerator` to each.
	 * @param self The View.
	 * @param enumerator The ViewEnumerator.
	 * @param data User data.
	 * @memberof View
	 */
	void (*enumerateSiblings)(const View *self, ViewEnumerator enumerator, ident data);

	/**
	 * @fn void View::enumerateSubviews(const View *self, ViewEnumerator enumerator, ident data)
	 * @brief Enumerates all subviews of this View, applying `enumerator` to each.
	 * @param self The View.
	 * @param enumerator The ViewEnumerator.
	 * @param data User data.
	 * @memberof View
	 */
	void (*enumerateSubviews)(const View *self, ViewEnumerator enumerator, ident data);

	/**
	 * @fn void View::enumerateSuperview(const View *self, ViewEnumerator enumerator, ident data)
	 * @brief Enumerates the superview of this View, if any, applying `enumerator` to it.
	 * @param self The View.
	 * @param enumerator The ViewEnumerator.
	 * @param data User data.
	 * @memberof View
	 */
	void (*enumerateSuperview)(const View *self, ViewEnumerator enumerator, ident data);

	/**
	 * @fn void View::enumerateVisible(View *self, ViewEnumerator enumerator, ident data)
	 * @brief Enumerates this View and its visible descendants, applying `enumerator` to each.
	 * @details This is an optimized path for enumerating only visible subtrees.
	 * @param self The View.
	 * @param enumerator The ViewEnumerator.
	 * @param data User data.
	 * @memberof View
	 */
	void (*enumerateVisible)(View *self, ViewEnumerator enumerator, ident data);

	/**
	 * @static
	 * @fn View *View::firstResponder(SDL_Window *window)
	 * @param window The window.
	 * @return The first responder for the given window, or `NULL` if none.
	 * @memberof View
	 */
	View *(*firstResponder)(SDL_Window *window);

	/**
	 * @fn bool View::hasClassName(const View *self, cosnt char *className)
	 * @param self The View
	 * @param className The class name.
	 * @return True if this View has the given class name, false otherwise.
	 * @memberof View
	 */
	bool (*hasClassName)(const View *self, const char *className);

	/**
	 * @fn bool View::hasOverflow(const View *self)
	 * @param self The View
	 * @return True if this View's visible subviews exceed this View's bounds.
	 * @memberof View
	 */
	bool (*hasOverflow)(const View *self);

	/**
	 * @fn View *View::hitTest(const View *self, const SDL_Point *point)
	 * @brief Performs a hit test against this View and its descendants for the given point.
	 * @param self The View.
	 * @param point The point to test.
	 * @return The furthest descendant View that contains the given point.
	 * @memberof View
	 */
	View *(*hitTest)(const View *self, const SDL_Point *point);

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
	 * @fn void View::invalidateStyle(View *self)
	 * @brief Invalidates the computed Style for this View and its descendants.
	 * @param self The View.
	 * @memberof View
	 */
	void (*invalidateStyle)(View *self);

	/**
	 * @fn bool View::isContainer(const View *self)
	 * @param self The view.
	 * @return True if this View's autoresizing mask includes `Fit` or `Contain`.
	 * @memberof View
	 */
	bool (*isContainer)(const View *self);

	/**
	 * @fn bool View::isDescendantOfView(const View *self, const View *view)
	 * @param self The View.
	 * @param view The View to test against this View's hierarchy.
	 * @return True if this View is a descendant of, or equal to, the given View.
	 * @memberof View
	 */
	bool (*isDescendantOfView)(const View *self, const View *view);

	/**
	 * @fn bool View::isFirstResponder(const View *self)
	 * @param self The View.
	 * @return True if this View is the first responder, false otherwise.
	 * @memberof View
	 */
	bool (*isFirstResponder)(const View *self);

	/**
	 * @fn bool View::isVisible(const View *self)
	 * @param self The View.
	 * @return True if this View is visible, false if it, or an ancestor, is hidden.
	 * @memberof View
	 */
	bool (*isVisible)(const View *self);

	/**
	 * @fn void View::layoutIfNeeded(View *self)
	 * @brief Recursively updates the layout of this View and its subviews.
	 * @param self The View.
	 * @memberof View
	 */
	void (*layoutIfNeeded)(View *self);

	/**
	 * @fn View::layoutSubviews(View *self)
	 * @brief Performs layout for this View's immediate subviews.
	 * @param self The View.
	 * @remarks Subclasses may override this method to perform their own layout operations. This
	 * method is called recursively by View::layoutIfNeeded.
	 * @memberof View
	 */
	void (*layoutSubviews)(View *self);

	/**
	 * @fn bool View::matchesSelector(const View *self, const SimpleSelector *simpleSelector)
	 * @param self The View.
	 * @param simpleSelector The SimpleSelector.
	 * @return True if this View matches the SimpleSelector, false otherwise.
	 * @memberof View
	 */
	bool (*matchesSelector)(const View *self, const SimpleSelector *simpleSelector);

	/**
	 * @fn void View::moveToWindow(View *self, SDL_Window *window)
	 * @brief Moves this View to the View hierarchy of the given window.
	 * @param self The View.
	 * @param window The window, or `NULL` if this View is moving from its current window.
	 * @memberof View
	 */
	void (*moveToWindow)(View *self, SDL_Window *window);

	/**
	 * @fn String *View::path(const View *self)
	 * @param self The View.
	 * @return A unique CSS selector matching this View.
	 * @memberof View
	 */
	String *(*path)(const View *self);

	/**
	 * @fn void View::removeAllClassNames(View *self)
	 * @brief Removes all class names from this View.
	 * @param self The View.
	 * @memberof View
	 */
	void (*removeAllClassNames)(View *self);

	/**
	 * @fn void View::removeAllSubviews(View *self)
	 * @brief Removes all subviews from this View.
	 * @param self The View.
	 * @memberof View
	 */
	void (*removeAllSubviews)(View *self);

	/**
	 * @fn void View::removeClassName(View *self, const char *className)
	 * @brief Removes the given class name to this View.
	 * @param self The View.
	 * @param className The class name.
	 * @memberof View
	 */
	void (*removeClassName)(View *self, const char *className);

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
	 * @brief Informs this View that the render device has reset.
	 * @param self The View.
	 * @remarks Subclasses may override this method to allocate any OpenGL objects they require.
	 * @memberof View
	 */
	void (*renderDeviceDidReset)(View *self);

	/**
	 * @fn void View::renderDeviceWillReset(View *self)
	 * @brief Informs this View that the render device will reset.
	 * @param self The View.
	 * @remarks Subclasses should override this method to free any OpenGL objects they own.
	 * @memberof View
	 */
	void (*renderDeviceWillReset)(View *self);

	/**
	 * @fn SDL_Frame View::renderFrame(const View *self)
	 * @param self The View.
	 * @return This View's absolute frame in the View hierarchy, in object space.
	 * @memberof View
	 */
	SDL_Rect (*renderFrame)(const View *self);

	/**
	 * @fn void View::replaceSubview(View *self, View *subview, View *replacement)
	 * @brief Replaces the specified subview with the given replacement.
	 * @param self The View.
	 * @param subview The subview to replace.
	 * @param replacement The replacement subview.
	 * @memberof View
	 */
	void (*replaceSubview)(View *self, View *subview, View *replacement);

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
	 * @fn void View::resolve(View *self, Outlet *outlets)
	 * @brief Resolves the given Outlets from this View's hierarchy.
	 * @param self The View.
	 * @param outlets The Outlets to resolve.
	 * @memberof View
	 */
	void (*resolve)(View *self, Outlet *outlets);

	/**
	 * @fn void View::respondToEvent(View *self, const SDL_Event *event)
	 * @brief Responds to the specified event.
	 * @param self The View.
	 * @param event The event.
	 * @memberof View
	 */
	void (*respondToEvent)(View *self, const SDL_Event *event);

	/**
	 * @fn Set *View::select(View *self, const char *rule)
	 * @brief Resolves all descendants (including this View) that match the given Selector rule.
	 * @param self The View.
	 * @param rule The Selector rule.
	 * @return The Set of selected descendant Views.
	 * @memberof View
	 */
	Set *(*select)(View *self, const char *rule);

	/**
	 * @static
	 * @fn void View::setFirstResponder(SDL_Window *window, View *view)
	 * @brief Sets the first responder for the given window.
	 * @param window The window.
	 * @param view The View.
	 * @memberof View
	 */
	void (*setFirstResponder)(SDL_Window *window, View *view);

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
	 * @fn SDL_Size View::sizeThatFills(const View *self)
	 * @param self The View.
	 * @return An SDL_Size that fills this View's superview.
	 * @memberof View
	 */
	SDL_Size (*sizeThatFills)(const View *self);

	/**
	 * @fn void View::sizeToContain(View *self)
	 * @brief Resizes this View to contain its subviews.
	 * @param self The View.
	 * @see View::sizeThatContains(const View *)
	 * @memberof View
	 */
	void (*sizeToContain)(View *self);

	/**
	 * @fn void View::sizeToFill(View *self)
	 * @brief Resizes this View to fill its superview.
	 * @param self The View.
	 * @see View::sizeThatFills(const View *)
	 * @memberof View
	 */
	void (*sizeToFill)(View *self);

	/**
	 * @fn void View::sizeToFit(View *self)
	 * @brief Resizes this View to fit its subviews.
	 * @param self The View.
	 * @see View::sizeThatFits(const View *)
	 * @memberof View
	 */
	void (*sizeToFit)(View *self);

	/**
	 * @fn View *View::subviewWithIdentifier(const View *self, const char *identifier)
	 * @param self The View.
	 * @param identifier The identifier.
	 * @return The first subview matching the given identifier.
	 * @memberof View
	 */
	View *(*subviewWithIdentifier)(const View *self, const char *identifier);

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
	 * @fn View *View::viewWithCharacters(const char *chars, Outlet *outlets)
	 * @brief Instantiates a View initialized with the given null-terminated JSON C string.
	 * @param chars A null-terminated JSON C string describing a View.
	 * @param outlets An optional array of Outlets to resolve.
	 * @return The initialized View, or `NULL` on error.
	 * @memberof View
	 */
	View *(*viewWithCharacters)(const char *chars, Outlet *outlets);

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
	 * @static
	 * @fn View *View::viewWithResource(const Resource *resource, Outlet *outlets)
	 * @brief Instantiates a View initialized with the JSON data in `resource`.
	 * @param resource A Resource containing JSON describing a View.
	 * @param outlets An optional array of Outlets to resolve.
	 * @return The initialized View, or `NULL` on error.
	 * @see View::initWithData(View *, const Data *, Outlet *outlets)
	 * @memberof View
	 */
	View *(*viewWithResource)(const Resource *resource, Outlet *outlets);

	/**
	 * @static
	 * @fn View *View::viewWithResourceName(const char *name, Outlet *outlets)
	 * @brief Instantiates a View initialized with the JSON Resource with the specified `name`.
	 * @param name The name of a JSON Resource describing a View.
	 * @param outlets An optional array of Outlets to resolve.
	 * @return The initialized View, or `NULL` on error.
	 * @see View::initWithResource(View *, const Dictionary *, Outlet *outlets)
	 * @memberof View
	 */
	View *(*viewWithResourceName)(const char *name, Outlet *outlets);

	/**
	 * @fn Array *View::visibleSubviews(const View *self)
	 * @param self The View.
	 * @return An Array of this View's visible subviews.
	 * @remarks The returned Array excludes that specify ViewAlignmentInternal.
	 * @memberof View
	 */
	Array *(*visibleSubviews)(const View *self);

	/**
	 * @fn void View::warn(View *self, const char *fmt, ...)
	 * @brief Appends a warning for this View.
	 * @param self The View.
	 * @param fmt The format string.
	 * @memberof View
	 */
	void (*warn)(View *self, const char *fmt, ...);

	/**
	 * @fn void View::willMoveToWindow(View *self, SDL_Window *window)
	 * @brief Informs this View that it will be added to the View hierarchy for the given window.
	 * @param self The View.
	 * @param window The window, or `NULL` if this View will be removed from the window.
	 * @memberof View
	 */
	void (*willMoveToWindow)(View *self, SDL_Window *window);
};

/**
 * @fn Class *View::_View(void)
 * @brief The View archetype.
 * @return The View Class.
 * @memberof View
 */
OBJECTIVELYMVC_EXPORT Class *_View(void);
