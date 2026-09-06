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

#include <ObjectivelyMVC/Font.h>
#include <ObjectivelyMVC/ImageAtlas.h>
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
   * @brief The Font instance cache, keyed by `Font::name` + pixel density.
   * @private
   */
  Dictionary *fontCache;

  /**
   * @brief The ImageAtlas holding this Theme's icons and other app art, created lazily by
   * Theme::icons.
   * @private
   */
  ImageAtlas *icons;

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
   * @fn AtlasImage *Theme::addIcon(Theme *self, const char *name, Image *image)
   * @brief Registers `image` as the icon `name`, so that `:name:` in any Text drawn under this
   * Theme renders it inline, and Theme::icon resolves it.
   * @details The icon is added to Theme::icons and the atlas is compiled, which repacks it, so
   * register icons at startup rather than while drawing. AtlasImages already handed out stay
   * valid: compiling updates their rects in place. Icons SHOULD be square; Text draws them as a
   * square of the line height.
   * @param self The Theme.
   * @param name The name, of `[A-Za-z0-9_-]`, under 64 bytes.
   * @param image The Image.
   * @return The AtlasImage, owned by this Theme's icon atlas.
   * @memberof Theme
   */
  AtlasImage *(*addIcon)(Theme *self, const char *name, Image *image);

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
   * @fn AtlasImage *Theme::icon(Theme *self, const char *name)
   * @param self The Theme.
   * @param name The name.
   * @return The icon registered under `name` via Theme::addIcon, or `NULL`.
   * @memberof Theme
   */
  AtlasImage *(*icon)(Theme *self, const char *name);

  /**
   * @fn ImageAtlas *Theme::icons(Theme *self)
   * @brief Returns the ImageAtlas holding this Theme's icons, and any other app art added to it,
   * creating it on first access.
   * @details Theme::addIcon is the usual way in. Apps MAY also add unnamed art directly
   * (ImageAtlas::addImage, then ImageAtlas::compile) exactly as with any other ImageAtlas; Theme
   * owns the instance and forwards render device resets to it, so such art gets correct GPU
   * resource lifecycle for free.
   * @param self The Theme.
   * @return This Theme's icon ImageAtlas, owned by this Theme.
   * @memberof Theme
   */
  ImageAtlas *(*icons)(Theme *self);

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
   * @brief Releases the GPU resources of every cached Font's bitmap, and of Theme::icons if it
   * has been created, ahead of a render device reset.
   * @details Fonts are cached here, not owned by the Views drawing them, so this Theme is the
   * one place that reliably reaches every bitmap Texture, including one no live View references.
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
