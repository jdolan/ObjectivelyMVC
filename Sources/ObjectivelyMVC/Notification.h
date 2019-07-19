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

typedef struct Notification Notification;

/**
 * @brief The Notification type.
 * @details Notifications provide broadcast communication to all ViewControllers in a given window.
 */
struct Notification {

	/**
	 * @brief The notification name.
	 */
	int name;

	/**
	 * @brief The sender.
	 */
	ident sender;

	/**
	 * @brief The data.
	 */
	ident data;
};

/**
 * @brief The custom event type for notifications.
 */
OBJECTIVELYMVC_EXPORT Uint32 MVC_NOTIFICATION_EVENT;

/**
 * @brief Posts the Notification to all ViewControllers in the current window.
 * @param notification The Notification.
 * @see ViewController::handleNotification
 */
OBJECTIVELYMVC_EXPORT void MVC_PostNotification(const Notification *notification);
