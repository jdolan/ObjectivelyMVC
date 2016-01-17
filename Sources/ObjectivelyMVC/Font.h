/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _ObjectivelyMVC_Font_h_
#define _ObjectivelyMVC_Font_h_

#include <fontconfig/fontconfig.h>
#include <SDL2/SDL_TTF.h>

#include <Objectively/Object.h>

/**
 * @file
 * 
 * @brief ..
 */

typedef struct Font Font;
typedef struct FontInterface FontInterface;

/**
 * @brief ..
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
	 * @fn Font *Font::initWithPattern(Font *self, const char *pattern)
	 *
	 * @brief Initializes this Font with the given pattern string.
	 *
	 * @return The initialized Font, or `NULL` on error.
	 *
	 * @memberof Font
	 */
	Font *(*initWithPattern)(Font *self, const char *pattern);
};

/**
 * @brief The Font Class.
 */
extern Class _Font;

#endif
