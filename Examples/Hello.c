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

#if defined(__APPLE__)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <assert.h>

#include <Objectively.h>
#include <ObjectivelyMVC.h>

#include "HelloViewController.h"

static void drawScene(void);

/**
 * @brief Program entry point.
 */
int main(int argc, char **argv) {

	LogSetPriority(SDL_LOG_PRIORITY_VERBOSE);
	
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_Window *window = SDL_CreateWindow(__FILE__,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1024,
		768,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
	);
	
	SDL_GL_SetSwapInterval(1);

	WindowController *windowController = $(alloc(WindowController), initWithWindow, window);
	
	ViewController *viewController = $((ViewController *) alloc(HelloViewController), init);

	$(windowController, setViewController, viewController);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, 0,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
	);

	while (true) {
		SDL_Event event;
		
		memset(&event, 0, sizeof(event));
		
		if (SDL_PollEvent(&event)) {
			
			$(windowController, respondToEvent, &event);
			
			if (event.type == SDL_QUIT) {
				break;
			}
		}

		SetRenderDrawColor(renderer, Colors.Black);
		
		SDL_RenderClear(renderer);

		drawScene();

		SetRenderDrawColor(renderer, Colors.White);

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		$(windowController, render, renderer);

		SDL_RenderPresent(renderer);
	}
	
	release(viewController);
	release(windowController);
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	SDL_Quit();
	
	return 0;
}

/**
 * @return The simulation time, in milliseconds.
 */
static long getCurrentTime(void) {
	static struct timeval start;
	
	if (start.tv_sec == 0) {
		gettimeofday(&start, NULL);
	}
	
	struct timeval now;
	struct timeval delta;

	gettimeofday(&now, NULL);
	
	delta.tv_sec = now.tv_sec - start.tv_sec;
	delta.tv_usec = now.tv_usec - start.tv_usec;
	
	if (delta.tv_usec < 0) {
		delta.tv_sec--;
		delta.tv_usec += MSEC_PER_SEC;
	}
	
	return delta.tv_sec * 1000.0 + delta.tv_usec / 1000.0;
}

/**
 * @brief Renders a teapot, demonstrating ObjectivelyMVC integrating with a
 * vanilla SDL2 / OpenGL application.
 */
static void drawScene(void) {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(-2.0, 2.0, -2.0, 2.0, -3.0, 3.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glPushMatrix();
	
	const float angle = getCurrentTime() / M_PI * 0.333;
	glRotatef(angle, 0.0, 1.0, 0.0);

	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glBegin(GL_QUADS);

	// Top face (y = 1.0)
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f( 1.0, 1.0, -1.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glVertex3f(-1.0, 1.0,  1.0);
	glVertex3f( 1.0, 1.0,  1.0);

	// Bottom face (y = -1.0)
	glColor3f(1.0, 0.5f, 0.0);
	glVertex3f( 1.0, -1.0,  1.0);
	glVertex3f(-1.0, -1.0,  1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f( 1.0, -1.0, -1.0);

	// Front face  (z = 1.0)
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f( 1.0,  1.0, 1.0);
	glVertex3f(-1.0,  1.0, 1.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f( 1.0, -1.0, 1.0);

	// Back face (z = -1.0)
	glColor3f(1.0, 1.0, 0.0);
	glVertex3f( 1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(-1.0,  1.0, -1.0);
	glVertex3f( 1.0,  1.0, -1.0);

	// Left face (x = -1.0)
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(-1.0,  1.0,  1.0);
	glVertex3f(-1.0,  1.0, -1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0,  1.0);

	// Right face (x = 1.0)
	glColor3f(1.0, 0.0, 1.0);
	glVertex3f(1.0,  1.0, -1.0);
	glVertex3f(1.0,  1.0,  1.0);
	glVertex3f(1.0, -1.0,  1.0);
	glVertex3f(1.0, -1.0, -1.0);

	glEnd();  // End of drawing color-cube

	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, 1024, 768, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
