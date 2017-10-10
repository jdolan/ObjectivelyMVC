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

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief W3C Color constants.
 */

/**
 * @brief W3C Color constants.
 * @see http://www.w3schools.com/colors/colors_names.asp
 */
OBJECTIVELYMVC_EXPORT const struct _Colors {

	SDL_Color aliceBlue;
	SDL_Color antiqueWhite;
	SDL_Color aqua;
	SDL_Color aquamarine;
	SDL_Color azure;
	SDL_Color beige;
	SDL_Color bisque;
	SDL_Color black;
	SDL_Color blanchedAlmond;
	SDL_Color blue;
	SDL_Color blueViolet;
	SDL_Color brown;
	SDL_Color burlyWood;
	SDL_Color cadetBlue;
	SDL_Color charcoal;
	SDL_Color chartreuse;
	SDL_Color chocolate;
	SDL_Color coral;
	SDL_Color cornflowerBlue;
	SDL_Color cornsilk;
	SDL_Color crimson;
	SDL_Color cyan;
	SDL_Color darkBlue;
	SDL_Color darkCyan;
	SDL_Color darkGoldenRod;
	SDL_Color darkGray;
	SDL_Color darkGrey;
	SDL_Color darkGreen;
	SDL_Color darkKhaki;
	SDL_Color darkMagenta;
	SDL_Color darkOliveGreen;
	SDL_Color darkOrange;
	SDL_Color darkOrchid;
	SDL_Color darkRed;
	SDL_Color darkSalmon;
	SDL_Color darkSeaGreen;
	SDL_Color darkSlateBlue;
	SDL_Color darkSlateGray;
	SDL_Color darkSlateGrey;
	SDL_Color darkTurquoise;
	SDL_Color darkViolet;
	SDL_Color deepPink;
	SDL_Color deepSkyBlue;
	SDL_Color dimGray;
	SDL_Color dimGrey;
	SDL_Color dodgerBlue;
	SDL_Color fireBrick;
	SDL_Color floralWhite;
	SDL_Color forestGreen;
	SDL_Color fuchsia;
	SDL_Color gainsboro;
	SDL_Color ghostWhite;
	SDL_Color gold;
	SDL_Color goldenRod;
	SDL_Color gray;
	SDL_Color grey;
	SDL_Color green;
	SDL_Color greenYellow;
	SDL_Color honeyDew;
	SDL_Color hotPink;
	SDL_Color indianRed;
	SDL_Color indigo;
	SDL_Color ivory;
	SDL_Color khaki;
	SDL_Color lavender;
	SDL_Color lavenderBlush;
	SDL_Color lawnGreen;
	SDL_Color lemonChiffon;
	SDL_Color lightBlue;
	SDL_Color lightCoral;
	SDL_Color lightCyan;
	SDL_Color lightGoldenRodYellow;
	SDL_Color lightGray;
	SDL_Color lightGrey;
	SDL_Color lightGreen;
	SDL_Color lightPink;
	SDL_Color lightSalmon;
	SDL_Color lightSeaGreen;
	SDL_Color lightSkyBlue;
	SDL_Color lightSlateGray;
	SDL_Color lightSlateGrey;
	SDL_Color lightSteelBlue;
	SDL_Color lightYellow;
	SDL_Color lime;
	SDL_Color limeGreen;
	SDL_Color linen;
	SDL_Color magenta;
	SDL_Color maroon;
	SDL_Color mediumAquaMarine;
	SDL_Color mediumBlue;
	SDL_Color mediumOrchid;
	SDL_Color mediumPurple;
	SDL_Color mediumSeaGreen;
	SDL_Color mediumSlateBlue;
	SDL_Color mediumSpringGreen;
	SDL_Color mediumTurquoise;
	SDL_Color mediumVioletRed;
	SDL_Color midnightBlue;
	SDL_Color mintCream;
	SDL_Color mistyRose;
	SDL_Color moccasin;
	SDL_Color navajoWhite;
	SDL_Color navy;
	SDL_Color oldLace;
	SDL_Color olive;
	SDL_Color oliveDrab;
	SDL_Color orange;
	SDL_Color orangeRed;
	SDL_Color orchid;
	SDL_Color paleGoldenRod;
	SDL_Color paleGreen;
	SDL_Color paleTurquoise;
	SDL_Color paleVioletRed;
	SDL_Color papayaWhip;
	SDL_Color peachPuff;
	SDL_Color peru;
	SDL_Color pink;
	SDL_Color plum;
	SDL_Color powderBlue;
	SDL_Color purple;
	SDL_Color rebeccaPurple;
	SDL_Color red;
	SDL_Color rosyBrown;
	SDL_Color royalBlue;
	SDL_Color saddleBrown;
	SDL_Color salmon;
	SDL_Color sandyBrown;
	SDL_Color seaGreen;
	SDL_Color seaShell;
	SDL_Color sienna;
	SDL_Color silver;
	SDL_Color skyBlue;
	SDL_Color slateBlue;
	SDL_Color slateGray;
	SDL_Color slateGrey;
	SDL_Color snow;
	SDL_Color springGreen;
	SDL_Color steelBlue;
	SDL_Color tan;
	SDL_Color teal;
	SDL_Color thistle;
	SDL_Color tomato;
	SDL_Color transparent;
	SDL_Color turquoise;
	SDL_Color violet;
	SDL_Color wheat;
	SDL_Color white;
	SDL_Color whiteSmoke;
	SDL_Color yellow;
	SDL_Color yellowGreen;
} Colors;

/**
 * @brief Creates an SDL_Color with the given components.
 */
#define MakeColor(r, g, b, a) (SDL_Color) { (r), (g), (b), (a) }

/**
 * @brief Converts the given hexadecimal color string to an RGBA color.
 * @param hexString The hexadecimal color string (e.g. `deadbeef`).
 * @return An SDL_Color containing the byte-clamped RGBA values.
 */
OBJECTIVELYMVC_EXPORT SDL_Color MVC_HexToRGBA(const char *hexString);

/**
 * @param color The RGB color.
 * @return The hexadecimal color string (e.g. `deadbeef`).
 * @remarks This function uses static memory and is not thread safe.
 */
OBJECTIVELYMVC_EXPORT char *MVC_RGBToHex(const SDL_Color *color);

/**
 * @param color The RGBA color.
 * @return The hexadecimal color string (e.g. `deadbeef`).
 * @remarks This function uses static memory and is not thread safe.
 */
OBJECTIVELYMVC_EXPORT char *MVC_RGBAToHex(const SDL_Color *color);

/**
 * @brief Converts the given HSV components to an RGB color.
 * @param hue The hue component (0.0 - 360.0).
 * @param saturation The saturation component (0.0 - 1.0).
 * @param value The value component (0.0 - 1.0).
 * @return An SDL_Color containing the byte-clamped RGB values.
 */
OBJECTIVELYMVC_EXPORT SDL_Color MVC_HSVToRGB(double hue, double saturation, double value);

/**
 * @brief Converts the given RGB color to HSV components.
 * @param color The RGB color.
 * @param *hue The hue component.
 * @param saturation The saturation component.
 * @param value The value component.
 */
OBJECTIVELYMVC_EXPORT void MVC_RGBToHSV(const SDL_Color *color, double *hue, double *saturation, double *value);
