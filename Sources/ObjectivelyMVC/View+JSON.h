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

#pragma once

#include <Objectively/Data.h>
#include <Objectively/Enum.h>

#include <ObjectivelyMVC/View.h>

/**
 * @file
 *
 * @brief The JSONView type
 */

/**
 * @brief Inlet type constants.
 */
typedef enum {
	InletTypeBool,
	InletTypeCharacters,
	InletTypeColor,
	InletTypeDouble,
	InletTypeEnum,
	InletTypeFloat,
	InletTypeFont,
	InletTypeImage,
	InletTypeInteger,
	InletTypeRectangle,
	InletTypeSize,
	InletTypeSubviews,
	InletTypeView,
} InletType;

typedef struct Inlet Inlet;

/**
 * @brief Inlets allow the resolution of View attributes via JSONView.
 */
struct Inlet {

	/**
	 * @brief The Inlet name, e.g. `"alignment"`.
	 */
	const char *name;

	/**
	 * @brief The InletType, e.g. InletTypeEnum.
	 */
	InletType type;

	/**
	 * @brief The Inlet destination.
	 */
	ident dest;

	/**
	 * @brief Type-specific data, e.g. an array of EnumNames.
	 */
	ident data;
};

/**
 * @brief Creates an Inlet with the specified parameters.
 */
#define MakeInlet(name, type, dest, data) \
	({ \
		Inlet _inlet = { (name), (type), (dest), (data) }; _inlet; \
	})

/**
 * @brief Creates a null-termianted array of Inlets.
 */
#define MakeInlets(...) \
	({ \
		const Inlet _inlets[] = { \
			__VA_ARGS__, \
			MakeInlet(NULL, -1, NULL, NULL) \
		}; \
		_inlets; \
	})

typedef struct Outlet Outlet;

/**
 * @brief Outlets allow the resolution of Views via JSONView.
 */
struct Outlet {

	/**
	 * @brief The View identifier.
	 */
	const char *identifier;

	/**
	 * @brief The output storage for the resolved View.
	 */
	View **view;
};

/**
 * @brief Creates an Outlet with the specified parameters.
 */
#define MakeOutlet(identifier, view) \
	({ \
		Outlet _outlet = { (identifier), (View **) (view) }; _outlet; \
	})

typedef struct JSONView JSONView;
typedef struct JSONViewInterface JSONViewInterface;

/**
 * @brief JSONView allows loading of View hierarchies from `.json` files.
 *
 * @extends Object
 */
struct JSONView {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Object object;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	JSONViewInterface *interface;
};

/**
 * @brief The JSONView interface.
 */
struct JSONViewInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 *
	 * @fn void JSONView::applyInlets(View *view, const Dictionary *dictionary, const Inlet *inlets)
	 *
	 * @brief Applies the specified Inlets, populated from `dictionary`, to the given View.
	 *
	 * @param view A View.
	 * @param dictionary A Dictionary describing the View.
	 * @param inlets An optional array of Inlets to apply.
	 *
	 * @memberof JSONView
	 */
	void (*applyInlets)(View *view, const Dictionary *dictionary, const Inlet *inlets);

	/**
	 * @static
	 *
	 * @fn View *JSONView::viewWithContentsOfFile(const char *path)
	 *
	 * @brief Instantiates a View initialized with the contents of the JSON file at `path`.
	 *
	 * @param path A path to a JSON file describing a View.
	 * @param outlets An optional array of Outlets to resolve.
	 *
	 * @return The initialized View, or `NULL` on error.
	 *
	 * @memberof JSONView
	 */
	View *(*viewWithContentsOfFile)(const char *path, Outlet *outlets);

	/**
	 * @static
	 *
	 * @fn View *JSONView::viewWithData(const Data *data)
	 *
	 * @brief Instantiates a View initialized with the contents of `data`.
	 *
	 * @param dictionary A Dictionary describing the View.
	 * @param outlets An optional array of Outlets to resolve.
	 *
	 * @return The initialized View, or `NULL` on error.
	 *
	 * @memberof JSONView
	 */
	View *(*viewWithData)(const Data *data, Outlet *outlets);

	/**
	 * @static
	 *
	 * @fn View *JSONView::viewWithDictionary(const Dictionary *dictionary, Outlet *outlets)
	 *
	 * @brief Instantiates a View initialized with the attributes described in `dictionary`.
	 *
	 * @param dictionary A Dictionary describing the View.
	 * @param outlets An optional array of Outlets to resolve.
	 *
	 * @return The initialized View, or `NULL` on error.
	 *
	 * @see View::initWithDictionary(View *, const Dictionary *, Outlet *outlets)
	 *
	 * @memberof JSONView
	 */
	View *(*viewWithDictionary)(const Dictionary *dictionary, Outlet *outlets);
};

/**
 * @brief The JSONView Class.
 */
extern Class _JSONView;

