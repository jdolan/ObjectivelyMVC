/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented, you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>
#include <math.h>
#include <string.h>

#include <ObjectivelyMVC/Colors.h>
#include <SDL2/SDL_endian.h>

const struct _Colors Colors = {
	.aliceBlue = { 240, 248, 255, 255 },
	.antiqueWhite = { 250, 235, 215, 255 },
	.aqua = { 0, 255, 255, 255 },
	.aquamarine = { 127, 255, 212, 255 },
	.azure = { 240, 255, 255, 255 },
	.beige = { 245, 245, 220, 255 },
	.bisque = { 255, 228, 196, 255 },
	.black = { 0, 0, 0, 255 },
	.blanchedAlmond = { 255, 235, 205, 255 },
	.blue = { 0, 0, 255, 255 },
	.blueViolet = { 138, 43, 226, 255 },
	.brown = { 165, 42, 42, 255 },
	.burlyWood = { 222, 184, 135, 255 },
	.cadetBlue = { 95, 158, 160, 255 },
	.charcoal = { 48, 48, 48, 255 },
	.chartreuse = { 127, 255, 0, 255 },
	.chocolate = { 210, 105, 30, 255 },
	.coral = { 255, 127, 80, 255 },
	.cornflowerBlue = { 100, 149, 237, 255 },
	.cornsilk = { 255, 248, 220, 255 },
	.crimson = { 220, 20, 60, 255 },
	.cyan = { 0, 255, 255, 255 },
	.darkBlue = { 0, 0, 139, 255 },
	.darkCyan = { 139, 139, 255, 255 },
	.darkGoldenRod = { 184, 134, 11, 255 },
	.darkGray = { 169, 169, 169, 255 },
	.darkGrey = { 169, 169, 169, 255 },
	.darkGreen = { 0, 100, 0, 255 },
	.darkKhaki = { 189, 183, 107, 255 },
	.darkMagenta = { 139, 0, 139, 255 },
	.darkOliveGreen = { 85, 107, 47, 255 },
	.darkOrange = { 255, 140, 0, 255 },
	.darkOrchid = { 153, 50, 204, 255 },
	.darkRed = { 139, 0, 0, 255 },
	.darkSalmon = { 233, 150, 122, 255 },
	.darkSeaGreen = { 143, 188, 143, 255 },
	.darkSlateBlue = { 72, 61, 139, 255 },
	.darkSlateGray = { 47, 79, 79, 255 },
	.darkSlateGrey = { 47, 79, 79, 255 },
	.darkTurquoise = { 206, 209, 255, 255 },
	.darkViolet = { 148, 0, 211, 255 },
	.deepPink = { 255, 20, 147, 255 },
	.deepSkyBlue = { 191, 255, 255, 255 },
	.dimGray = { 105, 105, 105, 255 },
	.dimGrey = { 105, 105, 105, 255 },
	.dodgerBlue = { 30, 144, 255, 255 },
	.fireBrick = { 178, 34, 34, 255 },
	.floralWhite = { 255, 250, 240, 255 },
	.forestGreen = { 34, 139, 34, 255 },
	.fuchsia = { 255, 0, 255, 255 },
	.gainsboro = { 220, 220, 220, 255 },
	.ghostWhite = { 248, 248, 255, 255 },
	.gold = { 255, 215, 0, 255 },
	.goldenRod = { 218, 165, 32, 255 },
	.gray = { 128, 128, 128, 255 },
	.grey = { 128, 128, 128, 255 },
	.green = { 0, 128, 0, 255 },
	.greenYellow = { 173, 255, 47, 255 },
	.honeyDew = { 240, 255, 240, 255 },
	.hotPink = { 255, 105, 180, 255 },
	.indianRed = { 205, 92, 92, 255 },
	.indigo = { 75, 0, 130, 255 },
	.ivory = { 255, 255, 240, 255 },
	.khaki = { 240, 230, 140, 255 },
	.lavender = { 230, 230, 250, 255 },
	.lavenderBlush = { 255, 240, 245, 255 },
	.lawnGreen = { 124, 252, 0, 255 },
	.lemonChiffon = { 255, 250, 205, 255 },
	.lightBlue = { 173, 216, 230, 255 },
	.lightCoral = { 240, 128, 128, 255 },
	.lightCyan = { 224, 255, 255, 255 },
	.lightGoldenRodYellow = { 250, 250, 210, 255 },
	.lightGray = { 211, 211, 211, 255 },
	.lightGrey = { 211, 211, 211, 255 },
	.lightGreen = { 144, 238, 144, 255 },
	.lightPink = { 255, 182, 193, 255 },
	.lightSalmon = { 255, 160, 122, 255 },
	.lightSeaGreen = { 32, 178, 170, 255 },
	.lightSkyBlue = { 135, 206, 250, 255 },
	.lightSlateGray = { 119, 136, 153, 255 },
	.lightSlateGrey = { 119, 136, 153, 255 },
	.lightSteelBlue = { 176, 196, 222, 255 },
	.lightYellow = { 255, 255, 224, 255 },
	.lime = { 0, 255, 0, 255 },
	.limeGreen = { 50, 205, 50, 255 },
	.linen = { 250, 240, 230, 255 },
	.magenta = { 255, 0, 255, 255 },
	.maroon = { 128, 0, 0, 255 },
	.mediumAquaMarine = { 102, 205, 170, 255 },
	.mediumBlue = { 0, 0, 205, 255 },
	.mediumOrchid = { 186, 85, 211, 255 },
	.mediumPurple = { 147, 112, 219, 255 },
	.mediumSeaGreen = { 60, 179, 113, 255 },
	.mediumSlateBlue = { 123, 104, 238, 255 },
	.mediumSpringGreen = { 250, 154, 255, 255 },
	.mediumTurquoise = { 72, 209, 204, 255 },
	.mediumVioletRed = { 199, 21, 133, 255 },
	.midnightBlue = { 25, 25, 112, 255 },
	.mintCream = { 245, 255, 250, 255 },
	.mistyRose = { 255, 228, 225, 255 },
	.moccasin = { 255, 228, 181, 255 },
	.navajoWhite = { 255, 222, 173, 255 },
	.navy = { 0, 0, 128, 255 },
	.oldLace = { 253, 245, 230, 255 },
	.olive = { 128, 128, 0, 255 },
	.oliveDrab = { 107, 142, 35, 255 },
	.orange = { 255, 165, 0, 255 },
	.orangeRed = { 255, 69, 0, 255 },
	.orchid = { 218, 112, 214, 255 },
	.paleGoldenRod = { 238, 232, 170, 255 },
	.paleGreen = { 152, 251, 152, 255 },
	.paleTurquoise = { 175, 238, 238, 255 },
	.paleVioletRed = { 219, 112, 147, 255 },
	.papayaWhip = { 255, 239, 213, 255 },
	.peachPuff = { 255, 218, 185, 255 },
	.peru = { 205, 133, 63, 255 },
	.pink = { 255, 192, 203, 255 },
	.plum = { 221, 160, 221, 255 },
	.powderBlue = { 176, 224, 230, 255 },
	.purple = { 128, 0, 128, 255 },
	.rebeccaPurple = { 102, 51, 153, 255 },
	.red = { 255, 0, 0, 255 },
	.rosyBrown = { 188, 143, 143, 255 },
	.royalBlue = { 65, 105, 225, 255 },
	.saddleBrown = { 139, 69, 19, 255 },
	.salmon = { 250, 128, 114, 255 },
	.sandyBrown = { 244, 164, 96, 255 },
	.seaGreen = { 46, 139, 87, 255 },
	.seaShell = { 255, 245, 238, 255 },
	.sienna = { 160, 82, 45, 255 },
	.silver = { 192, 192, 192, 255 },
	.skyBlue = { 135, 206, 235, 255 },
	.slateBlue = { 106, 90, 205, 255 },
	.slateGray = { 112, 128, 144, 255 },
	.slateGrey = { 112, 128, 144, 255 },
	.snow = { 255, 250, 250, 255 },
	.springGreen = { 0, 255, 127, 255 },
	.steelBlue = { 70, 130, 180, 255 },
	.tan = { 210, 180, 140, 255 },
	.teal = { 0, 128, 128, 255 },
	.thistle = { 216, 191, 216, 255 },
	.tomato = { 255, 99, 71, 255 },
	.transparent = { 255, 255, 255, 0 },
	.turquoise = { 64, 224, 208, 255 },
	.violet = { 238, 130, 238, 255 },
	.wheat = { 245, 222, 179, 255 },
	.white = { 255, 255, 255, 255 },
	.whiteSmoke = { 245, 245, 245, 255 },
	.yellow = { 255, 255, 0, 255 },
	.yellowGreen = { 154, 205, 50, 255 },
};

SDL_Color MVC_HexToRGBA(const char *hex) {
	static char buffer[9];

	union {
		SDL_Color c;
		unsigned int u;
	} color;

	color.u = 0;

	const size_t length = strlen(hex);
	if (length != 6 && length != 8) {
		return color.c;
	}

	strcpy(buffer, hex);

	if (length == 6) {
		strcat(buffer, "ff");
	}

	if (sscanf(buffer, "%x", &color.u) != 1) {
		return color.c;
	}

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	color.u = (((color.u & 0xff000000) >> 24) | ((color.u & 0x00ff0000) >> 8) | ((color.u & 0x0000ff00) << 8) | ((color.u & 0x000000ff) << 24));
#endif

	return color.c;
}

char *MVC_RGBToHex(const SDL_Color *color) {
	return MVC_RGBAToHex(&(const SDL_Color) { .r = color->r, .g = color->g, .b = color->b, .a = 255 });
}

char *MVC_RGBAToHex(const SDL_Color *color) {
	static char hex[9];

	snprintf(hex, sizeof(hex), "%02x%02x%02x%02x", color->r, color->g, color->b, color->a);

	return hex;
}

SDL_Color MVC_HSVToRGB(double hue, double saturation, double value) {

	SDL_Color color = { .a = 255 };

	const double h = fabs(fmod(hue, 360.0)) / 60.0;
	const int i = (int) h;
	const double f = h - i;
	const double v = value * 255;
	const double p = v * (1.0 - saturation);
	const double q = v * (1.0 - (saturation * f));
	const double t = v * (1.0 - (saturation * (1.0 - f)));

	switch (i) {
		case 0:
			color.r = v;
			color.g = t;
			color.b = p;
			break;
		case 1:
			color.r = q;
			color.g = v;
			color.b = p;
			break;
		case 2:
			color.r = p;
			color.g = v;
			color.b = t;
			break;
		case 3:
			color.r = p;
			color.g = q;
			color.b = v;
			break;
		case 4:
			color.r = t;
			color.g = p;
			color.b = v;
			break;
		default:
			color.r = v;
			color.g = p;
			color.b = q;
			break;
	}

	return color;
}

void MVC_RGBToHSV(const SDL_Color *color, double *hue, double *saturation, double *value) {

	assert(color);

	const double r = color->r / 255.0;
	const double g = color->g / 255.0;
	const double b = color->b / 255.0;

	const double rgbMax = max(max(r, g), b);
	const double rgbMin = min(min(r, g), b);
	const double rgbDelta = rgbMax - rgbMin;

	if (rgbDelta > 0.0) {

		if (hue) {
			if (rgbMax == r) {
				*hue = 60.0 * fmod(((g - b) / rgbDelta), 6.0);
			} else if (rgbMax == g) {
				*hue = 60.0 * (((b - r) / rgbDelta) + 2.0);
			} else if (rgbMax == b) {
				*hue = 60.0 * (((r - g) / rgbDelta) + 4.0);
			}

			while (*hue < 0.0) {
				*hue += 360.0;
			}
		}

		if (saturation) {
			if (rgbMax > 0.0) {
				*saturation = rgbDelta / rgbMax;
			} else {
				*saturation = 0.0;
			}
		}

		if (value) {
			*value = rgbMax;
		}

	} else {
		if (hue) {
			*hue = 0.0;
		}

		if (saturation) {
			*saturation = 0.0;
		}

		if (value) {
			*value = rgbMax;
		}
	}
}
