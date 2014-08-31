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

	g_list_free_full(self->hierarchy.subviews, (GDestroyNotify) MVC_View_dealloc);

	Super(Object, Object, dealloc);
}
/*
 * @brief Default implementation of addSubview.
 */
static void MVC_View_addSubview(MVC_View *self, MVC_View *subview) {

	if (subview) {
		if (g_list_index(self->hierarchy.subviews, subview) == -1) {
			self->hierarchy.subviews = g_list_append(self->hierarchy.subviews, subview);
			subview->hierarchy.superview = self;
		}
	}
}

/*
 * @brief Default implementation of removeSubview.
 */
static void MVC_View_removeSubview(MVC_View *self, MVC_View *subview) {

	if (subview) {
		if (g_list_index(self->hierarchy.subviews, subview) != -1) {
			self->hierarchy.subviews = g_list_remove(self->hierarchy.subviews, subview);
			subview->hierarchy.superview = NULL;
		}
	}
}

/*
 * @brief Default implementation of removeFromSuperview.
 */
static void MVC_View_removeFromSuperview(MVC_View *self) {

	if (self->hierarchy.superview) {
		self->hierarchy.superview->removeSubview(self->hierarchy.superview, self);
	}
}

/*
 * @brief Default implementation of draw.
 */
static void MVC_View_draw(MVC_View *self) {

}

/*
 * @brief Constructor.
 */
MVC_View *MVC_View_init(MVC_View *self, SDL_Window *window, SDL_GLContext *context) {

	if (Object_init(&self->super)) {
		self->super.dealloc = MVC_View_dealloc;

		self->addSubview = MVC_View_addSubview;
		self->removeSubview = MVC_View_removeSubview;
		self->removeFromSuperview = MVC_View_removeFromSuperview;

		self->draw = MVC_View_draw;

		memset(&self->display.backgroundColor, 255, sizeof(SDL_Color));

		self->display.window = window ? window : SDL_GL_GetCurrentWindow();
		self->display.context = context ? context : SDL_GL_GetCurrentContext();
	}

	return self;
}
