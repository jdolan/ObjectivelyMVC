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

#include "Sound.h"

#define _Class _Sound

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Sound *this = (Sound *) self;

	release(this->data);

	super(Object, self, dealloc);
}

#pragma mark - Sound

/**
 * @fn Sound *Sound::initWithData(Sound *, const SoundSpec *, Data *)
 * @memberof Sound
 */
static Sound *initWithData(Sound *self, const SoundSpec *spec, Data *data) {

	self = (Sound *) super(Object, self, init);
	if (self) {

		assert(spec);
		self->spec = *spec;

		assert(self->spec.format);
		assert(self->spec.channels);
		assert(self->spec.rate);

		if (data) {
			self->data = retain(data);
		}
	}

	return self;
}

/**
 * @fn Sound *Sound::initWithResource(Sound *, const SoundSpec *, const Resource *)
 * @memberof Sound
 */
static Sound *initWithResource(Sound *self, const SoundSpec *spec, const Resource *resource) {

	if (resource) {
		self = $(self, initWithData, spec, resource->data);
	} else {
		self = release(self);
	}

	return self;
}

/**
 * @fn Sound *Sound::initWithResourceName(Sound *, const SoundSpec *, const char *)
 * @memberof Sound
 */
static Sound *initWithResourceName(Sound *self, const SoundSpec *spec, const char *name) {

	Resource *resource = $$(Resource, resourceWithName, name);

	self = $(self, initWithResource, spec, resource);

	release(resource);

	return self;
}

/**
 * @fn Sound *Sound::convert(const Sound *, const SoundSpec *)
 * @memberof Sound
 */
static Sound *convert(const Sound *self, const SoundSpec *spec) {

	Sound *sound = $(alloc(Sound), initWithData, spec, NULL);
	assert(sound);

	if (self->data) {
		SDL_AudioStream *stream = SDL_NewAudioStream(self->spec.format,
								  self->spec.channels,
								  (int) self->spec.rate,
								  spec->format,
								  spec->channels,
								  (int) spec->rate);
		assert(stream);

		SDL_AudioStreamPut(stream, self->data->bytes, (int) self->data->length);
		SDL_AudioStreamFlush(stream);

		const int length = SDL_AudioStreamAvailable(stream);

		sound->data = $(alloc(Data), initWithMemory, malloc(length), length);
		assert(sound->data);

		SDL_AudioStreamGet(stream, (void *) sound->data->bytes, length);
		SDL_FreeAudioStream(stream);
	}

	return sound;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((SoundInterface *) clazz->interface)->convert = convert;
	((SoundInterface *) clazz->interface)->initWithData = initWithData;
	((SoundInterface *) clazz->interface)->initWithResource = initWithResource;
	((SoundInterface *) clazz->interface)->initWithResourceName = initWithResourceName;

}

/**
 * @fn Class *Sound::_Sound(void)
 * @memberof Sound
 */
Class *_Sound(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Sound",
			.superclass = _Object(),
			.instanceSize = sizeof(Sound),
			.interfaceOffset = offsetof(Sound, interface),
			.interfaceSize = sizeof(SoundInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
