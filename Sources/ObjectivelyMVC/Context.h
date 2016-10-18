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

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_video.h>

#include <Objectively/Object.h>

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief An OpenGL 2.1 context.
 */

extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

typedef struct Context Context;
typedef struct ContextInterface ContextInterface;

/**
 * @brief An OpenGL 2.1 context.
 * @extends Object
 */
struct Context {
	
	/**
	 * @brief The parent.
	 */
	Object object;
	
	/**
	 * @brief The typed interface.
	 * @protected
	 */
	ContextInterface *interface;

	/**
	 * @brief The backing context.
	 */
	SDL_GLContext *context;
};

/**
 * @brief The Context interface.
 */
struct ContextInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;
	
	/**
	 * @fn Context *Context::initWithContext(Context *self, SDL_GLContext *context)
	 * @brief Initializes this Context with the backing SDL_GLContext.
	 * @param The Context.
	 * @param context The backing SDL_GLContext.
	 * @return The initialized Context, or `NULL` on error.
	 * @memberof Context
	 */
	Context *(*initWithContext)(Context *self, SDL_GLContext *context);
};

/**
 * @brief The Context Class.
 */
extern Class _Context;

