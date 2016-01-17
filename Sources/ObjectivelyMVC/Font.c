/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
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
 * @param pattern The Fontconfig pattern.
 *
 * @return The TrueType font, or `NULL` on error.
 */
static TTF_Font *loadFont(FcPattern *pattern) {
	TTF_Font *font = NULL;

	assert(pattern);

	FcPatternAddString(pattern, FC_FONTFORMAT, (FcChar8 *) "TrueType");

	FcObjectSet *objects = FcObjectSetCreate();

	FcObjectSetAdd(objects, FC_FILE);
	FcObjectSetAdd(objects, FC_SIZE);
	FcObjectSetAdd(objects, FC_INDEX);

	FcFontSet *fonts = FcFontList(NULL, pattern, objects);
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

		FcPattern *pattern = FcNameParse((FcChar8 *) pattern);

		self->font = loadFont(pattern);
		assert(self->font);

		FcPatternDestroy(pattern);
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

		FcPattern *pattern = FcPatternCreate();

		FcPatternAddString(pattern, FC_FAMILY, (FcChar8 *) family);
		FcPatternAddDouble(pattern, FC_SIZE, (double) ptsize);

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

		self->font = loadFont(pattern);
		assert(self->font);

		FcPatternDestroy(pattern);
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
