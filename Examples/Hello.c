/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and GNU C.
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

#ifdef SDL_PLATFORM_IOS
# define HELLO_WINDOW_W      0
# define HELLO_WINDOW_H      0
# define HELLO_WINDOW_FLAGS  (SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_FULLSCREEN)
#else
# define HELLO_WINDOW_W      1024
# define HELLO_WINDOW_H      720
# define HELLO_WINDOW_FLAGS  SDL_WINDOW_HIGH_PIXEL_DENSITY
#endif

/**
 * @brief SDL application state passed via pointer to callbacks.
 */
typedef struct {
	SDL_Window *window;
	SDL_AudioStream *audioStream;
	WindowController *windowController;
} AppState;

static AppState app;

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

/**
 * @brief SDL3 application initialization callback.
 */
SDL_AppResult SDL_AppInit(void **unused, int argc, char *argv[]) {

	MVC_LogSetPriority(SDL_LOG_PRIORITY_DEBUG);

	MVC_Assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO), "SDL_Init");

#ifdef EXAMPLES
	$$(Resource, addResourcePath, EXAMPLES);
#endif

	const char *basePath = SDL_GetBasePath();
	if (basePath) {
		$$(Resource, addResourcePath, basePath);
	}

	app.audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &(SDL_AudioSpec) {
		.format = SDL_AUDIO_S16LE,
		.channels = 1,
		.freq = 22050,
	}, NULL, NULL);

	if (app.audioStream) {
		SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(app.audioStream));
	}

	app.window = SDL_CreateWindow("Hello ObjectivelyMVC", HELLO_WINDOW_W, HELLO_WINDOW_H, HELLO_WINDOW_FLAGS);
	MVC_Assert(app.window, "SDL_CreateWindow");

	app.windowController = $(alloc(WindowController), initWithWindow, app.window);

	ViewController *viewController = (ViewController *) $(alloc(HelloViewController), init);
	$(app.windowController, setViewController, viewController);
	release(viewController);

	Renderer *renderer = app.windowController->renderer;
	renderer->clear = false;

	initScene(renderer);

	return SDL_APP_CONTINUE;
}

/**
 * @brief SDL3 frame iteration callback.
 */
SDL_AppResult SDL_AppIterate(void *unused) {

	WindowController *wc = app.windowController;
	Renderer *renderer = wc->renderer;
	ViewController *vc = wc->viewController;

	$(renderer, beginFrame);

	if (renderer->cmd) {
		drawScene(renderer);

		$(vc->view, applyThemeIfNeeded, wc->theme);
		$(vc->view, layoutIfNeeded);
		$(vc->view, draw, renderer);

		$(renderer, endFrame);
	}

	return SDL_APP_CONTINUE;
}

/**
 * @brief SDL3 event callback.
 */
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

/**
 * @brief SDL3 quit callback.
 */
void SDL_AppQuit(void *unused, SDL_AppResult result) {

	release(app.windowController);

	if (app.audioStream) {
		SDL_DestroyAudioStream(app.audioStream);
	}

	if (app.window) {
		SDL_DestroyWindow(app.window);
	}

	SDL_Quit();
}
