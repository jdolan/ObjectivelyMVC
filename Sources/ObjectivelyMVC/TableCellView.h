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

#include <ObjectivelyMVC/Text.h>

/**
 * @file
 * @brief Each row in a TableView is comprised of TableCellViews.
 */

#define DEFAULT_TABLE_CELL_VIEW_PADDING 2

typedef struct TableCellView TableCellView;
typedef struct TableCellViewInterface TableCellViewInterface;

/**
 * @brief Each row in a TableView is comprised of TableCellViews.
 * @extends View
 * @ingroup Tables
 */
struct TableCellView {

	/**
	 * @brief The superclass.
	 */
	View view;

	/**
	 * @brief The interface.
	 * @protected
	 */
	TableCellViewInterface *interface;

	/**
	 * @brief The text.
	 */
	Text *text;
};

/**
 * @brief The TableCellView interface.
 */
struct TableCellViewInterface {

	/**
	 * @brief The superclass interface.
	 */
	ViewInterface viewInterface;

	/**
	 * @fn TableCellView *TableCellView::initWithFrame(TableCellView *self, const SDL_Rect *frame)
	 * @brief Initializes this TableCellView with the specified frame.
	 * @param self The TableCellView.
	 * @param frame The frame.
	 * @return The initialized TableCellView, or `NULL` on error.
	 * @memberof TableCellView
	 */
	TableCellView *(*initWithFrame)(TableCellView *self, const SDL_Rect *frame);
};

/**
 * @fn Class *TableCellView::_TableCellView(void)
 * @brief The TableCellView archetype.
 * @return The TableCellView Class.
 * @memberof TableCellView
 */
OBJECTIVELYMVC_EXPORT Class *_TableCellView(void);
