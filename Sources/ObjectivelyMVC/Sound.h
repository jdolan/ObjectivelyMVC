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

#pragma once

#include <SDL_audio.h>

#include <Objectively/Data.h>
#include <Objectively/Resource.h>

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief Sound loading and playback.
 */

/**
 * @brief The sound specification type.
 */
typedef struct SoundSpec {

	/**
	 * @brief The sample format.
	 */
	SDL_AudioFormat format;

	/**
	 * @brief The number of channels.
	 */
	size_t channels;

	/**
	 * @brief The sample rate.
	 */
	size_t rate;
} SoundSpec;

typedef struct Sound Sound;
typedef struct SoundInterface SoundInterface;

/**
 * @brief Sound loading and playback.
 * @extends Object
 */
struct Sound {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	SoundInterface *interface;

	/**
	 * @brief The SoundSpec.
	 */
	SoundSpec spec;

	/**
	 * @brief The PCM (waveform) data.
	 */
	Data *data;
};

/**
 * @brief The Sound interface.
 */
struct SoundInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn Sound *Sound::convert(const Sound *self, const SoundSpec *spec)
	 * @brief Converts this Sound to the given SoundSpec.
	 * @param self The Sound.
	 * @param spec The SoundSpec.
	 * @return A copy of this Sound converted to the given SoundSpec.
	 * @memberof Sound
	 */
	Sound *(*convert)(const Sound *self, const SoundSpec *spec);

	/**
	 * @fn Sound *Sound::initWithData(Sound *self, const SoundSpec *spec, Data *data)
	 * @brief Initializes this Sound with the specified Data.
	 * @param self The Sound.
	 * @param spec The SoundSpec.
	 * @param data The Data providing PCM (waveform) data.
	 * @return The initialized Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*initWithData)(Sound *self, const SoundSpec *spec, Data *data);

	/**
	 * @fn Sound *Sound::initWithResource(Sound *self, const SoundSpec *spec, const Resource *resource)
	 * @brief Initializes this Sound with the specified Resource.
	 * @param self The Sound.
	 * @param spec The SoundSpec.
	 * @param resource The Resource providing PCM (waveform) data.
	 * @return The initialized Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*initWithResource)(Sound *self, const SoundSpec *spec, const Resource *resource);

	/**
	 * @fn Sound *Sound::initWithResourceName(Sound *self, const SoundSpec *spec, const char *name)
	 * @brief Initializes this Sound with the specified Resource name.
	 * @param self The Sound.
	 * @param spec The SoundSpec.
	 * @param resource The name of the Resource providing PCM (waveform) data.
	 * @return The initialized Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*initWithResourceName)(Sound *self, const SoundSpec *spec, const char *name);
};

/**
 * @fn Class *Sound::_Sound(void)
 * @brief The Sound archetype.
 * @return The Sound Class.
 * @memberof Sound
 */
OBJECTIVELYMVC_EXPORT Class *_Sound(void);
