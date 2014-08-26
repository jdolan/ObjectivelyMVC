/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _SDL_view_h
#define _SDL_view_h

#include <SDL2/SDL_ttf.h>

typedef struct _MVC_View MVC_View;

typedef void (*ViewInit)(MVC_View *self, void *data);
typedef SDL_bool (*ViewHandle)(MVC_View *self, SDL_Event *event);
typedef void (*ViewRender)(MVC_View *self);
typedef void (*ViewDestroy)(MVC_View *self);

/*
 * @brief
 */
struct _MVC_View {
	char *name;

	ViewInit init;
	ViewHandle handle;
	ViewDestroy destroy;
	ViewRender render;

	struct {
		MVC_View *parent;
		MVC_View **children;
		MVC_View *next, *prev;
	} hierarchical;

	struct {
		_Bool is_visible;
		SDL_Rect rect;
	} visual;

	void *opaque;
};

/*
 * @brief
 */
extern DECLSPEC MVC_View * SDLCALL MVC_CreateView(const char *name);

/*
 * @brief
 */
extern DECLSPEC void SDLCALL MVC_DestroyView(MVC_View *view);

/*
 * @brief
 */
extern DECLSPEC void SDLCALL MVC_AddChildView(MVC_View *parent, MVC_View *child);

/*
 * @brief
 */
extern DECLSPEC void SDLCALL MVC_RemoveChildView(MVC_View *parent, MVC_View *child);

#endif
