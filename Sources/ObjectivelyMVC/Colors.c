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

#include <ObjectivelyMVC/Colors.h>
#include <SDL2/SDL_endian.h>

const struct _Colors Colors = {

	.DefaultColor = { 128, 128, 128, 224 },
	.HighlightedColor = { 105, 105, 105, 255 },
	.DisabledColor = { 96, 96, 96, 255 },
	.SelectedColor = { 48, 48, 48, 255 },
	.FocusedColor = { 128, 128, 128, 255 },

	.AlternateColor = { 152, 152, 152, 192 },

	.Clear = { 255, 255, 255, 0 },

	.AliceBlue = { 240, 248, 255, 255 },
	.AntiqueWhite = { 250, 235, 215, 255 },
	.Aqua = { 0, 255, 255, 255 },
	.Aquamarine = { 127, 255, 212, 255 },
	.Azure = { 240, 255, 255, 255 },
	.Beige = { 245, 245, 220, 255 },
	.Bisque = { 255, 228, 196, 255 },
	.Black = { 0, 0, 0, 255 },
	.BlanchedAlmond = { 255, 235, 205, 255 },
	.Blue = { 0, 0, 255, 255 },
	.BlueViolet = { 138, 43, 226, 255 },
	.Brown = { 165, 42, 42, 255 },
	.BurlyWood = { 222, 184, 135, 255 },
	.CadetBlue = { 95, 158, 160, 255 },
	.Charcoal = { 48, 48, 48, 255 },
	.Chartreuse = { 127, 255, 0, 255 },
	.Chocolate = { 210, 105, 30, 255 },
	.Coral = { 255, 127, 80, 255 },
	.CornflowerBlue = { 100, 149, 237, 255 },
	.Cornsilk = { 255, 248, 220, 255 },
	.Crimson = { 220, 20, 60, 255 },
	.Cyan = { 0, 255, 255, 255 },
	.DarkBlue = { 0, 0, 139, 255 },
	.DarkCyan = { 139, 139, 255, 255 },
	.DarkGoldenRod = { 184, 134, 11, 255 },
	.DarkGray = { 169, 169, 169, 255 },
	.DarkGrey = { 169, 169, 169, 255 },
	.DarkGreen = { 0, 100, 0, 255 },
	.DarkKhaki = { 189, 183, 107, 255 },
	.DarkMagenta = { 139, 0, 139, 255 },
	.DarkOliveGreen = { 85, 107, 47, 255 },
	.DarkOrange = { 255, 140, 0, 255 },
	.DarkOrchid = { 153, 50, 204, 255 },
	.DarkRed = { 139, 0, 0, 255 },
	.DarkSalmon = { 233, 150, 122, 255 },
	.DarkSeaGreen = { 143, 188, 143, 255 },
	.DarkSlateBlue = { 72, 61, 139, 255 },
	.DarkSlateGray = { 47, 79, 79, 255 },
	.DarkSlateGrey = { 47, 79, 79, 255 },
	.DarkTurquoise = { 206, 209, 255, 255 },
	.DarkViolet = { 148, 0, 211, 255 },
	.DeepPink = { 255, 20, 147, 255 },
	.DeepSkyBlue = { 191, 255, 255, 255 },
	.DimGray = { 105, 105, 105, 255 },
	.DimGrey = { 105, 105, 105, 255 },
	.DodgerBlue = { 30, 144, 255, 255 },
	.FireBrick = { 178, 34, 34, 255 },
	.FloralWhite = { 255, 250, 240, 255 },
	.ForestGreen = { 34, 139, 34, 255 },
	.Fuchsia = { 255, 0, 255, 255 },
	.Gainsboro = { 220, 220, 220, 255 },
	.GhostWhite = { 248, 248, 255, 255 },
	.Gold = { 255, 215, 0, 255 },
	.GoldenRod = { 218, 165, 32, 255 },
	.Gray = { 128, 128, 128, 255 },
	.Grey = { 128, 128, 128, 255 },
	.Green = { 0, 128, 0, 255 },
	.GreenYellow = { 173, 255, 47, 255 },
	.HoneyDew = { 240, 255, 240, 255 },
	.HotPink = { 255, 105, 180, 255 },
	.IndianRed = { 205, 92, 92, 255 },
	.Indigo = { 75, 0, 130, 255 },
	.Ivory = { 255, 255, 240, 255 },
	.Khaki = { 240, 230, 140, 255 },
	.Lavender = { 230, 230, 250, 255 },
	.LavenderBlush = { 255, 240, 245, 255 },
	.LawnGreen = { 124, 252, 0, 255 },
	.LemonChiffon = { 255, 250, 205, 255 },
	.LightBlue = { 173, 216, 230, 255 },
	.LightCoral = { 240, 128, 128, 255 },
	.LightCyan = { 224, 255, 255, 255 },
	.LightGoldenRodYellow = { 250, 250, 210, 255 },
	.LightGray = { 211, 211, 211, 255 },
	.LightGrey = { 211, 211, 211, 255 },
	.LightGreen = { 144, 238, 144, 255 },
	.LightPink = { 255, 182, 193, 255 },
	.LightSalmon = { 255, 160, 122, 255 },
	.LightSeaGreen = { 32, 178, 170, 255 },
	.LightSkyBlue = { 135, 206, 250, 255 },
	.LightSlateGray = { 119, 136, 153, 255 },
	.LightSlateGrey = { 119, 136, 153, 255 },
	.LightSteelBlue = { 176, 196, 222, 255 },
	.LightYellow = { 255, 255, 224, 255 },
	.Lime = { 0, 255, 0, 255 },
	.LimeGreen = { 50, 205, 50, 255 },
	.Linen = { 250, 240, 230, 255 },
	.Magenta = { 255, 0, 255, 255 },
	.Maroon = { 128, 0, 0, 255 },
	.MediumAquaMarine = { 102, 205, 170, 255 },
	.MediumBlue = { 0, 0, 205, 255 },
	.MediumOrchid = { 186, 85, 211, 255 },
	.MediumPurple = { 147, 112, 219, 255 },
	.MediumSeaGreen = { 60, 179, 113, 255 },
	.MediumSlateBlue = { 123, 104, 238, 255 },
	.MediumSpringGreen = { 250, 154, 255, 255 },
	.MediumTurquoise = { 72, 209, 204, 255 },
	.MediumVioletRed = { 199, 21, 133, 255 },
	.MidnightBlue = { 25, 25, 112, 255 },
	.MintCream = { 245, 255, 250, 255 },
	.MistyRose = { 255, 228, 225, 255 },
	.Moccasin = { 255, 228, 181, 255 },
	.NavajoWhite = { 255, 222, 173, 255 },
	.Navy = { 0, 0, 128, 255 },
	.OldLace = { 253, 245, 230, 255 },
	.Olive = { 128, 128, 0, 255 },
	.OliveDrab = { 107, 142, 35, 255 },
	.Orange = { 255, 165, 0, 255 },
	.OrangeRed = { 255, 69, 0, 255 },
	.Orchid = { 218, 112, 214, 255 },
	.PaleGoldenRod = { 238, 232, 170, 255 },
	.PaleGreen = { 152, 251, 152, 255 },
	.PaleTurquoise = { 175, 238, 238, 255 },
	.PaleVioletRed = { 219, 112, 147, 255 },
	.PapayaWhip = { 255, 239, 213, 255 },
	.PeachPuff = { 255, 218, 185, 255 },
	.Peru = { 205, 133, 63, 255 },
	.Pink = { 255, 192, 203, 255 },
	.Plum = { 221, 160, 221, 255 },
	.PowderBlue = { 176, 224, 230, 255 },
	.Purple = { 128, 0, 128, 255 },
	.RebeccaPurple = { 102, 51, 153, 255 },
	.Red = { 255, 0, 0, 255 },
	.RosyBrown = { 188, 143, 143, 255 },
	.RoyalBlue = { 65, 105, 225, 255 },
	.SaddleBrown = { 139, 69, 19, 255 },
	.Salmon = { 250, 128, 114, 255 },
	.SandyBrown = { 244, 164, 96, 255 },
	.SeaGreen = { 46, 139, 87, 255 },
	.SeaShell = { 255, 245, 238, 255 },
	.Sienna = { 160, 82, 45, 255 },
	.Silver = { 192, 192, 192, 255 },
	.SkyBlue = { 135, 206, 235, 255 },
	.SlateBlue = { 106, 90, 205, 255 },
	.SlateGray = { 112, 128, 144, 255 },
	.SlateGrey = { 112, 128, 144, 255 },
	.Snow = { 255, 250, 250, 255 },
	.SpringGreen = { 0, 255, 127, 255 },
	.SteelBlue = { 70, 130, 180, 255 },
	.Tan = { 210, 180, 140, 255 },
	.Teal = { 0, 128, 128, 255 },
	.Thistle = { 216, 191, 216, 255 },
	.Tomato = { 255, 99, 71, 255 },
	.Turquoise = { 64, 224, 208, 255 },
	.Violet = { 238, 130, 238, 255 },
	.Wheat = { 245, 222, 179, 255 },
	.White = { 255, 255, 255, 255 },
	.WhiteSmoke = { 245, 245, 245, 255 },
	.Yellow = { 255, 255, 0, 255 },
	.YellowGreen = { 154, 205, 50, 255 },
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

static double MVC_ConstrainAngle(double x) {

	x = fmod(x,360);
	
	if (x < 0)
		x += 360;
    
	return x;
}

SDL_Color MVC_HSVToRGB(double hue, double saturation, double value) {

	SDL_Color color = { .a = 255 };

	const double h = MVC_ConstrainAngle(hue) / 60.0;
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
