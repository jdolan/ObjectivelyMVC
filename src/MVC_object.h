/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_object_h
#define _MVC_object_h

#include "MVC_stdinc.h"

#define Interface(Class, SuperClass) \
	typedef struct _##Class Class; \
	struct _##Class { \
		SuperClass super; \
		\
		void (*dealloc)(Class *self);

#define End };

#define Overrides(Class) \
	typedef struct _##Class##_Overrides Class##_Overrides; \
	static Class##_Overrides Class##_overrides; \
	struct _##Class##_Overrides {

#define Override(Class, method, implementation) \
	Class##_overrides.method = self->super.method; \
	self->super.method = implementation;

#define Constructor(Class, ...) \
	extern Class * Class##_init(Class *self, ## __VA_ARGS__);

#define New(Class, ...) \
	Class##_init(g_new0(Class, 1), ## __VA_ARGS__)

Interface(MVC_Object, void *)

End

Constructor(MVC_Object);

#endif
