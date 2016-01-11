/*
 * MVC_color.h
 *
 *  Created on: Nov 19, 2014
 *      Author: jdolan
 */


#ifndef _MVC_color_h_
#define _MVC_color_h_

#include <Objectively/Object.h>

#include <SDL2/SDL_pixels.h>

typedef struct MVC_Color MVC_Color;
typedef struct MVC_ColorInterface MVC_ColorInterface;

/**
 * @brief The MVC_Color type.
 */
struct MVC_Color {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	MVC_ColorInterface *interface;

	/**
	 * @brief The color.
	 */
	SDL_Color color;
};

/**
 * @brief The MVC_color type.
 */
struct MVC_ColorInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface parentInterface;

	MVC_Color *(*white);

	MVC_Color *(*black);

	MVC_Color *(*gray);

	MVC_Color *(transparent);
};

/**
 * @brief The MVC_Color Class.
 */
extern Class __MVC_Color;

#endif
