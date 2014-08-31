/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#include <fontconfig/fontconfig.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "MVC_main.h"

int MVC_Init(void) {

	int err = IMG_Init(0);
	if (err == 0) {
		err = TTF_Init();
		if (err == 0) {
			err = (FcInit() != true);
		}
	}
	return err;
}

void MVC_Quit(void) {

	FcFini();
	TTF_Quit();
	IMG_Quit();
}
