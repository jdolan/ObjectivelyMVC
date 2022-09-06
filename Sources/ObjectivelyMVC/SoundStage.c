/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
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

#include <SDL2/SDL_mixer.h>

#include "Log.h"
#include "SoundStage.h"
#include "WindowController.h"

#define _Class _SoundStage

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Mix_CloseAudio();

	super(Object, self, dealloc);
}

#pragma mark - SoundStage

/**
 * @fn SoundStage *SoundStage::init(SoundStage *)
 * @memberof SoundStage
 */
static SoundStage *init(SoundStage *self) {

	self = (SoundStage *) super(Object, self, init);
	if (self) {

		if (Mix_OpenAudioDevice(48000, AUDIO_S16SYS, 2, 2048, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE) == 0) {
			MVC_LogInfo("Opened audio device for playback\n");
		} else {
			MVC_LogError("Failed to open audio device: %s\n", SDL_GetError());
			self = release(self);
		}
	}

	return self;
}

/**
 * @fn void SoundStage::play(const SoundStage *, const Sound *)
 * @memberof SoundStage
 */
static void play(const SoundStage *self, const Sound *sound) {
	static unsigned int ch;

	assert(sound);

	Mix_PlayChannel(ch++ % MIX_CHANNELS, sound->chunk, 0);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((SoundStageInterface *) clazz->interface)->init = init;
	((SoundStageInterface *) clazz->interface)->play = play;
}

/**
 * @fn Class *SoundStage::_SoundStage(void)
 * @memberof SoundStage
 */
Class *_SoundStage(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "SoundStage",
			.superclass = _Object(),
			.instanceSize = sizeof(SoundStage),
			.interfaceOffset = offsetof(SoundStage, interface),
			.interfaceSize = sizeof(SoundStageInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
