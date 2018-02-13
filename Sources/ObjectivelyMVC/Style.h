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

#include <Objectively/Enum.h>
#include <Objectively/Dictionary.h>

#include <ObjectivelyMVC/Selector.h>

/**
 * @file
 * @brief The Style type.
 */

typedef struct StyleInterface StyleInterface;

/**
 * @brief The Style type.
 * @extends Object
 * @ingroup Theme
 */
struct Style {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	StyleInterface *interface;

	/**
	 * The attributes.
	 */
	Dictionary *attributes;

	/**
	 * @brief The Selectors.
	 */
	Array *selectors;
};

/**
 * @brief The Style interface.
 */
struct StyleInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Style::addAttribute(Style *self, const char *attr, ident value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param value The attribute value.
	 * @remarks The `value` must be an Object and not a primitive.
	 * @memberof Style
	 */
	void (*addAttribute)(Style *self, const char *attr, ident value);

	/**
	 * @fn void Style::addAttributes(Style *self, const Dictionary *attributes)
	 * @brief Adds or replaces the attribtues in `attributes` to this Style.
	 * @param self The Style.
	 * @param attributes The attributes.
	 * @memberof Style
	 */
	void (*addAttributes)(Style *self, const Dictionary *attributes);

	/**
	 * @fn void Style::addBoolAttribute(Style *self, const char *attr, _Bool value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param value The attribute value.
	 * @memberof Style
	 */
	void (*addBoolAttribute)(Style *self, const char *attr, _Bool value);

	/**
	 * @fn void Style::addCharactersAttribute(Style *self, const char *attr, const char *value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param value The attribute value.
	 * @memberof Style
	 */
	void (*addCharactersAttribute)(Style *self, const char *attr, const char *value);

	/**
	 * @fn void Style::addColorAttribute(Style *self, const char *attr, const SDL_Color *value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param value The attribute value.
	 * @memberof Style
	 */
	void (*addColorAttribute)(Style *self, const char *attr, const SDL_Color *color);

	/**
	 * @fn void Style::addDoubleAttribute(Style *self, const char *attr, double value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param value The attribute value.
	 * @memberof Style
	 */
	void (*addDoubleAttribute)(Style *self, const char *attr, double value);

	/**
	 * @fn void Style::addEnumAttribute(Style *self, const char *attr, const EnumName *names, int value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param names The EnumNames from which to derrive the attribute value.
	 * @param value The attribute value.
	 * @memberof Style
	 */
	void (*addEnumAttribute)(Style *self, const char *attr, const EnumName *names, int value);

	/**
	 * @fn void Style::addFloatAttribute(Style *self, const char *attr, float value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param value The attribute value.
	 * @memberof Style
	 */
	void (*addFloatAttribute)(Style *self, const char *attr, float value);

	/**
	 * @fn void Style::addIntegerAttribute(Style *self, const char *attr, int value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param value The attribute value.
	 * @memberof Style
	 */
	void (*addIntegerAttribute)(Style *self, const char *attr, int value);

	/**
	 * @fn void Style::addPointAttribute(Style *self, const char *attr, const SDL_Point *value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param value The attribute value.
	 * @memberof Style
	 */
	void (*addPointAttribute)(Style *self, const char *attr, const SDL_Point *value);

	/**
	 * @fn void Style::addRectangleAttribute(Style *self, const char *attr, const SDL_Rect *value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param value The attribute value.
	 * @memberof Style
	 */
	void (*addRectangleAttribute)(Style *self, const char *attr, const SDL_Rect *value);

	/**
	 * @fn void Style::addSelector(Style *self, Selector *selector)
	 * @brief Adds the given Selector to this Style.
	 * @param self The Style.
	 * @param selector The Selector.
	 * @memberof Style
	 */
	void (*addSelector)(Style *self, Selector *selector);

	/**
	 * @fn void Style::addSizeAttribute(Style *self, const char *attr, const SDL_Size *value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param value The attribute value.
	 * @memberof Style
	 */
	void (*addSizeAttribute)(Style *self, const char *attr, const SDL_Size *value);

	/**
	 * @fn ident Style::attributeValue(const Style *self, const char *attr)
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @return The attribute value.
	 * @memberof Style
	 */
	ident (*attributeValue)(const Style *self, const char *attr);

	/**
	 * @fn Style *Style::initWithAttributes(Style *self, const Dictionary *attributes)
	 * @brief Initializes this Style with the given attributes.
	 * @param self The Style.
	 * @param attributes The attributes.
	 * @return The initialized Style, or `NULL` on error.
	 * @see View::awakeWithDictionary(View *, const Dictionary *)
	 * @memberof Style
	 */
	Style *(*initWithAttributes)(Style *self, const Dictionary *attributes);

	/**
	 * @fn Style *Style::initWithRules(Style *self, const char *rules)
	 * @brief Initializes this Style with the given CSS selector rules.
	 * @param self The Style.
	 * @param rules A null-terminated C string of one or more Selector rules.
	 * @return The initialized Style, or `NULL` on error.
	 * @see Selector::parse(const char *)
	 * @memberof Style
	 */
	Style *(*initWithRules)(Style *self, const char *rules);

	/**
	 * @fn _Bool Style::isComputedEqual(const Style *self, const Style *other)
	 * @brief Performs a fast, rule-based comparison of this Style to the given Style.
	 * @param self The Style.
	 * @param other The Style to test for computed equality.
	 * @return True if the given Style is deemed equal, false otherwise.
	 * @remarks This method is optimized to quickly determine if two computed Styles should contain
	 * the same attributes based on their Selector rules. For true equality, use `isEqual`.
	 * @memberof Style
	 */
	_Bool (*isComputedEqual)(const Style *self, const Style *other);

	/**
	 * @static
	 * @fn Array *Style::parse(const char *css)
	 * @brief Parses the null-terminated C string of CSS definitions into an Array of Styles.
	 * @param css The CSS definitions.
	 * @return An Array of Styles.
	 * @memberof Style
	 */
	Array *(*parse)(const char *css);

	/**
	 * @fn void Style::removeAllAttributes(Style *self)
	 * @brief Removes all attributes from this Style.
	 * @param self The Style.
	 * @memberof Style
	 */
	void (*removeAllAttributes)(Style *self);

	/**
	 * @fn void Style::removeAttribute(Style *self, const char *attr)
	 * @brief Removes the attribute with the given name.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @memberof Style
	 */
	void (*removeAttribute)(Style *self, const char *attr);
};

/**
 * @fn Class *Style::_Style(void)
 * @brief The Style archetype.
 * @return The Style Class.
 * @memberof Style
 */
OBJECTIVELYMVC_EXPORT Class *_Style(void);
