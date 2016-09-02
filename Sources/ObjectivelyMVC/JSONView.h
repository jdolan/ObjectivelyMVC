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

#include <Objectively/Data.h>
#include <Objectively/JSONPath.h>

#include <ObjectivelyMVC/View.h>

/**
 * @file
 *
 * @brief The JSONView type
 */

typedef struct JSONView JSONView;
typedef struct JSONViewInterface JSONViewInterface;

/**
 * @brief JSONView allows loading of View hierarchies from `.json` files.
 *
 * @extends Object
 */
struct JSONView {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Object object;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	JSONViewInterface *interface;
};

/**
 * @brief The JSONView interface.
 */
struct JSONViewInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 *
	 * @fn SDL_Color JSONView::colorWithPath(const ident root, const char *path)
	 *
	 * @brief Parses an SDL_Color from the nested JSON property.
	 *
	 * @param root The root element.
	 * @param path A JSONPath expression.
	 *
	 * @return The parsed SDL_Color, or `Colors.Clear` if unspecified.
	 *
	 * memberof JSONView
	 */
	SDL_Color (*colorWithPath)(const ident root, const char *path);

	/**
	 * @static
	 *
	 * @fn Padding JSONView::paddingWithPath(const ident root, const char *path)
	 *
	 * @brief Parses a Padding from the nested JSON property.
	 *
	 * @param root The root element.
	 * @param path A JSONPath expression.
	 *
	 * @return The parsed Padding, which will be empty if unspecified.
	 *
	 * memberof JSONView
	 */
	Padding (*paddingWithPath)(const ident root, const char *path);

	/**
	 * @static
	 *
	 * @fn SDL_Rect JSONView::rectWithPath(const ident root, const char *path)
	 *
	 * @brief Parses an SDL_Rect from the nested JSON property.
	 *
	 * @param root The root element.
	 * @param path A JSONPath expression.
	 *
	 * @return The parsed SDL_Rect, which will be empty if unspecified.
	 *
	 * memberof JSONView
	 */
	SDL_Rect (*rectWithPath)(const ident root, const char *path);

	/**
	 * @static
	 *
	 * @fn View *JSONView::viewWithContentsOfFile(const char *path)
	 *
	 * @brief Instantiates a View initialized with the contents of the JSON file at `path`.
	 *
	 * @param path A path to a JSON file describing a View.
	 * @param outlets An optional array of Outlets to resolve.
	 *
	 * @return The initialized View, or `NULL` on error.
	 *
	 * @memberof JSONView
	 */
	View *(*viewWithContentsOfFile)(const char *path, Outlet *outlets);

	/**
	 * @static
	 *
	 * @fn View *JSONView::viewWithData(const Data *data)
	 *
	 * @brief Instantiates a View initialized with the contents of `data`.
	 *
	 * @param dictionary A Dictionary describing the View.
	 * @param outlets An optional array of Outlets to resolve.
	 *
	 * @return The initialized View, or `NULL` on error.
	 *
	 * @memberof JSONView
	 */
	View *(*viewWithData)(const Data *data, Outlet *outlets);

	/**
	 * @static
	 *
	 * @fn View *JSONView::viewWithDictionary(const Dictionary *dictionary, Outlet *outlets)
	 *
	 * @brief Instantiates a View initialized with the attributes described in `dictionary`.
	 *
	 * @param dictionary A Dictionary describing the View.
	 * @param outlets An optional array of Outlets to resolve.
	 *
	 * @return The initialized View, or `NULL` on error.
	 *
	 * @see View::initWithDictionary(View *, const Dictionary *, Outlet *outlets)
	 *
	 * @memberof JSONView
	 */
	View *(*viewWithDictionary)(const Dictionary *dictionary, Outlet *outlets);
};

/**
 * @brief The JSONView Class.
 */
extern Class _JSONView;

