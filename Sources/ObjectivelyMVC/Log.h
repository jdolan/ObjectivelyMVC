/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
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

#include <SDL2/SDL_log.h>

/**
 * @file
 * @brief View logging facilities via SDL_Log.
 */

#define LOG_CATEGORY_MVC (SDL_LOG_CATEGORY_CUSTOM + 69)

#define MVC_LogSetPriority(priority) SDL_LogSetPriority(LOG_CATEGORY_MVC, priority)

#define MVC_LogEnabled(priority) \
	(SDL_LogGetPriority(LOG_CATEGORY_MVC) <= priority)

#define MVC_LogMessage(priority, fmt, ...) \
	SDL_LogMessage(LOG_CATEGORY_MVC, priority, "%s:%s "fmt, _Class()->name, __func__, ## __VA_ARGS__)

#define MVC_LogVerbose(fmt, ...) \
	SDL_LogVerbose(LOG_CATEGORY_MVC, "%s:%s: "fmt, _Class()->name, __func__, ## __VA_ARGS__)

#define MVC_LogDebug(fmt, ...) \
	SDL_LogDebug(LOG_CATEGORY_MVC, "%s:%s: "fmt, _Class()->name, __func__, ## __VA_ARGS__)

#define MVC_LogInfo(fmt, ...) \
	SDL_LogInfo(LOG_CATEGORY_MVC, "%s:%s: "fmt, _Class()->name, __func__, ## __VA_ARGS__)

#define MVC_LogWarn(fmt, ...) \
	SDL_LogWarn(LOG_CATEGORY_MVC, "%s:%s: "fmt, _Class()->name, __func__, ## __VA_ARGS__)

#define MVC_LogError(fmt, ...) \
	SDL_LogError(LOG_CATEGORY_MVC, "%s:%s: "fmt, _Class()->name, __func__, ## __VA_ARGS__)

#define MVC_LogCritical(fmt, ...) \
	SDL_LogCritical(LOG_CATEGORY_MVC, "%s:%s: "fmt, _Class()->name, __func__, ## __VA_ARGS__)
