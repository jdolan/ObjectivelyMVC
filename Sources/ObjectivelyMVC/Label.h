/*
 * Objectively - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _ObjectivelyMVC_Label_h_
#define _ObjectivelyMVC_Label_h_

#include <ObjectivelyMVC/View.h>
#include <ObjectivelyMVC/Font.h>

/**
 * @file
 *
 * @brief Labels.
 */

typedef struct Label Label;
typedef struct LabelInterface LabelInterface;

/**
 * @brief Labels.
 *
 * @extends View
 *
 * @ingroup View
 */
struct Label {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	View view;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	LabelInterface *interface;

	/**
	 * @brief The Font.
	 */
	Font *font;

	/**
	 * @brief The text color.
	 */
	SDL_Color color;

	/**
	 * @brief The text.
	 */
	char *text;

	/**
	 * @brief The rendered texture.
	 */
	SDL_Texture *texture;
};

/**
 * @brief The Label interface.
 */
struct LabelInterface {

	/**
	 * @brief The parent interface.
	 */
	ViewInterface viewInterface;

	/**
	 * @fn void Label::getSize(const Label *self, int *width, int *height)
	 *
	 * @brief Resolves the rendered size of this Label.
	 *
	 * @param width The rendered width.
	 * @param height The rendered height.
	 *
	 * @memberof Label
	 */
	void (*getSize)(const Label *self, int *width, int *height);

	/**
	 * @fn Label *Label::initWithText(Label *self, const char *text)
	 *
	 * @param text The text.
	 * @param font The Font (optional).
	 *
	 * @return The initialized Label, or `NULL` on error.
	 *
	 * @memberof Label
	 */
	Label *(*initWithText)(Label *self, const char *text, Font *font);

	/**
	 * @fn void Label::render(Label *self)
	 *
	 * @brief Renders this Label's texture so that it may be drawn.
	 *
	 * @param self The Label.
	 *
	 * @memberof Label
	 */
	void (*render)(Label *self);

	/**
	 * @fn void Label::setText(Label *self, const char *text)
	 *
	 * @brief Sets this Label's text, prompting it to be re-rendered.
	 *
	 * @param text The text to set.
	 *
	 * @memberof Label
	 */
	void (*setText)(Label *self, const char *text);
};

extern Class _Label;

#endif
