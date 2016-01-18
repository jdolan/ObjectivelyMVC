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

#include <Objectively/MutableArray.h>
#include <Objectively/Once.h>
#include <Objectively/String.h>

#include <ObjectivelyMVC/Font.h>

#define _Class _Font

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Font *this = (Font *) self;

	TTF_CloseFont(this->font);

	free(this->name);

	super(Object, self, dealloc);
}

#pragma mark - Font

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
	
	FcObjectSet *objectSet = FcObjectSetBuild(FC_FAMILY, FC_FILE, NULL);
	FcFontSet *fontSet = FcFontList(NULL, pattern, objectSet);
	
	FcPattern **font = fontSet->fonts;
	for (int i = 0; i < fontSet->nfont; i++, font++) {
		
		char *name = (char *) FcNameUnparse(*font);
		String *string = $$(String, stringWithMemory, name, strlen(name));
		
		printf("Matched %s\n", name);
		
		$(fonts, addObject, string);
		release(string);
	}
	
	FcFontSetDestroy(fontSet);
	FcObjectSetDestroy(objectSet);
	FcPatternDestroy(pattern);
	
	return (Array *) fonts;
}

static Font *_defaultFont;

/**
 * @fn Font *Font::defaultFont(void)
 *
 * @memberof Font
 */
static Font *defaultFont(void) {
	static Once once;

	DispatchOnce(once, {
		_defaultFont = $(alloc(Font), initWithName, "Sans-12");
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
					font = TTF_OpenFontIndex((char *) path, (int) size, index);
				}
			}
		}
	}
	
	FcPatternDestroy(match);
	return font;
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
		
		self->name = (char *) FcNameUnparse(pattern);
		assert(self->name);
		
		FcPatternDestroy(pattern);
	}
	
	return self;
}

/**
 * @fn Font *Font::initWithName(Font *self, const char *name)
 *
 * @memberof Font
 */
static Font *initWithName(Font *self, const char *name) {

	self = (Font *) super(Object, self, init);
	if (self) {

		FcPattern *pattern = FcNameParse((FcChar8 *) name);

		self->font = loadFont(pattern);
		assert(self->font);
		
		self->name = (char *) FcNameUnparse(pattern);
		assert(self->name);
		
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

	((FontInterface *) clazz->interface)->allFonts = allFonts;
	((FontInterface *) clazz->interface)->defaultFont = defaultFont;
	((FontInterface *) clazz->interface)->initWithAttributes = initWithAttributes;
	((FontInterface *) clazz->interface)->initWithName = initWithName;

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
