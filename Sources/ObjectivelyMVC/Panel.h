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

#include <ObjectivelyMVC/ImageView.h>
#include <ObjectivelyMVC/StackView.h>
#include <ObjectivelyMVC/View.h>

/**
 * @file
 *
 * @brief Draggable containers.
 */

#define DEFAULT_PANEL_PADDING 10
#define DEFAULT_PANEL_RESIZE_HANDLE_SIZE 10
#define DEFAULT_PANEL_SPACING 10

typedef struct Panel Panel;
typedef struct PanelInterface PanelInterface;

/**
 * @brief Draggable and resizable container Views.
 *
 * @extends StackView
 *
 * @ingroup Containers
 *
 * @remarks Panels are draggable containers, analagous to windows or dialogs in traditional 
 * desktop environments. Panels can optionally be positioned and resized by the user.
 */
struct Panel {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	StackView stackView;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	PanelInterface *interface;

	/**
	 * @brief The optional accessories container.
	 *
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
	_Bool isDraggable;

	/**
	 * @brief True if the user is repositioning this Panel.
	 */
	_Bool isDragging;

	/**
	 * @brief If true, this Panel may be resized by the user.
	 */
	_Bool isResizable;

	/**
	 * @brief True if the user is resizing this Panel.
	 */
	_Bool isResizing;

	/**
	 * @brief The resize handle.
	 *
	 * @private
	 */
	ImageView *resizeHandle;
};

/**
 * @brief The Panel interface.
 */
struct PanelInterface {
	
	/**
	 * @brief The parent interface.
	 */
	StackViewInterface stackViewInterface;

	/**
	 * @fn SDL_Size Panel::contentSize(const Panel *self)
	 *
	 * @return The available content size.
	 *
	 * @memberof Panel
	 */
	SDL_Size (*contentSize)(const Panel *self);
	
	/**
	 * @fn Panel *Panel::initWithFrame(Panel *self, const SDL_Rect *frame)
	 *
	 * @brief Initializes this Panel with the specified frame.
	 *
	 * @param frame The frame.
	 *
	 * @return The initialized Panel, or `NULL` on error.
	 *
	 * @memberof Panel
	 */
	Panel *(*initWithFrame)(Panel *self, const SDL_Rect *frame);
};

/**
 * @brief The Panel Class.
 */
extern Class _Panel;

