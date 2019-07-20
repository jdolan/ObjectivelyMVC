/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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

#include <SDL_endian.h>

#include "Colors.h"

const struct Colors Colors = {
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
	.Transparent = { 255, 255, 255, 0 },
	.Turquoise = { 64, 224, 208, 255 },
	.Violet = { 238, 130, 238, 255 },
	.Wheat = { 245, 222, 179, 255 },
	.White = { 255, 255, 255, 255 },
	.WhiteSmoke = { 245, 245, 245, 255 },
	.Yellow = { 255, 255, 0, 255 },
	.YellowGreen = { 154, 205, 50, 255 },
};

SDL_Color MVC_ColorForName(const char *name) {

	if (name) {
		if (strcasecmp(name, "AliceBlue") == 0) {
			return Colors.AliceBlue;
		}
		if (strcasecmp(name, "AntiqueWhite") == 0) {
			return Colors.AntiqueWhite;
		}
		if (strcasecmp(name, "Aqua") == 0) {
			return Colors.Aqua;
		}
		if (strcasecmp(name, "Aquamarine") == 0) {
			return Colors.Aquamarine;
		}
		if (strcasecmp(name, "Azure") == 0) {
			return Colors.Azure;
		}
		if (strcasecmp(name, "Beige") == 0) {
			return Colors.Beige;
		}
		if (strcasecmp(name, "Bisque") == 0) {
			return Colors.Bisque;
		}
		if (strcasecmp(name, "Black") == 0) {
			return Colors.Black;
		}
		if (strcasecmp(name, "BlanchedAlmond") == 0) {
			return Colors.BlanchedAlmond;
		}
		if (strcasecmp(name, "Blue") == 0) {
			return Colors.Blue;
		}
		if (strcasecmp(name, "BlueViolet") == 0) {
			return Colors.BlueViolet;
		}
		if (strcasecmp(name, "Brown") == 0) {
			return Colors.Brown;
		}
		if (strcasecmp(name, "BurlyWood") == 0) {
			return Colors.BurlyWood;
		}
		if (strcasecmp(name, "CadetBlue") == 0) {
			return Colors.CadetBlue;
		}
		if (strcasecmp(name, "Charcoal") == 0) {
			return Colors.Charcoal;
		}
		if (strcasecmp(name, "Chartreuse") == 0) {
			return Colors.Chartreuse;
		}
		if (strcasecmp(name, "Chocolate") == 0) {
			return Colors.Chocolate;
		}
		if (strcasecmp(name, "Coral") == 0) {
			return Colors.Coral;
		}
		if (strcasecmp(name, "CornflowerBlue") == 0) {
			return Colors.CornflowerBlue;
		}
		if (strcasecmp(name, "Cornsilk") == 0) {
			return Colors.Cornsilk;
		}
		if (strcasecmp(name, "Crimson") == 0) {
			return Colors.Crimson;
		}
		if (strcasecmp(name, "Cyan") == 0) {
			return Colors.Cyan;
		}
		if (strcasecmp(name, "DarkBlue") == 0) {
			return Colors.DarkBlue;
		}
		if (strcasecmp(name, "DarkCyan") == 0) {
			return Colors.DarkCyan;
		}
		if (strcasecmp(name, "DarkGoldenRod") == 0) {
			return Colors.DarkGoldenRod;
		}
		if (strcasecmp(name, "DarkGray") == 0) {
			return Colors.DarkGray;
		}
		if (strcasecmp(name, "DarkGrey") == 0) {
			return Colors.DarkGrey;
		}
		if (strcasecmp(name, "DarkGreen") == 0) {
			return Colors.DarkGreen;
		}
		if (strcasecmp(name, "DarkKhaki") == 0) {
			return Colors.DarkKhaki;
		}
		if (strcasecmp(name, "DarkMagenta") == 0) {
			return Colors.DarkMagenta;
		}
		if (strcasecmp(name, "DarkOliveGreen") == 0) {
			return Colors.DarkOliveGreen;
		}
		if (strcasecmp(name, "DarkOrange") == 0) {
			return Colors.DarkOrange;
		}
		if (strcasecmp(name, "DarkOrchid") == 0) {
			return Colors.DarkOrchid;
		}
		if (strcasecmp(name, "DarkRed") == 0) {
			return Colors.DarkRed;
		}
		if (strcasecmp(name, "DarkSalmon") == 0) {
			return Colors.DarkSalmon;
		}
		if (strcasecmp(name, "DarkSeaGreen") == 0) {
			return Colors.DarkSeaGreen;
		}
		if (strcasecmp(name, "DarkSlateBlue") == 0) {
			return Colors.DarkSlateBlue;
		}
		if (strcasecmp(name, "DarkSlateGray") == 0) {
			return Colors.DarkSlateGray;
		}
		if (strcasecmp(name, "DarkSlateGrey") == 0) {
			return Colors.DarkSlateGrey;
		}
		if (strcasecmp(name, "DarkTurquoise") == 0) {
			return Colors.DarkTurquoise;
		}
		if (strcasecmp(name, "DarkViolet") == 0) {
			return Colors.DarkViolet;
		}
		if (strcasecmp(name, "DeepPink") == 0) {
			return Colors.DeepPink;
		}
		if (strcasecmp(name, "DeepSkyBlue") == 0) {
			return Colors.DeepSkyBlue;
		}
		if (strcasecmp(name, "DimGray") == 0) {
			return Colors.DimGray;
		}
		if (strcasecmp(name, "DimGrey") == 0) {
			return Colors.DimGrey;
		}
		if (strcasecmp(name, "DodgerBlue") == 0) {
			return Colors.DodgerBlue;
		}
		if (strcasecmp(name, "FireBrick") == 0) {
			return Colors.FireBrick;
		}
		if (strcasecmp(name, "FloralWhite") == 0) {
			return Colors.FloralWhite;
		}
		if (strcasecmp(name, "ForestGreen") == 0) {
			return Colors.ForestGreen;
		}
		if (strcasecmp(name, "Fuchsia") == 0) {
			return Colors.Fuchsia;
		}
		if (strcasecmp(name, "Gainsboro") == 0) {
			return Colors.Gainsboro;
		}
		if (strcasecmp(name, "GhostWhite") == 0) {
			return Colors.GhostWhite;
		}
		if (strcasecmp(name, "Gold") == 0) {
			return Colors.Gold;
		}
		if (strcasecmp(name, "GoldenRod") == 0) {
			return Colors.GoldenRod;
		}
		if (strcasecmp(name, "Gray") == 0) {
			return Colors.Gray;
		}
		if (strcasecmp(name, "Grey") == 0) {
			return Colors.Grey;
		}
		if (strcasecmp(name, "Green") == 0) {
			return Colors.Green;
		}
		if (strcasecmp(name, "GreenYellow") == 0) {
			return Colors.GreenYellow;
		}
		if (strcasecmp(name, "HoneyDew") == 0) {
			return Colors.HoneyDew;
		}
		if (strcasecmp(name, "HotPink") == 0) {
			return Colors.HotPink;
		}
		if (strcasecmp(name, "IndianRed") == 0) {
			return Colors.IndianRed;
		}
		if (strcasecmp(name, "Indigo") == 0) {
			return Colors.Indigo;
		}
		if (strcasecmp(name, "Ivory") == 0) {
			return Colors.Ivory;
		}
		if (strcasecmp(name, "Khaki") == 0) {
			return Colors.Khaki;
		}
		if (strcasecmp(name, "Lavender") == 0) {
			return Colors.Lavender;
		}
		if (strcasecmp(name, "LavenderBlush") == 0) {
			return Colors.LavenderBlush;
		}
		if (strcasecmp(name, "LawnGreen") == 0) {
			return Colors.LawnGreen;
		}
		if (strcasecmp(name, "LemonChiffon") == 0) {
			return Colors.LemonChiffon;
		}
		if (strcasecmp(name, "LightBlue") == 0) {
			return Colors.LightBlue;
		}
		if (strcasecmp(name, "LightCoral") == 0) {
			return Colors.LightCoral;
		}
		if (strcasecmp(name, "LightCyan") == 0) {
			return Colors.LightCyan;
		}
		if (strcasecmp(name, "LightGoldenRodYellow") == 0) {
			return Colors.LightGoldenRodYellow;
		}
		if (strcasecmp(name, "LightGray") == 0) {
			return Colors.LightGray;
		}
		if (strcasecmp(name, "LightGrey") == 0) {
			return Colors.LightGrey;
		}
		if (strcasecmp(name, "LightGreen") == 0) {
			return Colors.LightGreen;
		}
		if (strcasecmp(name, "LightPink") == 0) {
			return Colors.LightPink;
		}
		if (strcasecmp(name, "LightSalmon") == 0) {
			return Colors.LightSalmon;
		}
		if (strcasecmp(name, "LightSeaGreen") == 0) {
			return Colors.LightSeaGreen;
		}
		if (strcasecmp(name, "LightSkyBlue") == 0) {
			return Colors.LightSkyBlue;
		}
		if (strcasecmp(name, "LightSlateGray") == 0) {
			return Colors.LightSlateGray;
		}
		if (strcasecmp(name, "LightSlateGrey") == 0) {
			return Colors.LightSlateGrey;
		}
		if (strcasecmp(name, "LightSteelBlue") == 0) {
			return Colors.LightSteelBlue;
		}
		if (strcasecmp(name, "LightYellow") == 0) {
			return Colors.LightYellow;
		}
		if (strcasecmp(name, "Lime") == 0) {
			return Colors.Lime;
		}
		if (strcasecmp(name, "LimeGreen") == 0) {
			return Colors.LimeGreen;
		}
		if (strcasecmp(name, "Linen") == 0) {
			return Colors.Linen;
		}
		if (strcasecmp(name, "Magenta") == 0) {
			return Colors.Magenta;
		}
		if (strcasecmp(name, "Maroon") == 0) {
			return Colors.Maroon;
		}
		if (strcasecmp(name, "MediumAquaMarine") == 0) {
			return Colors.MediumAquaMarine;
		}
		if (strcasecmp(name, "MediumBlue") == 0) {
			return Colors.MediumBlue;
		}
		if (strcasecmp(name, "MediumOrchid") == 0) {
			return Colors.MediumOrchid;
		}
		if (strcasecmp(name, "MediumPurple") == 0) {
			return Colors.MediumPurple;
		}
		if (strcasecmp(name, "MediumSeaGreen") == 0) {
			return Colors.MediumSeaGreen;
		}
		if (strcasecmp(name, "MediumSlateBlue") == 0) {
			return Colors.MediumSlateBlue;
		}
		if (strcasecmp(name, "MediumSpringGreen") == 0) {
			return Colors.MediumSpringGreen;
		}
		if (strcasecmp(name, "MediumTurquoise") == 0) {
			return Colors.MediumTurquoise;
		}
		if (strcasecmp(name, "MediumVioletRed") == 0) {
			return Colors.MediumVioletRed;
		}
		if (strcasecmp(name, "MidnightBlue") == 0) {
			return Colors.MidnightBlue;
		}
		if (strcasecmp(name, "MintCream") == 0) {
			return Colors.MintCream;
		}
		if (strcasecmp(name, "MistyRose") == 0) {
			return Colors.MistyRose;
		}
		if (strcasecmp(name, "Moccasin") == 0) {
			return Colors.Moccasin;
		}
		if (strcasecmp(name, "NavajoWhite") == 0) {
			return Colors.NavajoWhite;
		}
		if (strcasecmp(name, "Navy") == 0) {
			return Colors.Navy;
		}
		if (strcasecmp(name, "OldLace") == 0) {
			return Colors.OldLace;
		}
		if (strcasecmp(name, "Olive") == 0) {
			return Colors.Olive;
		}
		if (strcasecmp(name, "OliveDrab") == 0) {
			return Colors.OliveDrab;
		}
		if (strcasecmp(name, "Orange") == 0) {
			return Colors.Orange;
		}
		if (strcasecmp(name, "OrangeRed") == 0) {
			return Colors.OrangeRed;
		}
		if (strcasecmp(name, "Orchid") == 0) {
			return Colors.Orchid;
		}
		if (strcasecmp(name, "PaleGoldenRod") == 0) {
			return Colors.PaleGoldenRod;
		}
		if (strcasecmp(name, "PaleGreen") == 0) {
			return Colors.PaleGreen;
		}
		if (strcasecmp(name, "PaleTurquoise") == 0) {
			return Colors.PaleTurquoise;
		}
		if (strcasecmp(name, "PaleVioletRed") == 0) {
			return Colors.PaleVioletRed;
		}
		if (strcasecmp(name, "PapayaWhip") == 0) {
			return Colors.PapayaWhip;
		}
		if (strcasecmp(name, "PeachPuff") == 0) {
			return Colors.PeachPuff;
		}
		if (strcasecmp(name, "Peru") == 0) {
			return Colors.Peru;
		}
		if (strcasecmp(name, "Pink") == 0) {
			return Colors.Pink;
		}
		if (strcasecmp(name, "Plum") == 0) {
			return Colors.Plum;
		}
		if (strcasecmp(name, "PowderBlue") == 0) {
			return Colors.PowderBlue;
		}
		if (strcasecmp(name, "Purple") == 0) {
			return Colors.Purple;
		}
		if (strcasecmp(name, "RebeccaPurple") == 0) {
			return Colors.RebeccaPurple;
		}
		if (strcasecmp(name, "Red") == 0) {
			return Colors.Red;
		}
		if (strcasecmp(name, "RosyBrown") == 0) {
			return Colors.RosyBrown;
		}
		if (strcasecmp(name, "RoyalBlue") == 0) {
			return Colors.RoyalBlue;
		}
		if (strcasecmp(name, "SaddleBrown") == 0) {
			return Colors.SaddleBrown;
		}
		if (strcasecmp(name, "Salmon") == 0) {
			return Colors.Salmon;
		}
		if (strcasecmp(name, "SandyBrown") == 0) {
			return Colors.SandyBrown;
		}
		if (strcasecmp(name, "SeaGreen") == 0) {
			return Colors.SeaGreen;
		}
		if (strcasecmp(name, "SeaShell") == 0) {
			return Colors.SeaShell;
		}
		if (strcasecmp(name, "Sienna") == 0) {
			return Colors.Sienna;
		}
		if (strcasecmp(name, "Silver") == 0) {
			return Colors.Silver;
		}
		if (strcasecmp(name, "SkyBlue") == 0) {
			return Colors.SkyBlue;
		}
		if (strcasecmp(name, "SlateBlue") == 0) {
			return Colors.SlateBlue;
		}
		if (strcasecmp(name, "SlateGray") == 0) {
			return Colors.SlateGray;
		}
		if (strcasecmp(name, "SlateGrey") == 0) {
			return Colors.SlateGrey;
		}
		if (strcasecmp(name, "Snow") == 0) {
			return Colors.Snow;
		}
		if (strcasecmp(name, "SpringGreen") == 0) {
			return Colors.SpringGreen;
		}
		if (strcasecmp(name, "SteelBlue") == 0) {
			return Colors.SteelBlue;
		}
		if (strcasecmp(name, "Tan") == 0) {
			return Colors.Tan;
		}
		if (strcasecmp(name, "Teal") == 0) {
			return Colors.Teal;
		}
		if (strcasecmp(name, "Thistle") == 0) {
			return Colors.Thistle;
		}
		if (strcasecmp(name, "Tomato") == 0) {
			return Colors.Tomato;
		}
		if (strcasecmp(name, "Transparent") == 0) {
			return Colors.Transparent;
		}
		if (strcasecmp(name, "Turquoise") == 0) {
			return Colors.Turquoise;
		}
		if (strcasecmp(name, "Violet") == 0) {
			return Colors.Violet;
		}
		if (strcasecmp(name, "Wheat") == 0) {
			return Colors.Wheat;
		}
		if (strcasecmp(name, "White") == 0) {
			return Colors.White;
		}
		if (strcasecmp(name, "WhiteSmoke") == 0) {
			return Colors.WhiteSmoke;
		}
		if (strcasecmp(name, "Yellow") == 0) {
			return Colors.Yellow;
		}
		if (strcasecmp(name, "YellowGreen") == 0) {
			return Colors.YellowGreen;
		}
	}

	return Colors.Black;
}

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

	assert(color);

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
