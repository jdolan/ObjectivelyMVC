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

#include "Log.h"
#include "SoundStage.h"

#define _Class _SoundStage

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	SoundStage *this = (SoundStage *) self;

	SDL_CloseAudioDevice(this->deviceId);

	super(Object, self, dealloc);
}

#pragma mark - SoundStage

/**
 * @fn void SoundStage::clear(const SoundStage *)
 * @memberof SoundStage
 */
static void clear(const SoundStage *self) {
	SDL_ClearQueuedAudio(self->deviceId);
}

/**
 * @fn SoundStage *SoundStage::init(SoundStage *)
 * @memberof SoundStage
 */
static SoundStage *init(SoundStage *self) {
	return $(self, initWithDevice, NULL, NULL);
}

/**
 * @fn SoundStage *SoundStage::initWithDevice(SoundStage *self, const char *, const SoundSpec *)
 * @memberof SoundStage
 */
static SoundStage *initWithDevice(SoundStage *self, const char *device, const SoundSpec *spec) {

	self = (SoundStage *) super(Object, self, init);
	if (self) {

		self->desired = (SDL_AudioSpec) {
			.freq = spec ? (int) spec->rate : 44100,
			.channels = spec ? spec->channels : 2,
			.format = spec ? spec->format : AUDIO_U16SYS
		};

		self->deviceId = SDL_OpenAudioDevice(device, 0, &self->desired, &self->obtained, SDL_AUDIO_ALLOW_ANY_CHANGE);
		if (self->deviceId) {
			MVC_LogInfo("Opened device %s for %d bits @ %dHz with %d channels",
						device,
						SDL_AUDIO_BITSIZE(self->obtained.format),
						self->obtained.freq,
						self->obtained.channels);
		} else {
			MVC_LogError("Failed to open audio device %s: %s\n", device, SDL_GetError());
			self = release(self);
		}
	}

	return self;
}

/**
 * @fn void SoundStage::pause(SoundStage *)
 * @memberof SoundStage
 */
static void pause(SoundStage *self) {

	if (SDL_GetAudioDeviceStatus(self->deviceId) == SDL_AUDIO_PAUSED) {
		SDL_PauseAudioDevice(self->deviceId, 0);
	} else {
		SDL_PauseAudioDevice(self->deviceId, 1);
	}
}

/**
 * @fn void SoundStage::play(const SoundStage *, const Sound *)
 * @memberof SoundStage
 */
static void play(const SoundStage *self, const Sound *sound) {

	assert(sound);
	assert(sound->data);

//	if (!memcmp(&self->spec, &sound->spec, sizeof(self->spec))) {
//		// convert it, or just cry?
//	}

	SDL_QueueAudio(self->deviceId, sound->data->bytes, (int) sound->data->length);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((SoundStageInterface *) clazz->interface)->clear = clear;
	((SoundStageInterface *) clazz->interface)->init = init;
	((SoundStageInterface *) clazz->interface)->initWithDevice = initWithDevice;
	((SoundStageInterface *) clazz->interface)->pause = pause;
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
