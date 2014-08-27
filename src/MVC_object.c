/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include "MVC_object.h"

MVC_Object *MVC_Object_init(MVC_Object *self) {

	self->super = NULL;
	self->dealloc = (void (*)(MVC_Object *))g_free;

	return self;
}

