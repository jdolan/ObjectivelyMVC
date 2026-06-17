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
  SDL_AudioStream *stream;
  WindowController *windowController;
} AppState;

static void onViewEvent(SDL_AudioStream *stream, const SDL_UserEvent *event) {

  if (!instanceof(Control, event->data1)) {
    return;
  }

  switch (event->code) {
    case ViewEventClick:
      SDL_PutAudioStreamData(stream, click_wav, click_wav_len);
      break;
    case ViewEventChange:
      SDL_PutAudioStreamData(stream, clack_wav, clack_wav_len);
      break;
    default:
      break;
  }
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {

  MVC_LogSetPriority(SDL_LOG_PRIORITY_DEBUG);

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  AppState *state = SDL_calloc(1, sizeof(AppState));
  if (!state) {
    return SDL_APP_FAILURE;
  }
  *appstate = state;

  state->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &(SDL_AudioSpec) {
    .format = SDL_AUDIO_S16LE,
    .channels = 1,
    .freq = 22050,
  }, NULL, NULL);

  if (state->stream) {
    SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(state->stream));
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  state->window = SDL_CreateWindow("Hello",
    0, 0,
    SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_HIGH_PIXEL_DENSITY
  );

  if (!state->window) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->context = SDL_GL_CreateContext(state->window);
  if (!state->context) {
    SDL_Log("SDL_GL_CreateContext failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GL_SetSwapInterval(1);

  // On iOS, dlsym cannot find class factories in statically linked libraries.
  // Explicitly touch each factory to register all classes before any CSS/JSON
  // loading calls classForName().
  _Box(); _Button(); _Checkbox(); _CollectionItemView(); _CollectionView();
  _Control(); _DebugViewController(); _Font(); _HSVColorPicker(); _HueColorPicker();
  _Image(); _ImageView(); _Input(); _Label(); _NavigationViewController();
  _Option(); _PageView(); _Panel(); _ProgressBar(); _Renderer();
  _RGBColorPicker(); _ScrollView(); _Select(); _Selector(); _SelectorSequence();
  _SimpleSelector(); _Slider(); _SlideShowView(); _StackView(); _Style();
  _Stylesheet(); _TableCellView(); _TableColumn(); _TableHeaderCellView();
  _TableHeaderView(); _TableRowView(); _TableView(); _TabView();
  _TabViewController(); _TabViewItem(); _Text(); _TextView(); _Theme();
  _View(); _ViewController(); _Warning(); _WindowController();

  // On iOS, SDL_GetBasePath() returns the app bundle's resource directory.
  const char *basePath = SDL_GetBasePath();
  $$(Resource, addResourcePath, basePath ? basePath : ".");

  state->windowController = $(alloc(WindowController), initWithWindow, state->window);

  ViewController *viewController = (ViewController *) $(alloc(HelloViewController), init);
  $(state->windowController, setViewController, viewController);
  release(viewController);

  initScene();

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {

  AppState *state = appstate;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawScene(state->window);

  $(state->windowController, render);

  SDL_GL_SwapWindow(state->window);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {

  AppState *state = appstate;

  $(state->windowController, respondToEvent, event);

  if (event->type == MVC_VIEW_EVENT) {
    onViewEvent(state->stream, &event->user);
  }

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {

  AppState *state = appstate;
  if (state) {
    release(state->windowController);
    if (state->context) {
      SDL_GL_DestroyContext(state->context);
    }
    if (state->window) {
      SDL_DestroyWindow(state->window);
    }
    if (state->stream) {
      SDL_DestroyAudioStream(state->stream);
    }
    SDL_free(state);
  }

  SDL_Quit();
}
