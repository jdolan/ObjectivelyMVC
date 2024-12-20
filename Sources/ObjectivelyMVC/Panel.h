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

#include <ObjectivelyMVC/Control.h>
#include <ObjectivelyMVC/ImageView.h>
#include <ObjectivelyMVC/StackView.h>

/**
 * @file
 * @brief Draggable containers.
 */

#define DEFAULT_PANEL_RESIZE_HANDLE_SIZE 10

typedef struct Panel Panel;
typedef struct PanelInterface PanelInterface;

/**
 * @brief Draggable and resizable container Views.
 * @extends Control
 * @ingroup Containers Controls
 * @remarks Panels are draggable containers, analagous to windows or dialogs in traditional
 * desktop environments. Panels can optionally be positioned and resized by the user.
 */
struct Panel {

	/**
	 * @brief The superclass.
	 */
	Control control;

	/**
	 * @brief The interface.
	 * @protected
	 */
	PanelInterface *interface;

	/**
	 * @brief The optional accessories container.
	 * @remarks This View is hidden by default.
	 */
	StackView *accessoryView;

	/**
	 * @brief The internal container.
	 */
	StackView *contentView;

	/**
	 * @brief If true, this Panel may be repositioned by the user.
	 */
	bool isDraggable;

	/**
	 * @brief True if the user is repositioning this Panel.
	 */
	bool isDragging;

	/**
	 * @brief If true, this Panel may be resized by the user.
	 */
	bool isResizable;

	/**
	 * @brief True if the user is resizing this Panel.
	 */
	bool isResizing;

	/**
	 * @brief The minimum size to which this Panel's frame can be resized.
	 */
	SDL_Size minSize;

	/**
	 * @brief The maximum size to which this Panel's frame can be resized.
	 */
	SDL_Size maxSize;

	/**
	 * @brief The resize handle.
	 * @private
	 */
	ImageView *resizeHandle;

	/**
	 * @brief The StackView.
	 * @private
	 */
	StackView *stackView;
};

/**
 * @brief The Panel interface.
 */
struct PanelInterface {

	/**
	 * @brief The superclass interface.
	 */
	ControlInterface controlInterface;

	/**
	 * @fn SDL_Size Panel::contentSize(const Panel *self)
	 * @param self The Panel.
	 * @return The available content size.
	 * @memberof Panel
	 */
	SDL_Size (*contentSize)(const Panel *self);

	/**
	 * @fn Panel *Panel::initWithFrame(Panel *self, const SDL_Rect *frame)
	 * @brief Initializes this Panel with the specified frame and style.
	 * @param self The Panel.
	 * @param frame The frame.
	 * @return The initialized Panel, or `NULL` on error.
	 * @memberof Panel
	 */
	Panel *(*initWithFrame)(Panel *self, const SDL_Rect *frame);
};

/**
 * @fn Class *Panel::_Panel(void)
 * @brief The Panel archetype.
 * @return The Panel Class.
 * @memberof Panel
 */
OBJECTIVELYMVC_EXPORT Class *_Panel(void);

