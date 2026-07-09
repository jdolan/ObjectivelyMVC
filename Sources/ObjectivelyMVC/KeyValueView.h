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

/**
 * @file
 * @brief A single key -> value row holding an arbitrary View in each column.
 * @details A KeyValueView is a horizontal StackView holding two arbitrary
 * Views: the key (column 0) and the value (column 1). Any View may be used for
 * either (Label, TextView, Select, Slider, Checkbox, ...). Column widths are
 * enforced by setColumnWidths, which rigidly clamps each widget's own
 * minSize/maxSize width (the same mechanism View::resize honors for any widget,
 * which is why it never loses to a stylesheet min-width floor). Alignment thus
 * never depends on per-widget CSS — only the widget's own look (font, color) is
 * styled, by type-level rules that bind reliably. Authored declaratively via
 * the "key" and "value" JSON inlets, or in C via setKey/setValue.
 */

typedef struct KeyValueView KeyValueView;
typedef struct KeyValueViewInterface KeyValueViewInterface;

/**
 * @brief A key -> value row of two arbitrary Views.
 * @extends StackView
 */
struct KeyValueView {

  /**
   * @brief The superclass.
   * @private
   */
  StackView stackView;

  /**
   * @brief The interface.
   * @private
   */
  KeyValueViewInterface *interface;

  /**
   * @brief The current key widget (column 0). Weak; owned as a subview.
   */
  View *key;

  /**
   * @brief The current value widget (column 1). Weak; owned as a subview.
   */
  View *value;

  /**
   * @brief The column widths last set, re-applied when a widget is replaced.
   * @private
   */
  int keyWidth, valueWidth;
};

/**
 * @brief The KeyValueView interface.
 */
struct KeyValueViewInterface {

  /**
   * @brief The superclass interface.
   */
  StackViewInterface stackViewInterface;

  /**
   * @fn KeyValueView *KeyValueView::initWithFrame(KeyValueView *self, const SDL_Rect *frame)
   * @brief Initializes this KeyValueView with the given frame.
   * @param self The KeyValueView.
   * @param frame The frame, or `NULL`.
   * @return The initialized KeyValueView, or `NULL` on error.
   * @memberof KeyValueView
   */
  KeyValueView *(*initWithFrame)(KeyValueView *self, const SDL_Rect *frame);

  /**
   * @fn void KeyValueView::setKey(KeyValueView *self, View *key)
   * @brief Places `key` (any View) into the key cell, filling its width.
   * @param self The KeyValueView.
   * @param key The View to display in column 0, or `NULL` to clear it.
   * @memberof KeyValueView
   */
  void (*setKey)(KeyValueView *self, View *key);

  /**
   * @fn void KeyValueView::setValue(KeyValueView *self, View *value)
   * @brief Places `value` (any View) into the value cell, filling its width.
   * @param self The KeyValueView.
   * @param value The View to display in column 1, or `NULL` to clear it.
   * @memberof KeyValueView
   */
  void (*setValue)(KeyValueView *self, View *value);

  /**
   * @fn void KeyValueView::setColumnWidths(KeyValueView *self, int keyWidth, int valueWidth)
   * @brief Rigidly sizes the two cells, establishing the row's columns.
   * @param self The KeyValueView.
   * @param keyWidth The column-0 width, in points. Ignored if <= 0.
   * @param valueWidth The column-1 width, in points. Ignored if <= 0.
   * @memberof KeyValueView
   */
  void (*setColumnWidths)(KeyValueView *self, int keyWidth, int valueWidth);
};

/**
 * @fn Class *KeyValueView::_KeyValueView(void)
 * @brief The KeyValueView archetype.
 * @return The KeyValueView Class.
 * @memberof KeyValueView
 */
OBJECTIVELYMVC_EXPORT Class *_KeyValueView(void);
