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

#include <assert.h>
#include <Objectively/Once.h>

#include <ObjectivelyMVC/Notification.h>

Uint32 MVC_NOTIFICATION_EVENT;

void MVC_PostNotification(const Notification *notification) {
	static Once once;

	do_once(&once, {
		MVC_NOTIFICATION_EVENT = SDL_RegisterEvents(1);
		assert(MVC_NOTIFICATION_EVENT != (Uint32) -1);
	});

	assert(notification);

	SDL_PushEvent(&(SDL_Event) {
		.user.type = MVC_NOTIFICATION_EVENT,
		.user.code = notification->name,
		.user.data1 = notification->sender,
		.user.data2 = notification->data
	});
}
