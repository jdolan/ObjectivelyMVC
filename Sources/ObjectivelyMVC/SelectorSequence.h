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

#include <Objectively/Array.h>

#include <ObjectivelyMVC/SimpleSelector.h>

/**
 * @file
 * @brief The SeletorSequence type.
 */

/**
 * @brief Combinators are operators for chaining SelectorSequences.
 */
typedef enum {
	SequenceCombinatorNone,
	SequenceCombinatorDescendent,
	SequenceCombinatorChild,
	SequenceCombinatorSibling,
	SequenceCombinatorAdjacent,
	SequenceCombinatorTerminal
} SequenceCombinator;

typedef struct SelectorSequence SelectorSequence;
typedef struct SelectorSequenceInterface SelectorSequenceInterface;

/**
 * @brief SelectorSequences are comprised of one or more SimpleSelectors.
 * @extends Object
 * @ingroup Theme
 */
struct SelectorSequence {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	SelectorSequenceInterface *interface;

	/**
	 * @brief The combinator.
	 */
	SequenceCombinator combinator;

	/**
	 * @brief The SimpleSelectors comprising this SelectorSequence.
	 */
	Array *simpleSelectors;

	/**
	 * @brief The sequence, as provided by the user.
	 */
	char *sequence;
};

/**
 * @brief The SelectorSequence interface.
 */
struct SelectorSequenceInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn SelectorSequence *SelectorSequence::initWithSequence(SelectorSequence *self, const char *sequence)
	 * @brief Initializes this SelectorSequence with the given sequence.
	 * @param self The SelectorSequence.
	 * @param sequence A C-string specifying a sequence of SimpleSelectors.
	 * @return The initialized SelectorSequence, or `NULL` on error.
	 * @memberof SelectorSequence
	 */
	SelectorSequence *(*initWithSequence)(SelectorSequence *self, const char *sequence);

	/**
	 * @fn _Bool SelectorSequence::matchesView(const SelectorSequence *self, const View *view)
	 * @param self The SelectorSequence.
	 * @param view The View.
	 * @return True if all SimpleSelectors in this SelectorSequence match the given View, false otherwise.
	 * @memberof SelectorSequence
	 */
	_Bool (*matchesView)(const SelectorSequence *self, const View *view);

	/**
	 * @static
	 * @fn Array *SelectorSequence::parse(const char *rule)
	 * @param rule A C-string specifying a Selector rule.
	 * @return The parsed SelectorSequences.
	 * @memberof SelectorSequence
	 */
	Array *(*parse)(const char *rule);
};

/**
 * @fn Class *SelectorSequence::_SelectorSequence(void)
 * @brief The SelectorSequence archetype.
 * @return The SelectorSequence Class.
 * @memberof SelectorSequence
 */
OBJECTIVELYMVC_EXPORT Class *_SelectorSequence(void);
