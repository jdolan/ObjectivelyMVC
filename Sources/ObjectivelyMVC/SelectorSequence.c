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
#include <stdlib.h>
#include <string.h>

#include <Objectively/MutableArray.h>

#include "SelectorSequence.h"
#include "View.h"

#define _Class _SelectorSequence

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  SelectorSequence *this = (SelectorSequence *) self;

  free(this->sequence);
  release(this->simpleSelectors);

  super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

  const SelectorSequence *this = (SelectorSequence *) self;

  return str(this->sequence);
}

#pragma mark - SelectorSequence

/**
 * @fn SelectorSequence *SelectorSequence::initWithSequence(SelectorSequence *self, const char *sequence)
 * @memberof SelectorSequence
 */
static SelectorSequence *initWithSequence(SelectorSequence *self, const char *sequence) {

  self = (SelectorSequence *) super(Object, self, init);
  if (self) {

    self->sequence = strtrim(sequence);
    assert(self->sequence);

    assert(strlen(self->sequence));

    self->simpleSelectors = $$(SimpleSelector, parse, self->sequence);
    assert(self->simpleSelectors->count);
  }

  return self;
}

/**
 * @fn bool SelectorSequence::matchesView(const SelectorSequence *self, const View *view)
 * @memberof SelectorSequence
 */
static bool matchesView(const SelectorSequence *self, const View *view) {

  for (size_t i = 0; i < self->simpleSelectors->count; i++) {
    const SimpleSelector *simpleSelector = $(self->simpleSelectors, objectAtIndex, i);
    if ($(view, matchesSelector, simpleSelector) == false) {
      return false;
    }
  }

  return true;
}

/**
 * @return The SequenceCombinator for the given character.
 */
static SequenceCombinator sequenceCombinator(const char *c) {

  SequenceCombinator combinator = SequenceCombinatorNone;

  while (isspace(*c)) {
    combinator = SequenceCombinatorDescendent;
    c++;
  }

  switch (*c) {
    case '>':
      return SequenceCombinatorChild;
    case '~':
      return SequenceCombinatorSibling;
    case '+':
      return SequenceCombinatorAdjacent;
    case '\0':
      return SequenceCombinatorTerminal;
    default:
      break;
  }

  return combinator;
}

/**
 * @fn Array *SelectorSequence::parse(const char *rule)
 * @memberof SelectorSequence
 */
static Array *parse(const char *rule) {

  assert(rule);

  MutableArray *selectorSequences = $$(MutableArray, arrayWithCapacity, 8);
  assert(selectorSequences);

  SequenceCombinator left = SequenceCombinatorNone;

  const char *c = rule;
  while (*c) {
    const size_t size = strcspn(c, " \n\t>+~");
    if (size) {
      char *sequence = calloc(1, size + 1);
      assert(sequence);

      strncpy(sequence, c, size);

      SelectorSequence *selectorSequence = $(alloc(SelectorSequence), initWithSequence, sequence);
      assert(selectorSequence);

      selectorSequence->right = sequenceCombinator(c + size);
      assert(selectorSequence->right);

      selectorSequence->left = left;
      left = selectorSequence->right;

      $(selectorSequences, addObject, selectorSequence);

      release(selectorSequence);
      free(sequence);
    }

    c += size;
    c += strspn(c, " \n\t>+~");
  }

  return (Array *) selectorSequences;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  ((ObjectInterface *) clazz->interface)->description = description;

  ((SelectorSequenceInterface *) clazz->interface)->initWithSequence = initWithSequence;
  ((SelectorSequenceInterface *) clazz->interface)->matchesView = matchesView;
  ((SelectorSequenceInterface *) clazz->interface)->parse = parse;
}

/**
 * @fn Class *SelectorSequence::_SelectorSequence(void)
 * @memberof SelectorSequence
 */
Class *_SelectorSequence(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "SelectorSequence",
      .superclass = _Object(),
      .instanceSize = sizeof(SelectorSequence),
      .interfaceOffset = offsetof(SelectorSequence, interface),
      .interfaceSize = sizeof(SelectorSequenceInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
