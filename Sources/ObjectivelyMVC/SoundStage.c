/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 */

#include <assert.h>
#include <SDL.h>

#include "Log.h"
#include "SoundStage.h"
#include "View.h"

#include "click.wav.h"
#include "clack.wav.h"
#include "select.wav.h"

#define _Class _SoundStage

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	SoundStage *this = (SoundStage *) self;

	if (this->device) {
		SDL_CloseAudioDevice(this->device);
	}

	super(Object, self, dealloc);
}

#pragma mark - SoundStage

/**
 * @fn SoundStage *SoundStage::init(SoundStage *self)
 * @memberof SoundStage
 */
static SoundStage *initWithSpec(SoundStage *self, const char *device, const SDL_AudioSpec *spec) {

	self = (SoundStage *) super(Object, self, init);
	if (self) {

		self->device = SDL_OpenAudioDevice(device, 0, spec ?: &MVC_AUDIO_DEFAULT, &self->spec, SDL_AUDIO_ALLOW_ANY_CHANGE);
		if (self->device) {
			MVC_LogInfo("Opened audio device %d @ %dHz", self->device, self->spec.freq);
			SDL_PauseAudioDevice(self->device, 0);
		} else {
			MVC_LogError("Failed to open audio device: %s\n", SDL_GetError());
			self = release(self);
		}
	}

	return self;
}

/**
 * @fn void SoundStage::play(const SoundStage *self, const Sound *sound)
 * @memberof SoundStage
 */
static void play(const SoundStage *self, const Sound *sound) {
	
	assert(sound);

	const int err = SDL_QueueAudio(self->device, sound->data->bytes, (int) sound->data->length);
	if (err) {
		MVC_LogError("Failed to enqueue audio: %s\n", SDL_GetError());
	}
}

/**
 * @fn void SoundStage::respondToViewEvent(const SoundStage *self, const SDL_Event *event)
 * @memberof SoundStage
 */
static void respondToViewEvent(const SoundStage *self, const SDL_Event *event) {

	const View *view = event->user.data1;

	const char *attr = NULL;
	switch (event->user.code) {
		case ViewEventBlur:
			attr = "sound-on-blur";
			break;
		case ViewEventChange:
			attr = "sound-on-change";
			break;
		case ViewEventClick:
			attr = "sound-on-click";
			break;
		case ViewEventFocus:
			attr = "sound-on-focus";
			break;
		case ViewEventMouseButtonDown:
			attr = "sound-on-mouse-button-down";
			break;
		case ViewEventMouseButtonUp:
			attr = "sound-on-mouse-button-up";
			break;
		default:
			break;
	}

	if (attr) {
		const String *name =  $(view->computedStyle->attributes, objectForKeyPath, attr);
		if (name) {
			Sound *sound = NULL;
			if (!strcmp("click.wav.h", name->chars)) {
				sound = $$(Sound, soundWithBytes, click_wav, click_wav_len);
			} else if (!strcmp("clack.wav.h", name->chars)) {
				sound = $$(Sound, soundWithBytes, clack_wav, clack_wav_len);
			} else if (!strcmp("select.wav.h", name->chars)) {
				sound = $$(Sound, soundWithBytes, select_wav, select_wav_len);
			} else {
				sound = $$(Sound, soundWithResourceName, name->chars);
			}

			if (sound) {
				$(self, play, sound);
			}

			release(sound);
		}
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((SoundStageInterface *) clazz->interface)->initWithSpec = initWithSpec;
	((SoundStageInterface *) clazz->interface)->play = play;
	((SoundStageInterface *) clazz->interface)->respondToViewEvent = respondToViewEvent;
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
