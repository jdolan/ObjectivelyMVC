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

#include <stdlib.h>
#include <unistd.h>

#include <Objectively.h>
#include <ObjectivelyMVC.h>

#include "Hello-Scene.h"
#include "HelloViewController.h"

#ifndef EXAMPLES
# define EXAMPLES "."
#endif

static void onViewEvent(SDL_AudioStream *stream, const SDL_UserEvent *event);

/**
 * @brief Program entry point.
 */
int main(int argc, char *argv[]) {

  MVC_LogSetPriority(SDL_LOG_PRIORITY_DEBUG);

  MVC_Assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO), "SDL_Init");

  SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &(SDL_AudioSpec) {
    .format = SDL_AUDIO_S16LE,
    .channels = 1,
    .freq = 22050,
  }, NULL, NULL);
  MVC_Assert(stream, "SDL_OpenAudioDeviceStream");

  MVC_Assert(SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream)), "SDL_ResumeAudioDevice");

  SDL_Window *window = SDL_CreateWindow("Hello ObjectivelyMVC", 1024, 720, SDL_WINDOW_HIGH_PIXEL_DENSITY);
  MVC_Assert(window, "SDL_CreateWindow");

  $$(Resource, addResourcePath, EXAMPLES);

  WindowController *windowController = $(alloc(WindowController), initWithWindow, window);

  ViewController *viewController = (ViewController *) $(alloc(HelloViewController), init);

  $(windowController, setViewController, viewController);

  Renderer *renderer = windowController->renderer;
  renderer->clear = false;

  initScene(renderer);

  while (true) {
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      $(windowController, respondToEvent, &event);
      
      if (event.type == MVC_VIEW_EVENT) {
        onViewEvent(stream, &event.user);
      }

      if (event.type == SDL_EVENT_QUIT) {
        break;
      }
    }

    if (event.type == SDL_EVENT_QUIT) {
      break;
    }

    $(renderer, beginFrame);

    if (renderer->cmd) {
      drawScene(renderer);

      $(viewController->view, applyThemeIfNeeded, windowController->theme);
      $(viewController->view, layoutIfNeeded);
      $(viewController->view, draw, renderer);

      $(renderer, endFrame);
    }
  }

  release(viewController);
  release(windowController);

  SDL_DestroyWindow(window);

  SDL_DestroyAudioStream(stream);

  SDL_Quit();

  return 0;
}

#include "click.wav.h"
#include "clack.wav.h"

/**
 * @brief `ViewEvent` callback to play click sounds when interacting with `Control`s.
 */
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
