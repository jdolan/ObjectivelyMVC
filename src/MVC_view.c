/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "MVC_view.h"

static void MVC_View_dealloc(Object *object) {

	MVC_View *self = (MVC_View *) object;

	self->removeFromSuperview(self);

	g_list_free_full(self->subviews, (GDestroyNotify) MVC_View_dealloc);

	Super(Object, Object, object, dealloc);
}
/*
 * @brief Default implementation of addSubview.
 */
static void MVC_View_addSubview(MVC_View *self, MVC_View *subview) {

	if (subview) {
		if (g_list_index(self->subviews, subview) == -1) {
			self->subviews = g_list_append(self->subviews, subview);
			subview->superview = self;
		}
	}
}

/*
 * @brief Default implementation of removeSubview.
 */
static void MVC_View_removeSubview(MVC_View *self, MVC_View *subview) {

	if (subview) {
		if (g_list_index(self->subviews, subview) != -1) {
			self->subviews = g_list_remove(self->subviews, subview);
			subview->superview = NULL;
		}
	}
}

/*
 * @brief Default implementation of removeFromSuperview.
 */
static void MVC_View_removeFromSuperview(MVC_View *self) {

	if (self->superview) {
		self->superview->removeSubview(self->superview, self);
	}
}

/*
 * @brief Default implementation of draw.
 */
static void MVC_View_draw(MVC_View *self) {

	if (self->backgroundColor.a) {

		SDL_Color c = self->backgroundColor;
		SDL_SetRenderDrawColor(self->renderer, c.r, c.g, c.b, c.a);

		SDL_RenderFillRect(self->renderer, &self->frame);

		SDL_Color w = MVC_Colors.White;
		SDL_SetRenderDrawColor(self->renderer, w.r, w.g, w.b, w.a);
	}
}

/*
 * @brief Constructor.
 */
Implementation(MVC_View, SDL_Rect *frame)

	Initialize(MVC_View, NULL);

	if (Object_init(&self->super)) {

		self->window = SDL_GL_GetCurrentWindow();
		self->renderer = SDL_GetRenderer(self->window);
		if (self->renderer == NULL) {
			self->renderer = SDL_CreateRenderer(self->window, -1, SDL_RENDERER_ACCELERATED);
			if (self->renderer == NULL) {
				SDL_SetError("%s: Failed to create renderer", __func__);
			}
		}

		if (frame) {
			self->frame = *frame;
		} else {
			SDL_LogWarn(0, "%s: NULL frame", __func__);
			SDL_GetWindowSize(self->window, &self->frame.w, &self->frame.h);
		}

		self->backgroundColor = MVC_Colors.Clear;

		Override(Object, dealloc, MVC_View_dealloc);

		self->addSubview = MVC_View_addSubview;
		self->removeSubview = MVC_View_removeSubview;
		self->removeFromSuperview = MVC_View_removeFromSuperview;

		self->draw = MVC_View_draw;
	}

	return self;

End
