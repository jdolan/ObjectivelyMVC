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
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

#include <Objectively.h>
#include <ObjectivelyMVC.h>

#include "HelloViewController.h"

static void drawScene(void);

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
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
	);
	
	SDL_GL_SetSwapInterval(1);
	
	ViewController *helloViewController = $((ViewController *) alloc(HelloViewController), initRootViewController, window);
	
	SDL_Renderer *renderer = SDL_CreateRenderer(window, 0,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
	);
	
	while (true) {
		SDL_Event event;
		
		memset(&event, 0, sizeof(event));
		
		if (SDL_PollEvent(&event)) {
			
			$(helloViewController, respondToEvent, &event);
			
			if (event.type == SDL_QUIT) {
				break;
			}
		}
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		
		drawScene();
		
		$(helloViewController, drawView, renderer);
		
		SDL_RenderPresent(renderer);
	}
	
	release(helloViewController);
	
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
	
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(-2.0, 2.0, -2.0, 2.0, -3.0, 3.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	glPushMatrix();
	
	const long time = getCurrentTime();
	glRotatef(time / M_PI * 0.3333, 0.0, 1.0, 0.0);
	
	GLfloat color[3] = { 0.0, 0.0, 0.0 };
	color[(time / 1000) % 3] = 1.0;
	
	glColor3fv(color);
	glutWireTeapot(1.0);
	glColor3b(255, 255, 255);
	
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(0, 1024, 768, 0, -1, 1);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
