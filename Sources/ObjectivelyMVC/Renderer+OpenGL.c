/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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

#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "Renderer+OpenGL.h"

OpenGL gl;

void MVC_LoadGL(OpenGL *gl) {

#define L(type, name) \
  if (!(gl->name = (type) SDL_GL_GetProcAddress("gl" #name))) \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load GL proc: gl" #name "\n")

  L(PFNGLGENVERTEXARRAYSPROC,          GenVertexArrays);
  L(PFNGLDELETEVERTEXARRAYSPROC,       DeleteVertexArrays);
  L(PFNGLBINDVERTEXARRAYPROC,          BindVertexArray);
  L(PFNGLGENBUFFERSPROC,               GenBuffers);
  L(PFNGLDELETEBUFFERSPROC,            DeleteBuffers);
  L(PFNGLBINDBUFFERPROC,               BindBuffer);
  L(PFNGLBUFFERDATAPROC,               BufferData);
  L(PFNGLENABLEVERTEXATTRIBARRAYPROC,  EnableVertexAttribArray);
  L(PFNGLVERTEXATTRIBPOINTERPROC,      VertexAttribPointer);
  L(PFNGLCREATESHADERPROC,             CreateShader);
  L(PFNGLSHADERSOURCEPROC,             ShaderSource);
  L(PFNGLCOMPILESHADERPROC,            CompileShader);
  L(PFNGLGETSHADERIVPROC,              GetShaderiv);
  L(PFNGLGETSHADERINFOLOGPROC,         GetShaderInfoLog);
  L(PFNGLDELETESHADERPROC,             DeleteShader);
  L(PFNGLCREATEPROGRAMPROC,            CreateProgram);
  L(PFNGLATTACHSHADERPROC,             AttachShader);
  L(PFNGLLINKPROGRAMPROC,              LinkProgram);
  L(PFNGLGETPROGRAMIVPROC,             GetProgramiv);
  L(PFNGLGETPROGRAMINFOLOGPROC,        GetProgramInfoLog);
  L(PFNGLUSEPROGRAMPROC,               UseProgram);
  L(PFNGLDELETEPROGRAMPROC,            DeleteProgram);
  L(PFNGLGETUNIFORMLOCATIONPROC,       GetUniformLocation);
  L(PFNGLUNIFORM1IPROC,                Uniform1i);
  L(PFNGLUNIFORM4FPROC,                Uniform4f);
  L(PFNGLUNIFORMMATRIX4FVPROC,         UniformMatrix4fv);
  L(PFNGLACTIVETEXTUREPROC,            ActiveTexture);

#undef L
}

GLuint MVC_CompileShader(GLenum type, const char *name, const char *source) {

  const char *gl_version = (const char *) glGetString(GL_VERSION);
  const char *prefix = strstr(gl_version, "OpenGL ES")
    ? "#version 300 es\n"
    : "#version 330 core\n";

  const GLuint shader = gl.CreateShader(type);
  const char *sources[] = { prefix, source };
  gl.ShaderSource(shader, 2, sources, NULL);
  gl.CompileShader(shader);

  GLint status;
  gl.GetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLchar log[1024];
    gl.GetShaderInfoLog(shader, sizeof(log), NULL, log);
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MVC_CompileShader: %s: %s\n", name ?: "(null)", log);
    gl.DeleteShader(shader);
    return 0;
  }

  return shader;
}

GLuint MVC_LinkProgram(GLuint vs, GLuint fs) {

  const GLuint program = gl.CreateProgram();
  gl.AttachShader(program, vs);
  gl.AttachShader(program, fs);
  gl.LinkProgram(program);
  gl.DeleteShader(vs);
  gl.DeleteShader(fs);

  GLint status;
  gl.GetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLchar log[1024];
    gl.GetProgramInfoLog(program, sizeof(log), NULL, log);
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MVC_LinkProgram: %s\n", log);
    gl.DeleteProgram(program);
    return 0;
  }

  return program;
}
