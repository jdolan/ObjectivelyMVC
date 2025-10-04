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

#include <ObjectivelyMVC/TableCellView.h>

/**
 * @file
 * @brief Header cells for TableViews.
 */

typedef struct TableHeaderCellView TableHeaderCellView;
typedef struct TableHeaderCellViewInterface TableHeaderCellViewInterface;

/**
 * @brief Header cells provide clickable sort handles for TableView instances.
 * @extends TableCellView
 * @ingroup Tables
 */
struct TableHeaderCellView {

  /**
   * @brief The superclass.
   */
  TableCellView tableCellView;

  /**
   * @brief The interface.
   * @protected
   */
  TableHeaderCellViewInterface *interface;
};

/**
 * @brief The TableHeaderCellView interface.
 */
struct TableHeaderCellViewInterface {

  /**
   * @brief The superclass interface.
   */
  TableCellViewInterface tableCellViewInterface;

  /**
   * @fn TableHeaderCellView *TableHeaderCellView::initWithFrame(TableHeaderCellView *self, const SDL_Rect *frame)
   * @brief Initializes this TableHeaderCellView with the specified frame.
   * @param self The TableHeaderCellView.
   * @param frame The frame.
   * @return The initialized TableHeaderCellView, or `NULL` on error.
   * @memberof TableHeaderCellView
   */
  TableHeaderCellView *(*initWithFrame)(TableHeaderCellView *self, const SDL_Rect *frame);
};

/**
 * @fn Class *TableHeaderCellView::_TableHeaderCellView(void)
 * @brief The TableHeaderCellView archetype.
 * @return The TableHeaderCellView Class.
 * @memberof TableHeaderCellView
 */
OBJECTIVELYMVC_EXPORT Class *_TableHeaderCellView(void);

