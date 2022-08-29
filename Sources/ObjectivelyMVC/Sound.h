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

#include <SDL_mixer.h>

#include <Objectively/Data.h>
#include <Objectively/Resource.h>

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief Sound loading and playback.
 */

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
	 * @brief The backing audio chunk.
	 */
	Mix_Chunk *chunk;
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
	 * @fn Sound *Sound::initWithBytes(Sound *self, const uint8_t *bytes, size_t length)
	 * @brief Initializes this Sound with the specified bytes.
	 * @param self The Sound.
	 * @param bytes The encoded sound data.
	 * @param length The length of `bytes`.
	 * @return The new Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*initWithBytes)(Sound *self, const uint8_t *bytes, size_t length);

	/**
	 * @fn Sound *Sound::initWithChunk(Sound *self, const Chunk *chunk)
	 * @brief Initializes this Sound with the given audio chunk.
	 * @param self The Sound.
	 * @param chunk The backing chunk.
	 * @return The new Sound, or `NULL` on error.
	 * @remarks Designated initializer.
	 * @memberof Sound
	 */
	Sound *(*initWithChunk)(Sound *self, Mix_Chunk *chunk);

	/**
	 * @fn Sound *Sound::initWithData(Sound *self, const Data *data)
	 * @brief Initializes this Sound with the specified Data.
	 * @param self The Sound.
	 * @param data The Data providing encoded audio data.
	 * @return The initialized Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*initWithData)(Sound *self, const Data *data);

	/**
	 * @fn Sound *Sound::initWithResource(Sound *self, const Resource *resource)
	 * @brief Initializes this Sound with the specified Resource.
	 * @param self The Sound.
	 * @param resource The Resource providing encoded audio data.
	 * @return The initialized Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*initWithResource)(Sound *self, const Resource *resource);

	/**
	 * @fn Sound *Sound::initWithResourceName(Sound *self, const char *name)
	 * @brief Initializes this Sound with the specified Resource name.
	 * @param self The Sound.
	 * @param name The name of the Resource providing encoded audio data.
	 * @return The initialized Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*initWithResourceName)(Sound *self, const char *name);

	/**
	 * @static
	 * @fn Sound *Sound::soundWithBytes(const uint8_t *bytes, size_t length)
	 * @brief Instantiates an Sound with the specified bytes.
	 * @param bytes The encoded sound bytes.
	 * @param length The length of `bytes`.
	 * @return The new Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*soundWithBytes)(const uint8_t *bytes, size_t length);

	/**
	 * @static
	 * @fn Sound *Sound::soundWithChunk(Mix_Chunk *chunk)
	 * @brief Instantiates an Sound with the specified chunk.
	 * @param chunk The chunk.
	 * @return The new Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*soundWithChunk)(Mix_Chunk *chunk);
	
	/**
	 * @static
	 * @fn Sound *Sound::soundWithData(const Data *data)
	 * @brief Instantiates an Sound with the specified Data.
	 * @param data The encoded sound Data.
	 * @return The new Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*soundWithData)(const Data *data);

	/**
	 * @static
	 * @fn Sound *Sound::soundWithResource(const Resource *resource)
	 * @brief Instantiates an Sound with the specified Resource.
	 * @param resource The Resource containing encoded sound data.
	 * @return The new Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*soundWithResource)(const Resource *resource);

	/**
	 * @static
	 * @fn Sound *Sound::soundWithResourceName(const char *name)
	 * @brief Instantiates an Sound with the specified Resource name.
	 * @param name The name of a Resource containing encoded sound data.
	 * @return The new Sound, or `NULL` on error.
	 * @memberof Sound
	 */
	Sound *(*soundWithResourceName)(const char *name);
};

/**
 * @fn Class *Sound::_Sound(void)
 * @brief The Sound archetype.
 * @return The Sound Class.
 * @memberof Sound
 */
OBJECTIVELYMVC_EXPORT Class *_Sound(void);
