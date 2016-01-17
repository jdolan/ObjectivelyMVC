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

#include <assert.h>

#include <Objectively/Once.h>

#include <ObjectivelyMVC/Font.h>

#define _Class _Font

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Font *this = (Font *) self;

	TTF_CloseFont(this->font);

	super(Object, self, dealloc);
}

#pragma mark - Font

static Font *_defaultFont;

/**
 * @fn Font *Font::defaultFont(void)
 *
 * @memberof Font
 */
static Font *defaultFont(void) {
	static Once once;

	DispatchOnce(once, {
		_defaultFont = $(alloc(Font), initWithPattern, "-sans-12");
	});

	return _defaultFont;
}

/**
 * @brief Resolves and loads the first font matching the given pattern.
 *
 * @param pat The Fontconfig pattern.
 *
 * @return The TrueType font, or `NULL` on error.
 */
static TTF_Font *loadFont(FcPattern *pat) {
	TTF_Font *font = NULL;

	assert(pat);

	FcPatternAddString(pat, FC_FONTFORMAT, (FcChar8 *) "TrueType");

	FcObjectSet *objects = FcObjectSetCreate();

	FcObjectSetAdd(objects, FC_FILE);
	FcObjectSetAdd(objects, FC_SIZE);
	FcObjectSetAdd(objects, FC_INDEX);

	FcFontSet *fonts = FcFontList(NULL, pat, objects);
	FcPattern **f = fonts->fonts;
	for (int i = 0; i < fonts->nfont; i++, f++) {

		FcChar8 *path;
		if (FcPatternGetString(*f, FC_FILE, 0, &path) == FcResultMatch) {

			double size;
			if (FcPatternGetDouble(*f, FC_SIZE, 0, &size) == FcResultMatch) {

				int index;
				if (FcPatternGetInteger(*f, FC_INDEX, 0, &index) == FcResultMatch) {
					SDL_LogDebug(0, "Found font %s @%dpt (%d)", path, (int) size, index);

					font = TTF_OpenFontIndex((char *) path, (int) size, index);
					if (font) {
						break;
					}
				}
			}
		}
	}

	FcObjectSetDestroy(objects);
	return font;
}

/**
 * @fn Font *Font::initWithPattern(Font *self, const char *pattern)
 *
 * @memberof Font
 */
static Font *initWithPattern(Font *self, const char *pattern) {

	self = (Font *) super(Object, self, init);
	if (self) {

		FcPattern *pat = FcNameParse((FcChar8 *) pattern);

		self->font = loadFont(pat);
		assert(self->font);

		FcPatternDestroy(pat);
	}

	return self;
}

/**
 * @fn Font *Font::initWithAttributes(Font *self, const char *family, int ptsize, int style)
 *
 * @memberof Font
 */
static Font *initWithAttributes(Font *self, const char *family, int ptsize, int style) {

	self = (Font *) super(Object, self, init);
	if (self) {

		FcPattern *pat = FcPatternCreate();

		FcPatternAddString(pat, FC_FAMILY, (FcChar8 *) family);
		FcPatternAddDouble(pat, FC_SIZE, (double) ptsize);

		if (style & TTF_STYLE_BOLD) {
			if (style & TTF_STYLE_ITALIC) {
				FcPatternAddString(pat, FC_STYLE, (FcChar8 *) "Bold Italic");
			} else {
				FcPatternAddString(pat, FC_STYLE, (FcChar8 *) "Bold");
			}
		} else if (style & TTF_STYLE_ITALIC) {
			FcPatternAddString(pat, FC_STYLE, (FcChar8 *) "Italic");
		} else {
			FcPatternAddString(pat, FC_STYLE, (FcChar8 *) "Regular");
		}

		self->font = loadFont(pat);
		assert(self->font);

		FcPatternDestroy(pat);
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((FontInterface *) clazz->interface)->defaultFont = defaultFont;
	((FontInterface *) clazz->interface)->initWithAttributes = initWithAttributes;
	((FontInterface *) clazz->interface)->initWithPattern = initWithPattern;

	const FcBool res = FcInit();
	assert(res == FcTrue);

	const int err = TTF_Init();
	assert(err == 0);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_defaultFont);

	FcFini();
	TTF_Quit();
}

Class _Font = {
	.name = "Font",
	.superclass = &_Object,
	.instanceSize = sizeof(Font),
	.interfaceOffset = offsetof(Font, interface),
	.interfaceSize = sizeof(FontInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class
