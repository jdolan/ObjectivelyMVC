/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and C.
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

#include <Objectively/Array.h>
#include <Objectively/Dictionary.h>

#include <ObjectivelyMVC/BitmapFont.h>
#include <ObjectivelyMVC/Font.h>
#include <ObjectivelyMVC/Stylesheet.h>

/**
 * @file
 * @brief The Theme type.
 */

/**
 * @defgroup Theme Theming
 * @brief A Theme is a collection of one or more Stylesheets, ordered by priority.
 * @details Theming is a core part of the layout process, managed automatically by the
 * WindowController. In general, applications should not call the Theme API directly. Instead,
 * custom theming can be achieved by adding a Stylesheet to any Theme instance.
 */

typedef struct Theme Theme;
typedef struct ThemeInterface ThemeInterface;

/**
 * @brief The Theme type.
 * @extends Object
 * @ingroup Theme
 */
struct Theme {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface type.
   * @protected
   */
  ThemeInterface *interface[0];

  /**
   * @brief The BitmapFont instance cache, keyed by `Font::name` + pixel density.
   * @private
   */
  Dictionary *bitmapFontCache;

  /**
   * @brief The Font instance cache, keyed by `Font::name` + pixel density.
   * @private
   */
  Dictionary *fontCache;

  /**
   * @brief The Stylesheets, in order of priority.
   */
  Array *stylesheets;
};

/**
 * @brief The Theme interface.
 */
struct ThemeInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn void Theme::addStylesheet(Theme *self, Stylesheet *stylesheet)
   * @brief Adds the specified Stylesheet to this Theme.
   * @param self The Theme.
   * @param stylesheet The Stylesheet.
   * @memberof Theme
   */
  void (*addStylesheet)(Theme *self, Stylesheet *stylesheet);

  /**
   * @fn BitmapFont *Theme::bitmapFont(Theme *self, const FontAttributes *attributes, float pixelDensity)
   * @brief Resolves a BitmapFont with the given attributes, baking and caching it if needed.
   * @details Baked with BITMAP_FONT_DEFAULT_FIRST/BITMAP_FONT_DEFAULT_COUNT into a private
   * ImageAtlas. Instances are cached by this Theme, keyed by attributes and pixel density, so
   * every View requesting the same combination shares one BitmapFont (and its atlas/texture).
   * @param self The Theme.
   * @param attributes The FontAttributes.
   * @param pixelDensity The pixel density.
   * @return The BitmapFont, owned by this Theme, or `NULL` if the resolved Font is not
   * fixed-width. Callers retaining it beyond the current frame MUST re-resolve after a pixel
   * density change.
   * @memberof Theme
   */
  BitmapFont *(*bitmapFont)(Theme *self, const FontAttributes *attributes, float pixelDensity);

  /**
   * @fn void Theme::apply(const Theme *self, const View *view)
   * @brief Applies this Theme to the given View.
   * @param self The Theme.
   * @param view The View.
   * @memberof Theme
   */
  Style *(*computeStyle)(const Theme *self, const View *view);

  /**
   * @fn Font *Theme::font(Theme *self, const FontAttributes *attributes, float pixelDensity)
   * @brief Resolves the Font with the given attributes at the given pixel density.
   * @details Instances are cached by this Theme, keyed by attributes and pixel density, so
   * repeated requests -- including from multiple windows sharing this Theme -- return the
   * same Font.
   * @param self The Theme.
   * @param attributes The FontAttributes.
   * @param pixelDensity The pixel density.
   * @return The Font, owned by this Theme. Callers retaining it beyond the current frame MUST
   * re-resolve after a pixel density change.
   * @memberof Theme
   */
  Font *(*font)(Theme *self, const FontAttributes *attributes, float pixelDensity);

  /**
   * @fn Theme *Theme::init(Theme *self)
   * @brief Initializes this Theme.
   * @param self The Theme.
   * @return The initialized Theme, or `NULL` on error.
   * @memberof Theme
   */
  Theme *(*init)(Theme *self);

  /**
   * @fn void Theme::removeStylesheet(Theme *self, Stylesheet *stylesheet)
   * @brief Removes the given Stylesheet from this Theme.
   * @param self The Theme.
   * @param stylesheet The Stylesheet.
   * @memberof Theme
   */
  void (*removeStylesheet)(Theme *self, Stylesheet *stylesheet);

  /**
   * @fn void Theme::renderDeviceDidReset(Theme *self)
   * @brief Notifies this Theme that the render device has been reset.
   * @param self The Theme.
   * @memberof Theme
   */
  void (*renderDeviceDidReset)(Theme *self);

  /**
   * @fn void Theme::renderDeviceWillReset(Theme *self)
   * @brief Releases the GPU resources of every cached BitmapFont ahead of a render device reset.
   * @details Every View drawing from a BitmapFont it holds directly already forwards this call
   * to that BitmapFont's atlas, but a BitmapFont cached here can outlive every View that
   * referenced it, so this Theme MUST also forward the call to every BitmapFont it still owns.
   * The call is idempotent, so the duplication with any given View's own forwarding is harmless.
   * @param self The Theme.
   * @memberof Theme
   */
  void (*renderDeviceWillReset)(Theme *self);

  /**
   * @static
   * @fn Theme *Theme::theme(SDL_Window *window)
   * @param window The window.
   * @return The Theme for the given window.
   * @memberof Theme
   */
  Theme *(*theme)(SDL_Window *window);
};

/**
 * @fn Class *Theme::_Theme(void)
 * @brief The Theme archetype.
 * @return The Theme Class.
 * @memberof Theme
 */
OBJECTIVELYMVC_EXPORT Class *_Theme(void);
