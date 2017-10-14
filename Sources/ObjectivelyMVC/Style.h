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
#include <Objectively/MutableDictionary.h>

#include <ObjectivelyMVC/Font.h>
#include <ObjectivelyMVC/Image.h>
#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief The Style type.
 */

typedef struct Style Style;
typedef struct StyleInterface StyleInterface;

/**
 * @brief The Style type.
 * @extends Object
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
	 * @see View+JSON.h
	 */
	MutableDictionary *attributes;

	/**
	 * @brief The selector, e.g. `"Control:highlighted"`.
	 */
	String *selector;
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
	 * @fn void Style::addSizeAttribute(Style *self, const char *attr, const SDL_Size *value)
	 * @brief Adds or replaces the given attribute with `value`.
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @param value The attribute value.
	 * @memberof Style
	 */
	void (*addSizeAttribute)(Style *self, const char *attr, const SDL_Size *value);

	/**
	 * @fn ident Style::attributeValue(const Style *self, const char *attribute)
	 * @param self The Style.
	 * @param attr The attribute name.
	 * @return The attribute value.
	 * @memberof Style
	 */
	ident (*attributeValue)(const Style *self, const char *attr);

	/**
	 * @fn Style *Style::initWithSelector(Style *self, const char *selector, const Dictionary *attributes)
	 * @brief Initializes this Style with the given selector and attributes.
	 * @param self The Style.
	 * @param selector The selector.
	 * @param attributes The attributes, or `NULL` to create an empty Style.
	 * @return The initialized Style, or `NULL` on error.
	 * @memberof Style
	 */
	Style *(*initWithSelector)(Style *self, const char *selector, const Dictionary *attributes);

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
OBJECTIVELY_EXPORT Class *_Style(void);
