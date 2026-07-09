/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and C.
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

#include <ObjectivelyMVC/StackView.h>

#include <ObjectivelyMVC/KeyValueView.h>

/**
 * @file
 * @brief A vertical table of KeyValueView rows with inherited columns.
 * @details A KeyValueTableView is a vertical StackView that owns the column
 * widths for its rows. On every layout it pushes its `-key-width` /
 * `-value-width` (CSS attributes on its own root) into every row via
 * KeyValueView::setColumnWidths -- so all rows (including dynamically added
 * ones) share one set of columns, configured in one place and enforced in C,
 * immune to the per-widget min-width floors that CSS width rules fight.
 *
 * The table GROWS with its content; it does not scroll. Scrolling, when needed,
 * is provided by an ancestor StackView with `scroll: true`.
 */

typedef struct KeyValueTableView KeyValueTableView;
typedef struct KeyValueTableViewInterface KeyValueTableViewInterface;

/**
 * @brief A vertical table of KeyValueView rows.
 * @extends StackView
 */
struct KeyValueTableView {

  /**
   * @brief The superclass.
   * @private
   */
  StackView stackView;

  /**
   * @brief The interface.
   * @private
   */
  KeyValueTableViewInterface *interface;

  /**
   * @brief Column-0 width pushed to every row. Style attribute `-key-width`.
   */
  int keyWidth;

  /**
   * @brief Column-1 width pushed to every row. Style attribute `-value-width`.
   */
  int valueWidth;
};

/**
 * @brief The KeyValueTableView interface.
 */
struct KeyValueTableViewInterface {

  /**
   * @brief The superclass interface.
   */
  StackViewInterface stackViewInterface;

  /**
   * @fn KeyValueTableView *KeyValueTableView::initWithFrame(KeyValueTableView *self, const SDL_Rect *frame)
   * @brief Initializes this KeyValueTableView with the given frame.
   * @param self The KeyValueTableView.
   * @param frame The frame, or `NULL`.
   * @return The initialized KeyValueTableView, or `NULL` on error.
   * @memberof KeyValueTableView
   */
  KeyValueTableView *(*initWithFrame)(KeyValueTableView *self, const SDL_Rect *frame);

  /**
   * @fn KeyValueView *KeyValueTableView::addRow(KeyValueTableView *self, View *key, View *value)
   * @brief Appends a row holding `key` and `value`, inheriting the table columns.
   * @param self The KeyValueTableView.
   * @param key The View for column 0 (any View), or `NULL`.
   * @param value The View for column 1 (any View), or `NULL`.
   * @return The new KeyValueView row.
   * @memberof KeyValueTableView
   */
  KeyValueView *(*addRow)(KeyValueTableView *self, View *key, View *value);

  /**
   * @fn void KeyValueTableView::addRowView(KeyValueTableView *self, KeyValueView *row)
   * @brief Appends a pre-built KeyValueView (e.g. an editable subclass) as a row.
   * @param self The KeyValueTableView.
   * @param row The row to append; it inherits the table columns.
   * @memberof KeyValueTableView
   */
  void (*addRowView)(KeyValueTableView *self, KeyValueView *row);

  /**
   * @fn void KeyValueTableView::removeAllRows(KeyValueTableView *self)
   * @brief Removes every row from the table.
   * @param self The KeyValueTableView.
   * @memberof KeyValueTableView
   */
  void (*removeAllRows)(KeyValueTableView *self);
};

/**
 * @fn Class *KeyValueTableView::_KeyValueTableView(void)
 * @brief The KeyValueTableView archetype.
 * @return The KeyValueTableView Class.
 * @memberof KeyValueTableView
 */
OBJECTIVELYMVC_EXPORT Class *_KeyValueTableView(void);
