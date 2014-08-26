/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @file SDL_mvc.h
 * @author jdolan
 */

#ifndef _SDL_mvc_h
#define _SDL_mvc_h

#include <SDL2/SDL_ttf.h>

typedef _Bool BOOL;

typedef struct _MVC_View MVC_View;

typedef BOOL (*ViewInit)(MVC_View *self, void *data);
typedef BOOL (*ViewHandle)(MVC_View *self, SDL_Event *event);
typedef void (*ViewRender)(MVC_View *self);
typedef void (*ViewDestroy)(MVC_View *self);

/*
 * @brief
 */
struct _MVC_View {
	const char *name;

	ViewInit init;
	ViewHandle handle;
	ViewDestroy destroy;
	ViewRender render;

	struct hierarchical {
		MVC_View *parent;
		MVC_View **children;
		MVC_View *next, *prev;
	};

	struct visual {
		_Bool is_visible;
		SDL_Rect rect;
	};

	void *opaque;
};

typedef struct _MVC_ViewController MVC_ViewController;

typedef int (*ViewControllerInit)(MVC_ViewController *self);
typedef int (*ViewControllerActivate)(MVC_ViewController *self);
typedef int (*ViewControllerDeactivate)(MVC_ViewController *self);
typedef int (*ViewControllerDestroy)(MVC_ViewController *self);
/*
 * @brief
 */
struct _MVC_ViewController {
	const char *name;

	ViewControllerInit init;
	ViewControllerActivate activate;
	ViewControllerDeactivate deactivate;
	ViewControllerDestroy destroy;

	SDL_Window *window;
	MVC_View *view;
};

extern DECLSPEC int SDLCALL MVC_Init(void);
extern DECLSPEC void SDLCALL MVC_Quit(void);

extern DECLSPEC MVC_ViewController * SDLCALL MVC_CreateViewController(const char *name);
extern DECLSPEC void SDLCALL MVC_DestroyViewController(MVC_ViewController *controller);

extern DECLSPEC MVC_View * SDLCALL MVC_CreateView(const char *name);
extern DECLSPEC void SDLCALL MVC_DestroyView(MVC_View *view);

extern DECLSPEC void SDLCALL MVC_AddChildView(MVC_View *parent, MVC_View *child);
extern DECLSPEC void SDLCALL MVC_RemoveChildView(MVC_View *parent, MVC_View *child);

extern DECLSPEC void SDLCALL MVC_SwitchToViewController(MVC_ViewController *controller);

#endif
