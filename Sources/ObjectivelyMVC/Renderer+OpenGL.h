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

#pragma once

#if __has_include(<TargetConditionals.h>)
#  include <TargetConditionals.h>
#endif

#if defined(TARGET_OS_IOS) && TARGET_OS_IOS
#  include <OpenGLES/ES3/gl.h>
#  include <OpenGLES/ES3/glext.h>
// OpenGL ES headers don't provide PFNGL*PROC typedefs; define them for the
// shared function-pointer table used by MVC_LoadGL / SDL_GL_GetProcAddress.
typedef void (*PFNGLGENVERTEXARRAYSPROC)(GLsizei, GLuint *);
typedef void (*PFNGLDELETEVERTEXARRAYSPROC)(GLsizei, const GLuint *);
typedef void (*PFNGLBINDVERTEXARRAYPROC)(GLuint);
typedef void (*PFNGLGENBUFFERSPROC)(GLsizei, GLuint *);
typedef void (*PFNGLDELETEBUFFERSPROC)(GLsizei, const GLuint *);
typedef void (*PFNGLBINDBUFFERPROC)(GLenum, GLuint);
typedef void (*PFNGLBUFFERDATAPROC)(GLenum, GLsizeiptr, const void *, GLenum);
typedef void (*PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint);
typedef void (*PFNGLVERTEXATTRIBPOINTERPROC)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *);
typedef GLuint (*PFNGLCREATESHADERPROC)(GLenum);
typedef void (*PFNGLSHADERSOURCEPROC)(GLuint, GLsizei, const GLchar *const *, const GLint *);
typedef void (*PFNGLCOMPILESHADERPROC)(GLuint);
typedef void (*PFNGLGETSHADERIVPROC)(GLuint, GLenum, GLint *);
typedef void (*PFNGLGETSHADERINFOLOGPROC)(GLuint, GLsizei, GLsizei *, GLchar *);
typedef void (*PFNGLDELETESHADERPROC)(GLuint);
typedef GLuint (*PFNGLCREATEPROGRAMPROC)(void);
typedef void (*PFNGLATTACHSHADERPROC)(GLuint, GLuint);
typedef void (*PFNGLLINKPROGRAMPROC)(GLuint);
typedef void (*PFNGLGETPROGRAMIVPROC)(GLuint, GLenum, GLint *);
typedef void (*PFNGLGETPROGRAMINFOLOGPROC)(GLuint, GLsizei, GLsizei *, GLchar *);
typedef void (*PFNGLUSEPROGRAMPROC)(GLuint);
typedef void (*PFNGLDELETEPROGRAMPROC)(GLuint);
typedef GLint (*PFNGLGETUNIFORMLOCATIONPROC)(GLuint, const GLchar *);
typedef void (*PFNGLUNIFORM1IPROC)(GLint, GLint);
typedef void (*PFNGLUNIFORM4FPROC)(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
typedef void (*PFNGLUNIFORMMATRIX4FVPROC)(GLint, GLsizei, GLboolean, const GLfloat *);
typedef void (*PFNGLACTIVETEXTUREPROC)(GLenum);
#else
#  include <SDL3/SDL_opengl.h>
#  include <SDL3/SDL_opengl_glext.h>
#endif

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief OpenGL 3.3 Core / ES 3.0 convenience layer for Renderer and embedding applications.
 * @details Provides the `OpenGL` function pointer table (populated by `MVC_LoadGL` during
 * Renderer initialization) and high-level helpers for shader compilation and program linking.
 * Any code sharing a GL context with a `Renderer` may use these directly.
 */

/**
 * @brief GL 3.3 Core / ES 3.0 function pointer table.
 * Populated by `MVC_LoadGL` when the Renderer initializes or the context is reset.
 * Call sites use `gl.bindBuffer(...)` etc.
 */
typedef struct {

  // Vertex array objects
  PFNGLGENVERTEXARRAYSPROC         GenVertexArrays;
  PFNGLDELETEVERTEXARRAYSPROC      DeleteVertexArrays;
  PFNGLBINDVERTEXARRAYPROC         BindVertexArray;

  // Buffer objects
  PFNGLGENBUFFERSPROC              GenBuffers;
  PFNGLDELETEBUFFERSPROC           DeleteBuffers;
  PFNGLBINDBUFFERPROC              BindBuffer;
  PFNGLBUFFERDATAPROC              BufferData;

  // Vertex attributes
  PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray;
  PFNGLVERTEXATTRIBPOINTERPROC     VertexAttribPointer;

  // Shader compilation
  PFNGLCREATESHADERPROC            CreateShader;
  PFNGLSHADERSOURCEPROC            ShaderSource;
  PFNGLCOMPILESHADERPROC           CompileShader;
  PFNGLGETSHADERIVPROC             GetShaderiv;
  PFNGLGETSHADERINFOLOGPROC        GetShaderInfoLog;
  PFNGLDELETESHADERPROC            DeleteShader;

  // Program linking
  PFNGLCREATEPROGRAMPROC           CreateProgram;
  PFNGLATTACHSHADERPROC            AttachShader;
  PFNGLLINKPROGRAMPROC             LinkProgram;
  PFNGLGETPROGRAMIVPROC            GetProgramiv;
  PFNGLGETPROGRAMINFOLOGPROC       GetProgramInfoLog;
  PFNGLUSEPROGRAMPROC              UseProgram;
  PFNGLDELETEPROGRAMPROC           DeleteProgram;

  // Uniforms
  PFNGLGETUNIFORMLOCATIONPROC      GetUniformLocation;
  PFNGLUNIFORM1IPROC               Uniform1i;
  PFNGLUNIFORM4FPROC               Uniform4f;
  PFNGLUNIFORMMATRIX4FVPROC        UniformMatrix4fv;

  // Textures
  PFNGLACTIVETEXTUREPROC           ActiveTexture;

} OpenGL;

/**
 * @brief The GL function pointer table. Populated by `MVC_LoadGL`.
 */
OBJECTIVELYMVC_EXPORT OpenGL gl;

/**
 * @brief Loads all GL function pointers into `*gl` via `SDL_GL_GetProcAddress`.
 * @param gl The OpenGL table to populate. Pass `&gl` to populate the default instance.
 * @remark Must be called with a current GL context. Called automatically by Renderer on init
 * and on context reset; applications do not normally need to call this directly.
 */
OBJECTIVELYMVC_EXPORT void MVC_LoadGL(OpenGL *gl);

/**
 * @brief Compiles a shader from a body-only GLSL source string.
 * @details Prepends the correct `#version` directive for the active context
 * (`#version 330 core` on desktop, `#version 300 es` on ES).
 * @param type `GL_VERTEX_SHADER` or `GL_FRAGMENT_SHADER`.
 * @param name The shader name, for error reporting.
 * @param source Null-terminated GLSL source without a `#version` line.
 * @return The compiled shader name, or 0 on error.
 */
OBJECTIVELYMVC_EXPORT GLuint MVC_CompileShader(GLenum type, const char *name, const char *source);

/**
 * @brief Links a program from a compiled vertex and fragment shader.
 * @details Attaches, links, and deletes both shaders. On link failure the
 * program is deleted and 0 is returned.
 * @param vs Compiled vertex shader (consumed; deleted by this function).
 * @param fs Compiled fragment shader (consumed; deleted by this function).
 * @return The linked program name, or 0 on error.
 */
OBJECTIVELYMVC_EXPORT GLuint MVC_LinkProgram(GLuint vs, GLuint fs);
