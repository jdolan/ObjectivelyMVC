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

#include <SDL2/SDL_pixels.h>

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
	
	SDL_Color DefaultColor;
	SDL_Color HighlightedColor;
	SDL_Color DisabledColor;
	SDL_Color SelectedColor;
	SDL_Color FocusedColor;

	SDL_Color AlternateColor;
	
	SDL_Color Clear;
	
	SDL_Color AliceBlue;
	SDL_Color AntiqueWhite;
	SDL_Color Aqua;
	SDL_Color Aquamarine;
	SDL_Color Azure;
	SDL_Color Beige;
	SDL_Color Bisque;
	SDL_Color Black;
	SDL_Color BlanchedAlmond;
	SDL_Color Blue;
	SDL_Color BlueViolet;
	SDL_Color Brown;
	SDL_Color BurlyWood;
	SDL_Color CadetBlue;
	SDL_Color Charcoal;
	SDL_Color Chartreuse;
	SDL_Color Chocolate;
	SDL_Color Coral;
	SDL_Color CornflowerBlue;
	SDL_Color Cornsilk;
	SDL_Color Crimson;
	SDL_Color Cyan;
	SDL_Color DarkBlue;
	SDL_Color DarkCyan;
	SDL_Color DarkGoldenRod;
	SDL_Color DarkGray;
	SDL_Color DarkGrey;
	SDL_Color DarkGreen;
	SDL_Color DarkKhaki;
	SDL_Color DarkMagenta;
	SDL_Color DarkOliveGreen;
	SDL_Color DarkOrange;
	SDL_Color DarkOrchid;
	SDL_Color DarkRed;
	SDL_Color DarkSalmon;
	SDL_Color DarkSeaGreen;
	SDL_Color DarkSlateBlue;
	SDL_Color DarkSlateGray;
	SDL_Color DarkSlateGrey;
	SDL_Color DarkTurquoise;
	SDL_Color DarkViolet;
	SDL_Color DeepPink;
	SDL_Color DeepSkyBlue;
	SDL_Color DimGray;
	SDL_Color DimGrey;
	SDL_Color DodgerBlue;
	SDL_Color FireBrick;
	SDL_Color FloralWhite;
	SDL_Color ForestGreen;
	SDL_Color Fuchsia;
	SDL_Color Gainsboro;
	SDL_Color GhostWhite;
	SDL_Color Gold;
	SDL_Color GoldenRod;
	SDL_Color Gray;
	SDL_Color Grey;
	SDL_Color Green;
	SDL_Color GreenYellow;
	SDL_Color HoneyDew;
	SDL_Color HotPink;
	SDL_Color IndianRed;
	SDL_Color Indigo;
	SDL_Color Ivory;
	SDL_Color Khaki;
	SDL_Color Lavender;
	SDL_Color LavenderBlush;
	SDL_Color LawnGreen;
	SDL_Color LemonChiffon;
	SDL_Color LightBlue;
	SDL_Color LightCoral;
	SDL_Color LightCyan;
	SDL_Color LightGoldenRodYellow;
	SDL_Color LightGray;
	SDL_Color LightGrey;
	SDL_Color LightGreen;
	SDL_Color LightPink;
	SDL_Color LightSalmon;
	SDL_Color LightSeaGreen;
	SDL_Color LightSkyBlue;
	SDL_Color LightSlateGray;
	SDL_Color LightSlateGrey;
	SDL_Color LightSteelBlue;
	SDL_Color LightYellow;
	SDL_Color Lime;
	SDL_Color LimeGreen;
	SDL_Color Linen;
	SDL_Color Magenta;
	SDL_Color Maroon;
	SDL_Color MediumAquaMarine;
	SDL_Color MediumBlue;
	SDL_Color MediumOrchid;
	SDL_Color MediumPurple;
	SDL_Color MediumSeaGreen;
	SDL_Color MediumSlateBlue;
	SDL_Color MediumSpringGreen;
	SDL_Color MediumTurquoise;
	SDL_Color MediumVioletRed;
	SDL_Color MidnightBlue;
	SDL_Color MintCream;
	SDL_Color MistyRose;
	SDL_Color Moccasin;
	SDL_Color NavajoWhite;
	SDL_Color Navy;
	SDL_Color OldLace;
	SDL_Color Olive;
	SDL_Color OliveDrab;
	SDL_Color Orange;
	SDL_Color OrangeRed;
	SDL_Color Orchid;
	SDL_Color PaleGoldenRod;
	SDL_Color PaleGreen;
	SDL_Color PaleTurquoise;
	SDL_Color PaleVioletRed;
	SDL_Color PapayaWhip;
	SDL_Color PeachPuff;
	SDL_Color Peru;
	SDL_Color Pink;
	SDL_Color Plum;
	SDL_Color PowderBlue;
	SDL_Color Purple;
	SDL_Color RebeccaPurple;
	SDL_Color Red;
	SDL_Color RosyBrown;
	SDL_Color RoyalBlue;
	SDL_Color SaddleBrown;
	SDL_Color Salmon;
	SDL_Color SandyBrown;
	SDL_Color SeaGreen;
	SDL_Color SeaShell;
	SDL_Color Sienna;
	SDL_Color Silver;
	SDL_Color SkyBlue;
	SDL_Color SlateBlue;
	SDL_Color SlateGray;
	SDL_Color SlateGrey;
	SDL_Color Snow;
	SDL_Color SpringGreen;
	SDL_Color SteelBlue;
	SDL_Color Tan;
	SDL_Color Teal;
	SDL_Color Thistle;
	SDL_Color Tomato;
	SDL_Color Turquoise;
	SDL_Color Violet;
	SDL_Color Wheat;
	SDL_Color White;
	SDL_Color WhiteSmoke;
	SDL_Color Yellow;
	SDL_Color YellowGreen;
} Colors;
