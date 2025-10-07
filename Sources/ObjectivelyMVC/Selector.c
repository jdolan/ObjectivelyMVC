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
 
#include <assert.h>
#include <string.h>

#include <Objectively/Hash.h>
#include <Objectively/MutableArray.h>
#include <Objectively/MutableSet.h>

#include "Log.h"
#include "Selector.h"
#include "View.h"

#define _Class _Selector

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Selector *this = (Selector *) self;

  release(this->sequences);
  free(this->rule);

  super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

  const Selector *this = (Selector *) self;

  return str(this->rule);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

  Selector *this = (Selector *) self;

  return HashForCString(HASH_SEED, this->rule);
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static bool isEqual(const Object *self, const Object *other) {

  if (super(Object, self, isEqual, other)) {
    return true;
  }

  if (other && $(other, isKindOfClass, _Selector())) {

    const Selector *this = (Selector *) self;
    const Selector *that = (Selector *) other;

    return strcmp(this->rule, that->rule) == 0;
  }

  return false;
}

#pragma mark - Selector

/**
 * @return The specificity of the given Selector.
 */
static int specificity(const Selector *selector) {

  int specificity = 0;

  for (size_t i = 0; i < selector->sequences->count; i++) {
    SelectorSequence *sequence = $(selector->sequences, objectAtIndex, i);

    for (size_t j = 0; j < sequence->simpleSelectors->count; j++) {
      SimpleSelector *simpleSelector = $(sequence->simpleSelectors, objectAtIndex, j);

      switch (simpleSelector->type) {
        case SimpleSelectorTypeId:
          specificity += 100;
          break;
        case SimpleSelectorTypeClass:
        case SimpleSelectorTypePseudo:
          specificity += 10;
          break;
        case SimpleSelectorTypeType:
          specificity += 1;
          Class *clazz = classForName(simpleSelector->pattern);
          if (clazz) {
            while (clazz) {
              if (clazz == _View()) {
                break;
              }
              specificity += 1;
              clazz = clazz->def.superclass;
            }
            if (clazz != _View()) {
              MVC_LogError("Class `%s` in Selector `%s` does not extend View\n",
                     simpleSelector->pattern, selector->rule);
            }
          } else {
            MVC_LogWarn("Class `%s` in Selector `%s` not found\n",
                  simpleSelector->pattern, selector->rule);
          }

          break;
        default:
          break;
      }
    }
  }

  return specificity;
}

/**
 * @fn Order Selector::compareTo(const Selector *self, const Selector *other)
 * @memberof Selector
 */
static Order compareTo(const Selector *self, const Selector *other) {

  assert(other);

  if (self->specificity < other->specificity) {
    return OrderAscending;
  } else if (self->specificity > other->specificity) {
    return OrderDescending;
  }

  return OrderSame;
}

/**
 * @fn void Selector::enumerateSelection(const Selector *self, View *view, ViewEnumerator enumerator, ident data)
 * @memberof Selector
 */
static void enumerateSelection(const Selector *self, View *view, ViewEnumerator enumerator, ident data) {

  assert(enumerator);

  Set *selection = $(self, select, view);
  assert(selection);

  Array *array = $(selection, allObjects);
  assert(array);

  for (size_t i = 0; i < array->count; i++) {
    enumerator($(array, objectAtIndex, i), data);
  }

  release(array);
  release(selection);
}

/**
 * @fn Selector *Selector::initWithRule(Selector *self, const char *rule)
 * @memberof Selector
 */
static Selector *initWithRule(Selector *self, const char *rule) {

  self = (Selector *) super(Object, self, init);
  if (self) {

    self->rule = strtrim(rule);
    assert(self->rule);

    self->sequences = $$(SelectorSequence, parse, self->rule);
    assert(self->sequences->count);

    self->specificity = specificity(self);
  }

  return self;
}

/**
 * @brief A context for View matching.
 */
typedef struct {
  const Array *sequences;
  size_t sequence;
  bool match;
} Match;

/**
 * @brief ViewEnumerator for matchesView.
 */
static bool _matchesView(const View *view, Match *match) {

  const SelectorSequence *sequence = $(match->sequences, objectAtIndex, match->sequence);

  if ($(sequence, matchesView, view)) {

    switch (sequence->left) {
      case SequenceCombinatorNone:
        match->match = true;
        break;

      case SequenceCombinatorDescendent:
        match->sequence--;
        $(view, enumerateAncestors, (ViewEnumerator) _matchesView, match);
        break;

      case SequenceCombinatorChild:
        match->sequence--;
        $(view, enumerateSuperview, (ViewEnumerator) _matchesView, match);
        break;

      case SequenceCombinatorSibling:
        match->sequence--;
        $(view, enumerateSiblings, (ViewEnumerator) _matchesView, match);
        break;

      case SequenceCombinatorAdjacent:
        match->sequence--;
        $(view, enumerateAdjacent, (ViewEnumerator) _matchesView, match);
        break;

      case SequenceCombinatorTerminal:
        break;
    }
  }

  return match->match;
}

/**
 * @fn bool Selector::matchesView(const Selector *self, View *view)
 * @memberof Selector
 */
static bool matchesView(const Selector *self, const View *view) {

  assert(view);

  return _matchesView(view, &(Match) {
    .sequences = self->sequences,
    .sequence = self->sequences->count - 1
  });
}

/**
 * @fn Array *Selector::parse(const char *rules)
 * @memberof Selector
 */
static Array *parse(const char *rules) {

  MutableArray *selectors = $$(MutableArray, arrayWithCapacity, 4);
  assert(selectors);

  if (rules) {

    const char *c = rules;
    while (*c) {
      const size_t size = strcspn(c, ",");
      if (size) {
        char *rule = calloc(1, size + 1);
        assert(rule);

        strncpy(rule, c, size);

        Selector *selector = $(alloc(Selector), initWithRule, rule);
        assert(selector);

        $(selectors, addObject, selector);

        release(selector);
        free(rule);
      }
      c += size;
      c += strspn(c, ", \t\n");
    }
  }

  return (Array *) selectors;
}

/**
 * @brief A context for View selection.
 */
typedef struct {
  const Array *sequences;
  size_t sequence;
  MutableSet *selection;
  View *first;
} Selection;

/**
 * @brief Recursively selects Views by iterating the SelectorSequences in the given Selection.
 */
static Set *__select(View *view, Selection *selection) {

  const SelectorSequence *sequence = $(selection->sequences, objectAtIndex, selection->sequence);
  
  if ($(sequence, matchesView, view)) {

    switch (sequence->right) {
      case SequenceCombinatorNone:
        break;

      case SequenceCombinatorDescendent:
        selection->sequence++;
        $(view, enumerateDescendants, (ViewEnumerator) __select, selection);
        break;

      case SequenceCombinatorChild:
        selection->sequence++;
        $(view, enumerateSubviews, (ViewEnumerator) __select, selection);
        break;

      case SequenceCombinatorSibling:
        selection->sequence++;
        $(view, enumerateSiblings, (ViewEnumerator) __select, selection);
        break;

      case SequenceCombinatorAdjacent:
        selection->sequence++;
        $(view, enumerateAdjacent, (ViewEnumerator) __select, selection);
        break;

      case SequenceCombinatorTerminal:
        $(selection->selection, addObject, view);
        break;
    }
  }

  $(view, enumerateSubviews, (ViewEnumerator) __select, selection);

  return (Set *) selection->selection;
}

/**
 * @fn Set *Selector::select(const Selector *self, View *view)
 * @memberof Selector
 */
static Set *_select(const Selector *self, View *view) {

  assert(view);

  return __select(view, &(Selection) {
    .sequences = self->sequences,
    .selection = $$(MutableSet, set),
  });
}

/**
 * @brief Recursively selects the first View by iterating the SelectorSequences in the given Selection.
 */
static void _selectFirst(Selection *selection, View *view) {

  const SelectorSequence *sequence = $(selection->sequences, objectAtIndex, selection->sequence);

  if ($(sequence, matchesView, view)) {

    switch (sequence->right) {
      case SequenceCombinatorNone:
        break;

      case SequenceCombinatorDescendent:
        selection->sequence++;
        $(view, enumerateDescendants, (ViewEnumerator) __select, selection);
        break;

      case SequenceCombinatorChild:
        selection->sequence++;
        $(view, enumerateSubviews, (ViewEnumerator) __select, selection);
        break;

      case SequenceCombinatorSibling:
        selection->sequence++;
        $(view, enumerateSiblings, (ViewEnumerator) __select, selection);
        break;

      case SequenceCombinatorAdjacent:
        selection->sequence++;
        $(view, enumerateAdjacent, (ViewEnumerator) __select, selection);
        break;

      case SequenceCombinatorTerminal:
        selection->first = view;
        return;
    }
  }

  $(view, enumerateSubviews, (ViewEnumerator) _selectFirst, selection);
}

/**
 * @fn Set *Selector::selectFirst(const Selector *self, View *view)
 * @memberof Selector
 */
static View *selectFirst(const Selector *self, View *view) {

  assert(view);

  Selection selection = {
    .sequences = self->sequences,
  };

  _selectFirst(&selection, view);
  return selection.first;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  ((ObjectInterface *) clazz->interface)->description = description;
  ((ObjectInterface *) clazz->interface)->hash = hash;
  ((ObjectInterface *) clazz->interface)->isEqual = isEqual;

  ((SelectorInterface *) clazz->interface)->enumerateSelection = enumerateSelection;
  ((SelectorInterface *) clazz->interface)->compareTo = compareTo;
  ((SelectorInterface *) clazz->interface)->initWithRule = initWithRule;
  ((SelectorInterface *) clazz->interface)->matchesView = matchesView;
  ((SelectorInterface *) clazz->interface)->parse = parse;
  ((SelectorInterface *) clazz->interface)->select = _select;
  ((SelectorInterface *) clazz->interface)->selectFirst = selectFirst;
}

/**
 * @fn Class *Selector::_Selector(void)
 * @memberof Selector
 */
Class *_Selector(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Selector",
      .superclass = _Object(),
      .instanceSize = sizeof(Selector),
      .interfaceOffset = offsetof(Selector, interface),
      .interfaceSize = sizeof(SelectorInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
