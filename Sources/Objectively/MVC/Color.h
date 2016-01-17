/*
 * Color.h
 *
 *  Created on: Nov 19, 2014
 *      Author: jdolan
 */


#ifndef _ObjectivelyMVC_Color_h_
#define _ObjectivelyMVC_Color_h_

#include <Objectively/Object.h>

#include <SDL2/SDL_pixels.h>

typedef struct Color Color;
typedef struct ColorInterface ColorInterface;

/**
 * @brief The Color type.
 */
struct Color {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	ColorInterface *interface;

	/**
	 * @brief The color.
	 */
	SDL_Color color;
};

/**
 * @brief The color type.
 */
struct ColorInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface parentInterface;
	
	/**
	 * @static
	 *
	 * @fn Color *Color::black(void)
	 *
	 * @return The color black.
	 *
	 * @memberof Color
	 */
	Color *(*black)(void);
	
	/**
	 * @static
	 *
	 * @fn Color *Color::gray(void)
	 *
	 * @return The color gray.
	 *
	 * @memberof Color
	 */
	Color *(*gray)(void);
	
	/**
	 * @fn Color *Color::initWithComponents(Color *self, byte r, byte g, byte b, byte a)
	 *
	 * @brief Initializes this Color with the given components.
	 *
	 * @param r The red component.
	 * @param g The green component.
	 * @param b The blue component.
	 * @param a The alpha component.
	 *
	 * @return The initialiezd Color, or `NULL` on error.
	 *
	 * @memberof Color
	 */
	Color *(*initWithComponents)(Color *self, byte r, byte g, byte b, byte a);

	/**
	 * @static
	 *
	 * @fn Color *Color::transparent(void)
	 *
	 * @return The color transparent.
	 *
	 * @memberof Color
	 */
	Color *(*transparent)(void);
	
	/**
	 * @static
	 *
	 * @fn Color *Color::white(void)
	 *
	 * @return The color white.
	 *
	 * @memberof Color
	 */
	Color *(*white)(void);
};

/**
 * @brief The Color Class.
 */
extern Class _Color;

#endif
