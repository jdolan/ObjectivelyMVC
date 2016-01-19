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

static void buttonAction(ident sender, const SDL_Event *event, ident data) {
	printf("hi\n");
}

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
	assert(window);
	
	SDL_GLContext *context = SDL_GL_CreateContext(window);
	assert(context);
	
	ViewController *rootViewController = $(alloc(ViewController), initRootViewController, window);
	assert(rootViewController);
	
	SDL_Renderer *renderer = SDL_CreateRenderer(window, 0,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
	);
	assert(renderer);
	
	const SDL_Rect frame = { .x = 100, .y = 100 };
	
	Font *font = $(alloc(Font), initWithName, "Lucida Sans Unicode-24");
	assert(font);
	
	Label *label = $(alloc(Label), initWithText, "Hello World!", font);
	assert(label);
		
	Button *button = $(alloc(Button), initWithFrame, &frame);
	assert(button);
	
	$(button->label, setText, "Click Me");
	$(button->label, setFont, font);
	
	$((Control *) button, addActionForEventType, SDL_MOUSEBUTTONUP, buttonAction, NULL);
	
	View *container = $(alloc(View), initWithFrame, &frame);
	assert(container);
	
	$(container, addSubview, (View *) label);
	$(container, addSubview, (View *) button);
	$(rootViewController->view, addSubview, container);
	
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
		
		usleep(1000);
	}
	
	release(font);
	release(label);
	release(container);
	release(rootViewController);
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	SDL_Quit();
	
	return 0;
}

