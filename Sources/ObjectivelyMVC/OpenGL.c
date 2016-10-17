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

#include <SDL2/SDL_video.h>

#include <ObjectivelyMVC/OpenGL.h>

PFNGLATTACHSHADERPROC glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

void initializeOpenGL(void) {

	glAttachShader = (PFNGLATTACHSHADERPROC) SDL_GL_GetProcAddress("glAttachShader");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) SDL_GL_GetProcAddress("glBindAttribLocation");
	glBindBuffer = (PFNGLBINDBUFFERPROC) SDL_GL_GetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC) SDL_GL_GetProcAddress("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC) SDL_GL_GetProcAddress("glBufferSubData");
	glCompileShader = (PFNGLCOMPILESHADERPROC) SDL_GL_GetProcAddress("glCompileShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC) SDL_GL_GetProcAddress("glCreateProgram");
	glCreateShader = (PFNGLCREATESHADERPROC) SDL_GL_GetProcAddress("glCreateShader");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) SDL_GL_GetProcAddress("glDeleteBuffers");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC) SDL_GL_GetProcAddress("glDeleteProgram");
	glDeleteShader = (PFNGLDELETESHADERPROC) SDL_GL_GetProcAddress("glDeleteShader");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) SDL_GL_GetProcAddress("glDisableVertexAttribArray");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) SDL_GL_GetProcAddress("glEnableVertexAttribArray");
	glGenBuffers = (PFNGLGENBUFFERSPROC) SDL_GL_GetProcAddress("glGenBuffers");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) SDL_GL_GetProcAddress("glGetAttribLocation");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) SDL_GL_GetProcAddress("glGetProgramInfoLog");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC) SDL_GL_GetProcAddress("glGetProgramiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) SDL_GL_GetProcAddress("glGetShaderInfoLog");
	glGetShaderiv = (PFNGLGETSHADERIVPROC) SDL_GL_GetProcAddress("glGetShaderiv");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) SDL_GL_GetProcAddress("glGetUniformLocation");
	glLinkProgram = (PFNGLLINKPROGRAMPROC) SDL_GL_GetProcAddress("glLinkProgram");
	glShaderSource = (PFNGLSHADERSOURCEPROC) SDL_GL_GetProcAddress("glShaderSource");
	glUniform4fv = (PFNGLUNIFORM4FVPROC) SDL_GL_GetProcAddress("glUniform4fv");
	glUseProgram = (PFNGLUSEPROGRAMPROC) SDL_GL_GetProcAddress("glUseProgram");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) SDL_GL_GetProcAddress("glVertexAttribPointer");
}
