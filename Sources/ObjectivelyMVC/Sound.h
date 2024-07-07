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

#pragma once

#include <SDL_audio.h>

#include <Objectively/Resource.h>

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief Sound loading.
 */

extern SDL_AudioSpec MVC_AUDIO_DEFAULT;

typedef struct Sound Sound;
typedef struct SoundInterface SoundInterface;

/**
 * @brief Sound loading.
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
	 * @brief The audio specification.
	 */
	SDL_AudioSpec spec;

	/**
	 * @brief The backing data.
	 */
	ident data;
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
	 * @static
	 * @fn Sound *Sound::SoundWithBytes(const uint8_t *bytes, size_t length)
	 * @brief Instantiates a Sound with the given bytes.
	 * @param bytes The WAV bytes.
	 * @param length The length of `bytes`.
	 * @return The new Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*soundWithBytes)(const uint8_t *bytes, size_t length);

	/**
	 * @static
	 * @fn Sound *Sound::SoundWithData(const Data *data)
	 * @brief Instantiates a Sound with the given Data.
	 * @param data The WAV data.
	 * @return The new Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*soundWithData)(Data *data);

	/**
	 * @static
	 * @fn Sound *Sound::SoundWithOpaqueData(ident data)
	 * @brief Initializes this Sound with opaque data.
	 * @details The data will not be decoded in any way. You must provide a custom
	 * SoundStage implementation to play Sounds initialized with this method.
	 * @param data The opaque data.
	 * @return The new Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*soundWithOpaqueData)(ident data);

	/**
	 * @static
	 * @fn Sound *Sound::SoundWithResource(const Resource *resource)
	 * @brief Instantiates a Sound with the given Resource.
	 * @param resource The Resource containing WAV data.
	 * @return The new Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*soundWithResource)(const Resource *resource);

	/**
	 * @static
	 * @fn Sound *Sound::soundWithResourceName(const char *name)
	 * @brief Instantiates a Sound with the given Resource name.
	 * @param name The name of a Resource containing WAV data.
	 * @return The new Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*soundWithResourceName)(const char *name);

	/**
	 * @fn Sound *Sound::initWithBytes(Sound *self, const uint8_t *bytes, size_t length)
	 * @brief Initializes this Sound with the given bytes.
	 * @param self The Sound.
	 * @param bytes The WAV bytes.
	 * @param length The length of `bytes`.
	 * @return The initialized Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*initWithBytes)(Sound *self, const uint8_t *bytes, size_t length);

	/**
	 * @fn Sound *Sound::initWithData(Sound *self, const Data *data)
	 * @brief Initializes this Sound with the given specification and Data.
	 * @param self The Sound.
	 * @param data The WAV Data.
	 * @return The initialized Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*initWithData)(Sound *self, const Data *data);

	/**
	 * @fn Sound *Sound::initWithOpaqueData(Sound *self, ident data)
	 * @brief Initializes this Sound with opaque data.
	 * @details The data will not be decoded in any way. You must provide a custom
	 * SoundStage implementation to play Sounds initialized with this method.
	 * @param self The Sound.
	 * @param data The application-specific data name.
	 * @return The initialized Sound, or `NULL` on error.
	 * @memberof Sound
	 * @see SoundStage::load(const SoundStage *, const char *)
	 */
	Sound *(*initWithOpaqueData)(Sound *self, ident data);

	/**
	 * @fn Sound *Sound::initWithResource(Sound *self, const Resource *resource)
	 * @brief Initializes this Sound with the specified Resource.
	 * @param self The Sound.
	 * @param resource The Resource containing WAV data.
	 * @return The initialized Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*initWithResource)(Sound *self, const Resource *resource);

	/**
	 * @fn Sound *Sound::initWithResourceName(Sound *self, const char *name)
	 * @brief Initializes this Sound, loading the Resource by the given name.
	 * @param self The Sound.
	 * @param name The Resource name.
	 * @return The initialized Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*initWithResourceName)(Sound *self, const char *name);
};

/**
 * @fn Class *Sound::_Sound(void)
 * @brief The Sound archetype.
 * @return The Sound Class.
 * @memberof Sound
 */
OBJECTIVELYMVC_EXPORT Class *_Sound(void);
