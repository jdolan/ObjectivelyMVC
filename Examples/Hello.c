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

#include <Objectively.h>
#include <ObjectivelyMVC.h>

#include "HelloViewController.h"

#ifndef EXAMPLES
# define EXAMPLES "."
#endif

static void drawScene(SDL_Window *window);

/**
 * @brief Program entry point.
 */
int main(int argc, char *argv[]) {

	MVC_LogSetPriority(SDL_LOG_PRIORITY_DEBUG);

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow(__FILE__,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1024,
		768,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
	);

	SDL_GLContext *context = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(1);

	$$(Resource, addResourcePath, EXAMPLES);

	WindowController *windowController = $(alloc(WindowController), initWithWindow, window);

	ViewController *viewController = (ViewController *) $(alloc(HelloViewController), init);

	$(windowController, setViewController, viewController);

	while (true) {
		
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			$(windowController, respondToEvent, &event);
		}

		if (event.type == SDL_QUIT) {
			break;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawScene(window);

		$(windowController, render);

		SDL_GL_SwapWindow(window);
	}

	release(viewController);
	release(windowController);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}

/**
 * @brief Renders a rotating cube, demonstrating ObjectivelyMVC integrating with a vanilla SDL2 /
 * OpenGL application.
 */
static void drawScene(SDL_Window *window) {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-2.0, 2.0, -2.0, 2.0, -3.0, 3.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0, 0.0, -1.0);

	glPushMatrix();

	glRotatef(SDL_GetTicks() * 0.1, 1.0, 1.0, 1.0);

	glEnable(GL_DEPTH_TEST);

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

	glDisable(GL_DEPTH_TEST);

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	glOrtho(0, w, h, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
