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

#include <assert.h>
#include <string.h>

#include "TableRowView.h"
#include "TableView.h"

#define _Class _TableRowView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  TableRowView *this = (TableRowView *) self;

  release(this->cells);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::matchesSelector(const View *, const SimpleSelector *)
 */
static bool matchesSelector(const View *self, const SimpleSelector *simpleSelector) {

  TableRowView *this = (TableRowView *) self;

  if (simpleSelector->type == SimpleSelectorTypePseudo) {
    if (strcmp("selected", simpleSelector->pattern) == 0) {
      return this->isSelected;
    }
  }

  return super(View, self, matchesSelector, simpleSelector);
}

/**
 * @see View::applyStyle(View *, const Style *)
 * @remarks When selected, nudges this row's own (unselected) background-color +25 per RGB
 * channel, rather than hard-coding an unrelated selection color. A stylesheet rule targeting
 * `TableRowView:selected` still takes precedence: if such a rule changed backgroundColor during
 * this same pass (via `super`), that value is left alone.
 * @remarks `applyStyle` may run more than once while selected (e.g. re-triggered by unrelated
 * hover-state invalidation elsewhere in the tree); reading the *incoming* backgroundColor as
 * "the base" would treat an already-nudged value as the new base and compound on every call,
 * drifting toward white. `unselectedBackgroundColor` is only ever captured while NOT selected, so
 * it stays a stable reference no matter how many times this runs while selected.
 */
static void applyStyle(View *self, const Style *style) {

  TableRowView *this = (TableRowView *) self;

  if (this->isSelected == false) {
    super(View, self, applyStyle, style);
    this->unselectedBackgroundColor = self->backgroundColor;
    return;
  }

  self->backgroundColor = this->unselectedBackgroundColor;

  super(View, self, applyStyle, style);

  if (memcmp(&self->backgroundColor, &this->unselectedBackgroundColor, sizeof(SDL_Color)) == 0) {
    const int selectedColorNudge = 25;
    const SDL_Color baseColor = this->unselectedBackgroundColor;
    // Nudge alpha too, uniformly with RGB: this framework represents "transparent" as
    // white with alpha=0, not black-transparent, so a TableRowView with no explicit
    // background (the common case) has base (255,255,255,0). Forcing alpha to 255 in
    // that case turned an invisible row into a solid white block; nudging alpha by
    // the same amount instead yields a faint, barely-visible highlight (alpha 25),
    // consistent with the subtle-overlay tints used elsewhere (e.g. #ffffff22).
    self->backgroundColor = MakeColor(
      (Uint8) min(255, baseColor.r + selectedColorNudge),
      (Uint8) min(255, baseColor.g + selectedColorNudge),
      (Uint8) min(255, baseColor.b + selectedColorNudge),
      (Uint8) min(255, baseColor.a + selectedColorNudge)
    );
  }
}

/**
 * @see View::layoutSubviews(View *)
 * @remarks After the normal StackView layout, the LAST cell is stretched to fill
 * whatever width remains after the preceding cells -- a "fixed label(s) + flexible
 * value" table shape (every current consumer is exactly this: a key/label column
 * plus a value column meant to reach the table's own width). This is not
 * expressible via StackView distribution alone: `Fill` scales every child
 * uniformly, so a caller-side fixed-width clamp on the other cells just fights and
 * undoes it. Grows the cell's real content widget (the one subview that isn't the
 * cell's own hidden default Text) rather than the cell itself, since the cell's
 * `autoresizing-mask:contain` hugs that content -- growing the content lets
 * Contain naturally report the wider size back up, instead of fighting it.
 */
static void layoutSubviews(View *self) {

  super(View, self, layoutSubviews);

  TableRowView *this = (TableRowView *) self;

  const Array *cells = (Array *) this->cells;
  if (cells->count < 2) {
    return;
  }

  const SDL_Rect bounds = $(self, bounds);

  int used = 0;
  for (size_t i = 0; i < cells->count - 1; i++) {
    const View *cell = $(cells, objectAtIndex, i);
    used += cell->frame.w + this->stackView.spacing;
  }

  TableCellView *last = $(cells, objectAtIndex, cells->count - 1);
  const View *lastView = (View *) last;

  // The cell's own padding is added back on top of its (Contain-computed) child
  // size, so the child must be sized to `width` MINUS that padding -- otherwise
  // the cell reports back width+padding next pass, which grows `bounds.w` on the
  // pass after that, compounding a few pixels wider on every re-layout (visible
  // as "clicking anything grows the row").
  const int width = max(0, bounds.w - used - lastView->padding.left - lastView->padding.right);

  const Array *subviews = lastView->subviews;
  for (size_t i = 0; i < subviews->count; i++) {

    View *child = $(subviews, objectAtIndex, i);
    if (child != (View *) last->text) {
      if (child->frame.w != width) {
        const SDL_Size size = MakeSize(width, child->frame.h);
        $(child, resize, &size);
      }
      break;
    }
  }

  ((View *) last)->needsLayout = true;
  $((View *) last, layoutIfNeeded);

  ((View *) last)->frame.x = used;
}

#pragma mark - TableRowView

/**
 * @fn void TableRowView::addCell(TableRowView *self, TableCellView *cell)
 * @memberof TableRowView
 */
static void addCell(TableRowView *self, TableCellView *cell) {

  assert(cell);

  $(self->cells, addObject, cell);

  $((View *) self, addSubview, (View *) cell);
}

/**
 * @fn TableRowView *TableRowView::initWithTableView(TableRowView *self, TableView *tableView)
 * @memberof TableRowView
 */
static TableRowView *initWithTableView(TableRowView *self, TableView *tableView) {

  self = (TableRowView *) super(StackView, self, initWithFrame, NULL);
  if (self) {

    const Array *columns = (Array *) tableView->columns;
    
    self->cells = $$(Array, arrayWithCapacity, columns->count);
    assert(self->cells);

    self->tableView = tableView;
    assert(self->tableView);
  }

  return self;
}

/**
 * @brief ArrayEnumerator to remove TableCellViews from the row.
 */
static void removeAllCells_enumerate(const Array *array, ident obj, ident data) {
  $((View *) obj, removeFromSuperview);
}

/**
 * @fn void TableRowView::removeAllCells(TableRowView *self)
 * @brief Removes all cells from this row.
 * @memberof TableRowView
 */
static void removeAllCells(TableRowView *self) {
  $(self->cells, removeAllObjectsWithEnumerator, removeAllCells_enumerate, NULL);
}

/**
 * @fn void TableRowView::removeCell(TableRowView *self, TableCellView *cell)
 * @memberof TableRowView
 */
static void removeCell(TableRowView *self, TableCellView *cell) {

  assert(cell);

  $(self->cells, removeObject, cell);

  $((View *) self, removeSubview, (View *) cell);
}

/**
 * @fn void TableRowView::setSelected(TableRowView *self, bool isSelected)
 * @memberof TableRowView
 */
static void setSelected(TableRowView *self, bool isSelected) {

  if (isSelected != self->isSelected) {
    self->isSelected = isSelected;
    $((View *) self, invalidateStyle);
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewInterface *) clazz->interface)->matchesSelector = matchesSelector;
  ((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
  ((ViewInterface *) clazz->interface)->applyStyle = applyStyle;

  ((TableRowViewInterface *) clazz->interface)->addCell = addCell;
  ((TableRowViewInterface *) clazz->interface)->initWithTableView = initWithTableView;
  ((TableRowViewInterface *) clazz->interface)->removeAllCells = removeAllCells;
  ((TableRowViewInterface *) clazz->interface)->removeCell = removeCell;
  ((TableRowViewInterface *) clazz->interface)->setSelected = setSelected;
}

/**
 * @fn Class *TableRowView::_TableRowView(void)
 * @memberof TableRowView
 */
Class *_TableRowView(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "TableRowView",
      .superclass = _StackView(),
      .instanceSize = sizeof(TableRowView),
      .interfaceOffset = offsetof(TableRowView, interface),
      .interfaceSize = sizeof(TableRowViewInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}
#undef _Class

