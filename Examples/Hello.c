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

#include <assert.h>
#include <unistd.h>

#include <Objectively.h>
#include <ObjectivelyMVC.h>

#include "HelloViewController.h"

/**
 * @brief Program entry point.
 */
int main(int arg, char **argv) {
	
	LogSetPriority(SDL_LOG_PRIORITY_VERBOSE);
	
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_Window *window = SDL_CreateWindow(__FILE__,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1024,
		768,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	
	ViewController *rootViewController = $((ViewController *) alloc(HelloViewController), initRootViewController, window);
	
	SDL_Renderer *renderer = SDL_CreateRenderer(window, 0,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
	);
	
	while (true) {
		SDL_Event event;
		
		memset(&event, 0, sizeof(event));
		
		if (SDL_PollEvent(&event)) {
			
			$(rootViewController, respondToEvent, &event);
			
			if (event.type == SDL_QUIT) {
				break;
			}
		}
		
		SDL_RenderClear(renderer);
		
		$(rootViewController, drawView, renderer);
		
		SDL_RenderPresent(renderer);
		
		usleep(16000);
	}
	
	release(rootViewController);
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	SDL_Quit();
	
	return 0;
}

