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
#include <string.h>

#include <fontconfig/fontconfig.h>

#include <Objectively/MutableArray.h>
#include <Objectively/String.h>

#include <ObjectivelyMVC/Font.h>
#include <ObjectivelyMVC/Log.h>
#include <ObjectivelyMVC/View.h>
#include <ObjectivelyMVC/Window.h>

const EnumName FontStyleNames[] = MakeEnumNames(
	MakeEnumAlias(FontStyleRegular, regular),
	MakeEnumAlias(FontStyleBold, bold),
	MakeEnumAlias(FontStyleItalic, italic),
	MakeEnumAlias(FontStyleUnderline, underline),
	MakeEnumAlias(FontStyleStrikeThrough, strikethrough)
);

const EnumName FontCategoryNames[] = MakeEnumNames(
	MakeEnumAlias(FontCategoryDefault, default),
	MakeEnumAlias(FontCategoryPrimaryLabel, primary-label),
	MakeEnumAlias(FontCategorySecondaryLabel, secondary-label),
	MakeEnumAlias(FontCategoryPrimaryControl, primary-control),
	MakeEnumAlias(FontCategorySecondaryControl, secondary-control),
	MakeEnumAlias(FontCategoryPrimaryResponder, primary-responder),
	MakeEnumAlias(FontCategorySecondaryResponder, secondary-responder),
	MakeEnumAlias(FontCategoryUser, user)
);

#define _Class _Font

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Font *this = (Font *) self;

	release(this->data);
	
	TTF_CloseFont(this->font);

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

static Font *_defaultFonts[FontCategoryMax];

/**
 * @fn Font *Font::defaultFont(FontCategory)
 * @memberof Font
 */
static Font *defaultFont(FontCategory category) {

	if (!_defaultFonts[category]) {
		switch (category) {
			case FontCategoryDefault:
			case FontCategoryPrimaryLabel:
			case FontCategoryPrimaryControl:
				_defaultFonts[category] = $(alloc(Font), initWithAttributes, DEFAULT_FONT_FAMILY, 16, 0);
				break;
			case FontCategorySecondaryLabel:
			case FontCategorySecondaryControl:
				_defaultFonts[category] = $(alloc(Font), initWithAttributes, DEFAULT_FONT_FAMILY, 14, 0);
				break;
			case FontCategoryPrimaryResponder:
			case FontCategorySecondaryResponder:
				_defaultFonts[category] = $(alloc(Font), initWithAttributes, DEFAULT_FONT_FAMILY, 18, 0);
				break;
			default:
				MVC_LogWarn("No Font set for user category %d\n", category);
				_defaultFonts[category] = $(alloc(Font), initWithAttributes, DEFAULT_FONT_FAMILY, 16, 0);
				break;
		}
	}

	assert(_defaultFonts[category]);
	return _defaultFonts[category];
}

/**
 * @fn Font *Font::initWithAttributes(Font *self, const char *family, int size, int style)
 * @memberof Font
 */
static Font *initWithAttributes(Font *self, const char *family, int size, int style) {
	FcResult result;

	FcPattern *search = FcPatternCreate();
	assert(search);

	FcPatternAddString(search, FC_FONTFORMAT, (FcChar8 *) "TrueType");
	FcPatternAddString(search, FC_FAMILY, (FcChar8 *) (family ?: DEFAULT_FONT_FAMILY));
	FcPatternAddDouble(search, FC_SIZE, (double) (size ?: DEFAULT_FONT_SIZE));

	if (style & FontStyleBold) {
		if (style & FontStyleItalic) {
			FcPatternAddString(search, FC_STYLE, (FcChar8 *) "Bold Italic");
		} else {
			FcPatternAddString(search, FC_STYLE, (FcChar8 *) "Bold");
		}
	} else if (style & FontStyleItalic) {
		FcPatternAddString(search, FC_STYLE, (FcChar8 *) "Italic");
	} else {
		FcPatternAddString(search, FC_STYLE, (FcChar8 *) "Regular");
	}

	FcConfigSubstitute(NULL, search, FcMatchPattern);
	FcDefaultSubstitute(search);

	FcPattern *pattern = FcFontMatch(NULL, search, &result);
	if (result == FcResultMatch) {
		self = $(self, initWithPattern, pattern);
	}

	FcPatternDestroy(pattern);
	FcPatternDestroy(search);

	return self;
}

/**
 * @fn Font *Font::initWithData(Font *self, Data *data, int size, int index)
 * @memberof Font
 */
static Font *initWithData(Font *self, Data *data, int size, int index) {

	self = (Font *) super(Object, self, init);
	if (self) {

		self->data = retain(data);
		assert(self->data);

		self->size = size;
		self->index = index;

		$(self, renderDeviceDidReset);
	}

	return self;
}

/**
 * @fn Font *Font::initWithName(Font *self, const char *name)
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
 * @memberof Font
 */
static Font *initWithPattern(Font *self, ident pattern) {

	assert(pattern);

	FcChar8 *file;
	if (FcPatternGetString(pattern, FC_FILE, 0, &file) == FcResultMatch) {

		double size;
		if (FcPatternGetDouble(pattern, FC_SIZE, 0, &size) == FcResultMatch) {

			int index = 0;
			if (FcPatternGetInteger(pattern, FC_INDEX, 0, &index) == FcResultMatch) {

				Data *data = $$(Data, dataWithContentsOfFile, (char *) file);

				self = $(self, initWithData, data, size, index);

				release(data);
			}
		}
	}

	return self;
}

/**
 * @fn void Font::renderCharacters(const Font *self, const char *chars, SDL_Color color)
 * @memberof Font
 */
static SDL_Surface *renderCharacters(const Font *self, const char *chars, SDL_Color color) {

	SDL_Surface *surface = TTF_RenderUTF8_Blended(self->font, chars, color);

	if (surface == NULL) {
		MVC_LogError("%s\n", TTF_GetError());
	}

	return surface;
}

/**
 * @fn void Font::renderDeviceDidReset(Font *self)
 * @memberof Font
 */
static void renderDeviceDidReset(Font *self) {

	const int renderSize = self->size * MVC_WindowScale(NULL, NULL, NULL);
	if (renderSize != self->renderSize) {

		self->renderSize = renderSize;

		if (self->font) {
			TTF_CloseFont(self->font);
		}

		SDL_RWops *buffer = SDL_RWFromConstMem(self->data->bytes, (int) self->data->length);
		assert(buffer);

		self->font = TTF_OpenFontIndexRW(buffer, 1, self->renderSize, self->index);
		assert(self->font);

		TTF_SetFontHinting(self->font, TTF_HINTING_NORMAL);
	}
}

/**
 * @fn void Font::setDefaultFont(FontCategory category, Font *font)
 * @memberof Font
 */
void setDefaultFont(FontCategory category, Font *font) {

	if (_defaultFonts[category] != font) {

		_defaultFonts[category] = release(_defaultFonts[category]);

		if (font) {
			_defaultFonts[category] = retain(font);
		}
	}
}

/**
 * @fn void Font::sizeCharacters(const Font *self, const char *chars, int *w, int *h)
 * @memberof Font
 */
static void sizeCharacters(const Font *self, const char *chars, int *w, int *h) {

	TTF_SizeUTF8(self->font, chars, w, h);

	const float scale = MVC_WindowScale(NULL, NULL, NULL);
	if (w) {
		*w /= scale;
	}
	if (h) {
		*h /= scale;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((FontInterface *) clazz->def->interface)->allFonts = allFonts;
	((FontInterface *) clazz->def->interface)->defaultFont = defaultFont;
	((FontInterface *) clazz->def->interface)->initWithAttributes = initWithAttributes;
	((FontInterface *) clazz->def->interface)->initWithData = initWithData;
	((FontInterface *) clazz->def->interface)->initWithName = initWithName;
	((FontInterface *) clazz->def->interface)->initWithPattern = initWithPattern;
	((FontInterface *) clazz->def->interface)->renderCharacters = renderCharacters;
	((FontInterface *) clazz->def->interface)->renderDeviceDidReset = renderDeviceDidReset;
	((FontInterface *) clazz->def->interface)->setDefaultFont = setDefaultFont;
	((FontInterface *) clazz->def->interface)->sizeCharacters = sizeCharacters;

	const FcBool res = FcInit();
	assert(res == FcTrue);

	const int err = TTF_Init();
	assert(err == 0);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	for (FontCategory c = 0; c < FontCategoryMax; c++) {
		release(_defaultFonts[c]);
	}

	FcFini();
	TTF_Quit();
}

/**
 * @fn Class *Font::_Font(void)
 * @memberof Font
 */
Class *_Font(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Font";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Font);
		clazz.interfaceOffset = offsetof(Font, interface);
		clazz.interfaceSize = sizeof(FontInterface);
		clazz.initialize = initialize;
		clazz.destroy = destroy;
	});

	return &clazz;
}

#undef _Class
