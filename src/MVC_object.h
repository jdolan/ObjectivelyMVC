/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_class_h
#define _MVC_class_h

#include "MVC_stdinc.h"

#define Interface(Class, SuperClass) \
	typedef struct _##Class Class; \
	struct _##Class { \
		SuperClass super; \
		\
		void (*dealloc)(Class *self);

#define End };

#define Constructor(Class, ...) \
	extern Class * Class##_init(Class *self, ## __VA_ARGS__);

#define New(Class, ...) \
	Class##_init(g_new0(Class, 1), ## __VA_ARGS__)

Interface(MVC_Object, void *)

End

Constructor(MVC_Object);

#endif
