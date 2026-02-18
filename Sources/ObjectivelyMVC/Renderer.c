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

#include "Log.h"
#include "Renderer.h"
#include "View.h"
#include "Window.h"

#define _Class _Renderer

#pragma mark - Renderer

/**
 * @fn void Renderer::beginFrame(Renderer *self)
 * @memberof Renderer
 */
static void beginFrame(Renderer *self) {

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_SCISSOR_TEST);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  $(self, setDrawColor, &Colors.White);
}

/**
 * @fn GLuint Renderer::createTexture(const Renderer *self, const SDL_Surface *surface)
 * @memberof Renderer
 */
static GLuint createTexture(const Renderer *self, const SDL_Surface *surface) {

  assert(surface);

  GLenum format;
  switch (SDL_BYTESPERPIXEL(surface->format)) {
    case 1:
      format = GL_LUMINANCE;
      break;
    case 3:
      format = GL_RGB;
      break;
    case 4:
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
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

  glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

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

  glVertexPointer(2, GL_INT, 0, points);

  glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) count);
}

/**
 * @fn void Renderer::drawRect(const Renderer *self, const SDL_Rect *rect)
 * @memberof Renderer
 */
static void drawRect(const Renderer *self, const SDL_Rect *rect) {

  assert(rect);

  GLint verts[8];

  verts[0] = rect->x;
  verts[1] = rect->y;

  verts[2] = rect->x + rect->w;
  verts[3] = rect->y;

  verts[4] = rect->x + rect->w;
  verts[5] = rect->y + rect->h;

  verts[6] = rect->x;
  verts[7] = rect->y + rect->h;

  glVertexPointer(2, GL_INT, 0, verts);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
}

/**
 * @fn void Renderer::drawRectFilled(const Renderer *self, const SDL_Rect *rect)
 * @memberof Renderer
 */
static void drawRectFilled(const Renderer *self, const SDL_Rect *rect) {

  assert(rect);

  glRecti(rect->x - 1, rect->y - 1, rect->x + rect->w + 1, rect->y + rect->h + 1);
}

/**
 * @fn void Renderer::drawTexture(const Renderer *self, GLuint texture, const SDL_Rect *dest)
 * @memberof Renderer
 */
static void drawTexture(const Renderer *self, GLuint texture, const SDL_Rect *rect) {

  assert(rect);

  const GLfloat texcoords[] = {
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0
  };

  GLint verts[8];

  verts[0] = rect->x;
  verts[1] = rect->y;

  verts[2] = rect->x + rect->w;
  verts[3] = rect->y;

  verts[4] = rect->x + rect->w;
  verts[5] = rect->y + rect->h;

  verts[6] = rect->x;
  verts[7] = rect->y + rect->h;

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  glVertexPointer(2, GL_INT, 0, verts);
  glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

  glDrawArrays(GL_QUADS, 0, 4);

  glDisable(GL_TEXTURE_2D);
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

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  $(self, setClippingFrame, NULL);

  glDisable(GL_SCISSOR_TEST);

  glBlendFunc(GL_ONE, GL_ZERO);
  glDisable(GL_BLEND);

  const GLenum err = glGetError();
  if (err) {
    MVC_LogError("GL error: %d\n", err);
  }
}

/**
 * @fn Renderer *Renderer::init(Renderer *self)
 * @memberof Renderer
 */
static Renderer *init(Renderer *self) {
  return (Renderer *) super(Object, self, init);
}

/**
 * @fn void Renderer::renderDeviceDidReset(Renderer *self)
 * @memberof Renderer
 */
static void renderDeviceDidReset(Renderer *self) {

}

/**
 * @fn void Renderer::renderDeviceWillReset(Renderer *self)
 * @memberof Renderer
 */
static void renderDeviceWillReset(Renderer *self) {

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
}

/**
 * @fn void Renderer::setDrawColor(Renderer *self, const SDL_Color *color)
 * @memberof Renderer
 */
static void setDrawColor(Renderer *self, const SDL_Color *color) {
  glColor4ubv((const GLubyte *) color);
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

