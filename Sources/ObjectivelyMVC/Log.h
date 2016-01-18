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

#ifndef _ObjectivelyMVC_Log_h_
#define _ObjectivelyMVC_Log_h_

#include <SDL2/SDL_log.h>

#include <Objectively/MutableString.h>

#define LogCategoryMVC (SDL_LOG_CATEGORY_CUSTOM + 69)

#define LogSetPriority(pri) SDL_LogSetPriority(LogCategoryMVC, pri)

#define LogVerbose(fmt, ...) SDL_LogVerbose(LogCategoryMVC, fmt, ## __VA_ARGS__)
#define LogDebug(fmt, ...) SDL_LogDebug(LogCategoryMVC, fmt, ## __VA_ARGS__)
#define LogInfo(fmt, ...) SDL_LogInfo(LogCategoryMVC, fmt, ## __VA_ARGS__)
#define LogWarn(fmt, ...) SDL_LogWarn(LogCategoryMVC, fmt, ## __VA_ARGS__)
#define LogError(fmt, ...) SDL_LogError(LogCategoryMVC, fmt, ## __VA_ARGS__)
#define LogCritical(fmt, ...) SDL_LogCritical(LogCategoryMVC, fmt, ## __VA_ARGS__)

#endif