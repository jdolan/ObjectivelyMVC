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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Log.h"
#include "Renderer+OpenGL.h"
#include "Renderer.h"
#include "View.h"
#include "Window.h"

#include "../Assets/renderer_fs.glsl.h"
#include "../Assets/renderer_vs.glsl.h"

#define _Class _Renderer

#define GL_GET_ERROR() do { \
  GLenum _err; \
  while ((_err = glGetError()) != GL_NO_ERROR) { \
    MVC_LogError("%s: GL error: %d\n", __func__, _err); \
    SDL_TriggerBreakpoint(); \
  } \
} while (0)

/**
 * @brief Interleaved position + texcoord vertex.
 */
typedef struct {
  GLfloat x, y;
  GLfloat u, v;
} Vertex;

/**
 * @brief Uploads vertices to the VBO and issues a draw call.
 */
static void drawVertices(const Renderer *self, GLenum mode, const Vertex *verts, GLsizei count, GLuint texture) {

  glBindTexture(GL_TEXTURE_2D, texture);

  gl.BufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), verts, GL_STREAM_DRAW);

  gl.Uniform4f(self->uniforms.color,
               self->color.r / 255.0f,
               self->color.g / 255.0f,
               self->color.b / 255.0f,
               self->color.a / 255.0f);

  glDrawArrays(mode, 0, count);
  GL_GET_ERROR();
}

#pragma mark - Renderer

/**
 * @fn void Renderer::beginFrame(Renderer *self)
 * @memberof Renderer
 */
static void beginFrame(Renderer *self) {

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_SCISSOR_TEST);

  gl.UseProgram(self->program);

  gl.BindVertexArray(self->vao);
  gl.BindBuffer(GL_ARRAY_BUFFER, self->vbo);

  SDL_Window *window = SDL_GL_GetCurrentWindow();
  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  const GLfloat projection[16] = {
     2.0f / w,  0.0f,      0.0f,  0.0f,
     0.0f,     -2.0f / h,  0.0f,  0.0f,
     0.0f,      0.0f,     -1.0f,  0.0f,
    -1.0f,      1.0f,      0.0f,  1.0f,
  };
  gl.UniformMatrix4fv(self->uniforms.projection, 1, GL_FALSE, projection);

  gl.ActiveTexture(GL_TEXTURE0);

  $(self, setDrawColor, &Colors.White);

  GL_GET_ERROR();
}

/**
 * @fn GLuint Renderer::createTexture(const Renderer *self, const SDL_Surface *surface)
 * @memberof Renderer
 */
static GLuint createTexture(const Renderer *self, const SDL_Surface *surface) {

  assert(surface);

  GLenum format;
  GLint internal_format;
  switch (SDL_BYTESPERPIXEL(surface->format)) {
    case 1:
      internal_format = GL_R8;
      format = GL_RED;
      break;
    case 3:
      internal_format = GL_RGB;
      format = GL_RGB;
      break;
    case 4:
      internal_format = GL_RGBA;
      format = GL_RGBA;
      break;
    default:
      MVC_LogError("Invalid surface format: %s\n", SDL_GetPixelFormatName(surface->format));
      return 0;
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, internal_format, surface->w, surface->h, 0,
               format, GL_UNSIGNED_BYTE, surface->pixels);

  if (internal_format == GL_R8) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
  }

  GL_GET_ERROR();
  return texture;
}

/**
 * @fn void Renderer::drawLine(const Renderer *self, const SDL_Point *points)
 * @memberof Renderer
 */
static void drawLine(const Renderer *self, const SDL_Point *points) {

  assert(points);

  $(self, drawLines, points, 2);
}

/**
 * @fn void Renderer::drawLines(const Renderer *self, const SDL_Point *points, size_t count)
 * @memberof Renderer
 */
static void drawLines(const Renderer *self, const SDL_Point *points, size_t count) {

  assert(points);

  Vertex *verts = malloc(count * sizeof(Vertex));
  assert(verts);

  for (size_t i = 0; i < count; i++) {
    verts[i] = (Vertex) { points[i].x, points[i].y, 0.0f, 0.0f };
  }

  drawVertices(self, GL_LINE_STRIP, verts, (GLsizei) count, self->white);

  free(verts);
}

/**
 * @fn void Renderer::drawRect(const Renderer *self, const SDL_Rect *rect)
 * @memberof Renderer
 */
static void drawRect(const Renderer *self, const SDL_Rect *rect) {

  assert(rect);

  const Vertex verts[4] = {
    { rect->x,           rect->y,           0.0f, 0.0f },
    { rect->x + rect->w, rect->y,           0.0f, 0.0f },
    { rect->x + rect->w, rect->y + rect->h, 0.0f, 0.0f },
    { rect->x,           rect->y + rect->h, 0.0f, 0.0f },
  };

  drawVertices(self, GL_LINE_LOOP, verts, 4, self->white);
}

/**
 * @fn void Renderer::drawRectFilled(const Renderer *self, const SDL_Rect *rect)
 * @memberof Renderer
 */
static void drawRectFilled(const Renderer *self, const SDL_Rect *rect) {

  assert(rect);

  const GLfloat x1 = rect->x - 1, y1 = rect->y - 1;
  const GLfloat x2 = rect->x + rect->w + 1, y2 = rect->y + rect->h + 1;

  const Vertex verts[4] = {
    { x1, y1, 0.0f, 0.0f },
    { x2, y1, 0.0f, 0.0f },
    { x2, y2, 0.0f, 0.0f },
    { x1, y2, 0.0f, 0.0f },
  };

  drawVertices(self, GL_TRIANGLE_FAN, verts, 4, self->white);
}

/**
 * @fn void Renderer::drawTexture(const Renderer *self, GLuint texture, const SDL_Rect *dest)
 * @memberof Renderer
 */
static void drawTexture(const Renderer *self, GLuint texture, const SDL_Rect *rect) {

  assert(rect);

  const Vertex verts[4] = {
    { rect->x,           rect->y,           0.0f, 0.0f },
    { rect->x + rect->w, rect->y,           1.0f, 0.0f },
    { rect->x + rect->w, rect->y + rect->h, 1.0f, 1.0f },
    { rect->x,           rect->y + rect->h, 0.0f, 1.0f },
  };

  drawVertices(self, GL_TRIANGLE_FAN, verts, 4, texture);
}

/**
 * @fn void Renderer::drawView(Renderer *self, View *view)
 * @memberof Renderer
 */
static void drawView(Renderer *self, View *view) {

  assert(view);

  const SDL_Rect clippingFrame = $(view, clippingFrame);
  if (clippingFrame.w && clippingFrame.h) {

    $(self, setClippingFrame, &clippingFrame);

    $(view, render, self);
  }
}

/**
 * @fn void Renderer::endFrame(Renderer *self)
 * @memberof Renderer
 */
static void endFrame(Renderer *self) {

  $(self, setDrawColor, &Colors.White);

  $(self, setClippingFrame, NULL);

  gl.BindBuffer(GL_ARRAY_BUFFER, 0);
  gl.BindVertexArray(0);
  gl.UseProgram(0);

  glDisable(GL_SCISSOR_TEST);

  glBlendFunc(GL_ONE, GL_ZERO);
  glDisable(GL_BLEND);

  GL_GET_ERROR();
}

/**
 * @fn Renderer *Renderer::init(Renderer *self)
 * @memberof Renderer
 */
static Renderer *init(Renderer *self) {

  self = (Renderer *) super(Object, self, init);
  if (self) {
    $(self, renderDeviceDidReset);
  }
  return self;
}

/**
 * @fn void Renderer::renderDeviceDidReset(Renderer *self)
 * @memberof Renderer
 */
static void renderDeviceDidReset(Renderer *self) {

  $(self, renderDeviceWillReset);

  MVC_LoadGL(&gl);

  const GLuint vs = MVC_CompileShader(GL_VERTEX_SHADER, (const char *) renderer_vs_glsl);
  const GLuint fs = MVC_CompileShader(GL_FRAGMENT_SHADER, (const char *) renderer_fs_glsl);

  self->program = MVC_LinkProgram(vs, fs);
  if (!self->program) {
    return;
  }

  self->uniforms.projection = gl.GetUniformLocation(self->program, "projection");
  self->uniforms.color = gl.GetUniformLocation(self->program, "color");

  const GLubyte white[] = { 255, 255, 255, 255 };
  glGenTextures(1, &self->white);
  glBindTexture(GL_TEXTURE_2D, self->white);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  gl.GenVertexArrays(1, &self->vao);
  gl.BindVertexArray(self->vao);

  gl.GenBuffers(1, &self->vbo);
  gl.BindBuffer(GL_ARRAY_BUFFER, self->vbo);

  gl.EnableVertexAttribArray(0);
  gl.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, x));

  gl.EnableVertexAttribArray(1);
  gl.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, u));

  gl.BindBuffer(GL_ARRAY_BUFFER, 0);
  gl.BindVertexArray(0);

  GL_GET_ERROR();
}

/**
 * @fn void Renderer::renderDeviceWillReset(Renderer *self)
 * @memberof Renderer
 */
static void renderDeviceWillReset(Renderer *self) {

  if (self->white) {
    glDeleteTextures(1, &self->white);
    self->white = 0;
  }

  if (self->vbo) {
    gl.DeleteBuffers(1, &self->vbo);
    self->vbo = 0;
  }

  if (self->vao) {
    gl.DeleteVertexArrays(1, &self->vao);
    self->vao = 0;
  }

  if (self->program) {
    gl.DeleteProgram(self->program);
    self->program = 0;
  }

  GL_GET_ERROR();
}

/**
 * @fn void Renderer::setClippingFrame(Renderer *self, const SDL_Rect *clippingFrame)
 * @memberof Renderer
 */
static void setClippingFrame(Renderer *self, const SDL_Rect *clippingFrame) {

  SDL_Window *window = SDL_GL_GetCurrentWindow();

  SDL_Rect rect;
  if (clippingFrame) {
    rect = *clippingFrame;
  } else {
    rect = MakeRect(0, 0, 0, 0);
    SDL_GetWindowSize(window, &rect.w, &rect.h);
  }

  const SDL_Rect scissor = MVC_TransformToWindow(window, &rect);

  glScissor(scissor.x - 1, scissor.y - 1, scissor.w + 2, scissor.h + 2);

  GL_GET_ERROR();
}

/**
 * @fn void Renderer::setDrawColor(Renderer *self, const SDL_Color *color)
 * @memberof Renderer
 */
static void setDrawColor(Renderer *self, const SDL_Color *color) {
  self->color = *color;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
  
  ((RendererInterface *) clazz->interface)->beginFrame = beginFrame;
  ((RendererInterface *) clazz->interface)->createTexture = createTexture;
  ((RendererInterface *) clazz->interface)->drawLine = drawLine;
  ((RendererInterface *) clazz->interface)->drawLines = drawLines;
  ((RendererInterface *) clazz->interface)->drawRect = drawRect;
  ((RendererInterface *) clazz->interface)->drawRectFilled = drawRectFilled;
  ((RendererInterface *) clazz->interface)->drawTexture = drawTexture;
  ((RendererInterface *) clazz->interface)->drawView = drawView;
  ((RendererInterface *) clazz->interface)->endFrame = endFrame;
  ((RendererInterface *) clazz->interface)->init = init;
  ((RendererInterface *) clazz->interface)->renderDeviceDidReset = renderDeviceDidReset;
  ((RendererInterface *) clazz->interface)->renderDeviceWillReset = renderDeviceWillReset;
  ((RendererInterface *) clazz->interface)->setClippingFrame = setClippingFrame;
  ((RendererInterface *) clazz->interface)->setDrawColor = setDrawColor;
}

/**
 * @fn Class *Renderer::_Renderer(void)
 * @memberof Renderer
 */
Class *_Renderer(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Renderer",
      .superclass = _Object(),
      .instanceSize = sizeof(Renderer),
      .interfaceOffset = offsetof(Renderer, interface),
      .interfaceSize = sizeof(RendererInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
