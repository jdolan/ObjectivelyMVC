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

#ifndef _ObjectivelyMVC_Font_h_
#define _ObjectivelyMVC_Font_h_

#include <fontconfig/fontconfig.h>

#include <SDL2/SDL_TTF.h>

#include <Objectively/Array.h>

#if defined(__APPLE__)
#define DEFAULT_FONT_FAMILY "Helvetica Neue"
#elif defined(__MINGW32__)
#define DEFAULT_FONT_FAMILY "Verdana"
#else
#define DEFAULT_FONT_FAMILY "Open Sans"
#endif

/**
 * @file
 * 
 * @brief TrueType fonts.
 */

typedef struct Font Font;
typedef struct FontInterface FontInterface;

/**
 * @brief The Font type.
 *
 * @extends Object
 */
struct Font {

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
	FontInterface *interface;
	
	/**
	 * @brief The backing TTF_Font.
	 */
	TTF_Font *font;
	
	/**
	 * @brief The TrueType font name, according to Fontconfig.
	 */
	char *name;
};

/**
 * @brief The Font interface.
 */
struct FontInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;
	
	/**
	 * @static
	 *
	 * @fn Array *Font::allFonts(void)
	 *
	 * @return An Array of all available Font names.
	 *
	 * @memberof Font
	 */
	Array *(*allFonts)(void);

	/**
	 * @static
	 *
	 * @fn Font *Font::defaultFont(void)
	 *
	 * @return The default Font.
	 *
	 * @memberof Font
	 */
	Font *(*defaultFont)(void);

	/**
	 * @fn Font *Font::initWithAttributes(Font *self, const char *family, int ptsize, int style)
	 *
	 * @brief Initializes this Font with the given attributes.
	 *
	 * @param family The font family.
	 * @param ptsize The point size.
	 * @param style The style (e.g. `TTF_STYLE_BOLD`).
	 *
	 * @return The initialized Font, or `NULL` on error.
	 *
	 * @memberof Font
	 */
	Font *(*initWithAttributes)(Font *self, const char *family, int ptsize, int style);

	/**
	 * @fn Font *Font::initWithName(Font *self, const char *name)
	 *
	 * @brief Initializes this Font with the given Fontconfig name.
	 *
	 * @param name The Fontconfig name.
	 *
	 * @return The initialized Font, or `NULL` on error.
	 *
	 * @memberof Font
	 */
	Font *(*initWithName)(Font *self, const char *name);
};

/**
 * @brief The Font Class.
 */
extern Class _Font;

#endif
