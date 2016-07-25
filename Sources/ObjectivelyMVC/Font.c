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

#include <fontconfig/fontconfig.h>

#include <Objectively/MutableArray.h>
#include <Objectively/Once.h>
#include <Objectively/String.h>

#include <ObjectivelyMVC/Font.h>
#include <ObjectivelyMVC/Image.h>
#include <ObjectivelyMVC/Log.h>

#define _Class _Font

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Font *this = (Font *) self;

	if (this->font) {
		TTF_CloseFont(this->font);
	}

	free(this->name);

	super(Object, self, dealloc);
}

#pragma mark - Font

/**
 * @brief Comparator for allFonts sorting.
 */
static Order allFonts_sort(const ident a, const ident b) {
	return strcmp(((String *) a)->chars, ((String *) b)->chars);
}

/**
 * @fn Array *Font::allFonts(void)
 *
 * @memberof Font
 */
static Array *allFonts(void) {
	
	MutableArray *fonts = $$(MutableArray, array);

	FcPattern *pattern = FcPatternCreate();
	
	FcPatternAddString(pattern, FC_FONTFORMAT, (FcChar8 *) "TrueType");
	FcConfigSubstitute(NULL, pattern, FcMatchScan);
	
	FcObjectSet *objectSet = FcObjectSetBuild(FC_FULLNAME, NULL);
	FcFontSet *fontSet = FcFontList(NULL, pattern, objectSet);
	
	FcPattern **font = fontSet->fonts;
	for (int i = 0; i < fontSet->nfont; i++, font++) {

		char *name = (char *) FcNameUnparse(*font);
		String *string = $$(String, stringWithMemory, name, strlen(name));
		
		$(fonts, addObject, string);
		release(string);
	}
	
	FcFontSetDestroy(fontSet);
	FcObjectSetDestroy(objectSet);
	FcPatternDestroy(pattern);

	$(fonts, sort, allFonts_sort);
	
	return (Array *) fonts;
}

static Font *_normal;
static Font *_smaller;
static Font *_bigger;

/**
 * @fn Font *Font::defaultFont(FontCategory)
 *
 * @memberof Font
 */
static Font *defaultFont(FontCategory category) {
	static Once once;

	do_once(&once, {
		_normal = $(alloc(Font), initWithAttributes, DEFAULT_FONT_FAMILY, 14, 0);
		assert(_normal);

		_smaller = $(alloc(Font), initWithAttributes, DEFAULT_FONT_FAMILY, 12, 0);
		assert(_smaller);

		_bigger = $(alloc(Font), initWithAttributes, DEFAULT_FONT_FAMILY, 16, 0);
		assert(_bigger);
	});

	switch (category) {
		case FontCategoryDefault:
		case FontCategoryPrimaryLabel:
		case FontCategoryPrimaryControl:
			return _normal;
		case FontCategorySecondaryLabel:
		case FontCategorySecondaryControl:
			return _smaller;
	}
}

/**
 * @return A Fontconfig pattern from the given attributes.
 */
static FcPattern *patternWithAttributes(const char *family, int ptsize, int style) {

	FcPattern *pattern = FcPatternCreate();
	assert(pattern);

	FcPatternAddString(pattern, FC_FAMILY, (FcChar8 *) family);

	if (WindowUsesHighDPI(NULL)) {
		FcPatternAddDouble(pattern, FC_SIZE, (double) ptsize * 2.0);
	} else {
		FcPatternAddDouble(pattern, FC_SIZE, (double) ptsize);
	}

	if (style & TTF_STYLE_BOLD) {
		if (style & TTF_STYLE_ITALIC) {
			FcPatternAddString(pattern, FC_STYLE, (FcChar8 *) "Bold Italic");
		} else {
			FcPatternAddString(pattern, FC_STYLE, (FcChar8 *) "Bold");
		}
	} else if (style & TTF_STYLE_ITALIC) {
		FcPatternAddString(pattern, FC_STYLE, (FcChar8 *) "Italic");
	} else {
		FcPatternAddString(pattern, FC_STYLE, (FcChar8 *) "Regular");
	}

	return pattern;
}

/**
 * @fn Font *Font::initWithAttributes(Font *self, const char *family, int ptsize, int style)
 *
 * @memberof Font
 */
static Font *initWithAttributes(Font *self, const char *family, int ptsize, int style) {

	FcPattern *pattern = patternWithAttributes(family, ptsize, style);

	self = $(self, initWithPattern, pattern);

	FcPatternDestroy(pattern);

	return self;
}

/**
 * @fn Font *Font::initWithName(Font *self, const char *name)
 *
 * @memberof Font
 */
static Font *initWithName(Font *self, const char *name) {

	FcPattern *pattern = FcNameParse((FcChar8 *) name);
	if (pattern) {

		self = $(self, initWithPattern, pattern);

		FcPatternDestroy(pattern);
	}

	return self;
}

/**
 * @fn Font *Font::initWithPattern(Font *self, ident pattern)
 *
 * @memberof Font
 */
static Font *initWithPattern(Font *self, ident pattern) {

	self = (Font *) super(Object, self, init);
	if (self) {

		assert(pattern);

		FcPatternAddString(pattern, FC_FONTFORMAT, (FcChar8 *) "TrueType");

		FcConfigSubstitute(NULL, pattern, FcMatchFont);
		FcDefaultSubstitute(pattern);

		FcResult result;
		FcPattern *match = FcFontMatch(NULL, pattern, &result);

		if (result == FcResultMatch) {

			FcChar8 *path;
			if (FcPatternGetString(match, FC_FILE, 0, &path) == FcResultMatch) {

				double size;
				if (FcPatternGetDouble(match, FC_SIZE, 0, &size) == FcResultMatch) {

					int index;
					if (FcPatternGetInteger(match, FC_INDEX, 0, &index) == FcResultMatch) {

						self->font = TTF_OpenFontIndex((char *) path, (int) size, index);
						if (self->font) {
							TTF_SetFontHinting(self->font, TTF_HINTING_LIGHT);

							self->name = (char *) FcNameUnparse(pattern);
							assert(self->name);
						}
					}
				}
			}
		}
		
		FcPatternDestroy(match);

		if (self->font == NULL) {
			FcChar8 *name = FcNameUnparse(pattern);
			LogWarn("Failed to load font with pattern \"%s\"\n", name);

			free(name);
			release(self);
			self = NULL;
		}
	}

	return self;
}


/**
 * @fn void Font::renderCharacters(const Font *self, const char *chars, SDL_Color color)
 *
 * @memberof Font
 */
static SDL_Surface *renderCharacters(const Font *self, const char *chars, SDL_Color color) {
	return TTF_RenderUTF8_Blended(self->font, chars, color);
}

/**
 * @fn void Font::sizeCharacters(const Font *self, const char *chars, int *w, int *h)
 *
 * @memberof Font
 */
static void sizeCharacters(const Font *self, const char *chars, int *w, int *h) {
	
	TTF_SizeUTF8(self->font, chars, w, h);
	
	if (WindowUsesHighDPI(NULL)) {
		*w *= 0.5;
		*h *= 0.5;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((FontInterface *) clazz->interface)->allFonts = allFonts;
	((FontInterface *) clazz->interface)->defaultFont = defaultFont;
	((FontInterface *) clazz->interface)->initWithAttributes = initWithAttributes;
	((FontInterface *) clazz->interface)->initWithName = initWithName;
	((FontInterface *) clazz->interface)->initWithPattern = initWithPattern;
	((FontInterface *) clazz->interface)->renderCharacters = renderCharacters;
	((FontInterface *) clazz->interface)->sizeCharacters = sizeCharacters;

	const FcBool res = FcInit();
	assert(res == FcTrue);

	const int err = TTF_Init();
	assert(err == 0);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_normal);
	release(_smaller);
	release(_bigger);

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
