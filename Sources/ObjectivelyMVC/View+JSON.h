/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief JSON data binding for Views.
 */

/**
 * @brief Inlet type constants.
 */
typedef enum {

	/**
	 * @remarks Special end-of-list value for an inlet list
	 */
	InitTypeEnd = -1,

	/**
	 * @remarks Inlet destination must be of type `Bool *`.
	 */
	InletTypeBool,

	/**
	 * @remarks Inlet destination must be of type `char **`. The inbound C string is copied via
	 * `strdup`, and thus the receiver should `free` it when it is no longer needed.
	 */
	InletTypeCharacters,

	/**
	 * @remarks Inlet destination must be of type `View **`.
	 */
	InletTypeClassNames,

	/**
	 * @remarks Inlet destination must be of type `SDL_Color *`.
	 */
	InletTypeColor,

	/**
	 * @remarks Inlet destination must be of type `double *`.
	 */
	InletTypeDouble,

	/**
	 * @remarks Inlet destination must be of an `enum *` type. Inlet data must provide a null-
	 * terminated array of EnumNames.
	 * @see valueof
	 * @see MakeEnumNames
	 */
	InletTypeEnum,

	/**
	 * @remarks Inlet destination must be of type `float *`.
	 */
	InletTypeFloat,

	/**
	 * @remarks Inlet destination must be of type `Font **`.
	 */
	InletTypeFont,

	/**
	 * @remarks Inlet destination must be of type `Image **`.
	 */
	InletTypeImage,

	/**
	 * @remarks Inlet destination must be of type `int *`.
	 */
	InletTypeInteger,

	/**
	 * @remarks Inlet destination must be of type `SDL_Point *`.
	 */
	InletTypePoint,

	/**
	 * @remarks Inlet destination must be of type `SDL_Rect *`.
	 */
	InletTypeRectangle,

	/**
	 * @remarks Inlet destination must be of type `SDL_Size *`.
	 */
	InletTypeSize,

	/**
	 * @remarks Inlet destination must be of type `View **`. The Style of the specified View is
	 * populated with the attributes of the bound Dictionary.
	 */
	InletTypeStyle,

	/**
	 * @remarks Inlet destination must be of type `View **`. The subviews of the specified View are
	 * populated from the bound Array of View definitions.
	 */
	InletTypeSubviews,

	/**
	 * @remarks Inlet destination must be of type `View **`.
	 * @remarks By default, the destination View is awoken with the inbound View definition.
	 * There are two notable exceptions to this behavior:
	 *  * `"class"` - If the inbound View definition includes a `"class"` directive, a new View of
	 *      the specified type is initialized, and the existing View is replaced and released. The
	 *      type must be initialized in order for the class to be resolved. Use this behavior to
	 *      substitute a specialized or custom View implementation.
	 *  * `"include"` - If the inbound View definition specifies an `"include"` directive, the
	 *      specified JSON file will be recursively processed. The existing View is replaced with 
	 *      the resulting View, and subsequently released.
	 * @see _initialize(Clazz *)
	 * @see View::awakeWithDictionary(View *, const Dictionary *)
	 * @see View::viewWithContentsOfFile(const char *path)
	 */
	InletTypeView,

	/**
	 * @remarks Inlet destination is of an application-defined type. The Inlet data must provide
	 * an InletBinding function. That function is responsible for populating the Inlet destination.
	 */
	InletTypeApplicationDefined,

} InletType;

typedef struct Inlet Inlet;

/**
 * @brief Inlets enable inbound data binding of View attributes through JSON.
 * @see View::bind(View *, const Dictionary *, const Inlet *)
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
 * @brief A function pointer for Inlet binding.
 * @param inlet The Inlet.
 * @param obj The Object resolved from the JSON Dictionary.
 * @remarks For Inlets of type InletTypeApplicationDefined, applications must provide a pointer to
 * a function of this prototype as their Inlet's data. ObjectivelyMVC will invoke the function to
 * resolve the JSON data binding.
 */
typedef void (*InletBinding)(const Inlet *inlet, ident obj);

/**
 * @brief The Array of InletBindings, indexed by InletType.
 */
OBJECTIVELYMVC_EXPORT const InletBinding inletBindings[];

typedef struct Outlet Outlet;

/**
 * @brief Outlets enable outbound data binding of Views through JSON.
 * @remarks References returned via Outlet binding are weak, and do not `retain` the matched View.
 * @see View::viewWithDictionary(const Dictionary *, Outlet *)
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
 * @brief Creates an Inlet with the specified parameters.
 */
#define MakeInlet(name, type, dest, data) (Inlet) { (name), (type), (dest), (ident) (data) }

/**
 * @brief Creates a null-termianted array of Inlets.
 */
#define MakeInlets(...) \
	{ \
		__VA_ARGS__, \
		MakeInlet(NULL, InitTypeEnd, NULL, NULL) \
	}

/**
 * @brief Creates an Outlet with the specified parameters.
 */
#define MakeOutlet(identifier, view) (Outlet) { (identifier), (View **) (view) }

/**
 * @brief Creates a `NULL`-termianted array of Outlets.
 */
#define MakeOutlets(...) \
	{ \
		__VA_ARGS__, \
		MakeOutlet(NULL, NULL) \
	}

/**
 * @brief Binds the Inlet to `obj` by invoking the appropriate InletBinding function.
 */
#define BindInlet(inlet, obj) (inletBindings[(inlet)->type])(inlet, (ident) obj)

/**
 * @brief Binds each Inlet specified in `inlets` to the data provided in `dictionary`.
 * @param inlets The Inlets to bind.
 * @param dictionary The Dictionary from which to bind.
 * @return True if one or more Inlets were bound, false otherwise.
 */
OBJECTIVELYMVC_EXPORT bool bindInlets(const Inlet *inlets, const Dictionary *dictionary);
