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

#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <Objectively.h>
#include <ObjectivelyMVC.h>

#include "Hello-Scene.h"
#include "HelloViewController.h"

#include "click.wav.h"
#include "clack.wav.h"

typedef struct {
  SDL_Window *window;
  SDL_GLContext context;
  SDL_AudioStream *audioStream;
  WindowController *windowController;
} Application;

static Application app;

static void onViewEvent(const SDL_UserEvent *event) {

  if (!instanceof(Control, event->data1)) {
    return;
  }

  switch (event->code) {
    case ViewEventClick:
      SDL_PutAudioStreamData(app.audioStream, click_wav, click_wav_len);
      break;
    case ViewEventChange:
      SDL_PutAudioStreamData(app.audioStream, clack_wav, clack_wav_len);
      break;
    default:
      break;
  }
}

SDL_AppResult SDL_AppInit(void **unused, int argc, char *argv[]) {

  MVC_LogSetPriority(SDL_LOG_PRIORITY_DEBUG);

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  memset(&app, 0, sizeof(app));

  app.audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &(SDL_AudioSpec) {
    .format = SDL_AUDIO_S16LE,
    .channels = 1,
    .freq = 22050,
  }, NULL, NULL);

  if (app.audioStream) {
    SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(app.audioStream));
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  app.window = SDL_CreateWindow("Hello",
    0, 0,
    SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_HIGH_PIXEL_DENSITY
  );

  if (!app.window) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  app.context = SDL_GL_CreateContext(app.window);
  if (!app.context) {
    SDL_Log("SDL_GL_CreateContext failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GL_SetSwapInterval(1);

  // On iOS, SDL_GetBasePath() returns the app bundle's resource directory.
  const char *basePath = SDL_GetBasePath();
  $$(Resource, addResourcePath, basePath ? basePath : ".");

  app.windowController = $(alloc(WindowController), initWithWindow, app.window);

  ViewController *viewController = (ViewController *) $(alloc(HelloViewController), init);
  $(app.windowController, setViewController, viewController);
  release(viewController);

  initScene();

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *unused) {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawScene(app.window);

  $(app.windowController, render);

  SDL_GL_SwapWindow(app.window);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *unused, SDL_Event *event) {

  $(app.windowController, respondToEvent, event);

  if (event->type == MVC_VIEW_EVENT) {
    onViewEvent(&event->user);
  }

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *unused, SDL_AppResult result) {

  release(app.windowController);

  if (app.audioStream) {
    SDL_DestroyAudioStream(app.audioStream);
  }

  if (app.context) {
    SDL_GL_DestroyContext(app.context);
  }

  if (app.window) {
    SDL_DestroyWindow(app.window);
  }

  SDL_Quit();
}
