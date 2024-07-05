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

#include <Objectively/Object.h>

#include <ObjectivelyMVC/Sound.h>

/**
 * @file
 * @brief The SoundStage is responsible for sound playback.
 */

typedef struct SoundStage SoundStage;
typedef struct SoundStageInterface SoundStageInterface;

/**
 * @brief The SoundStage is responsible for sound playback.
 * @details This class provides a no-op implementation of the SoundStageInterface. Applications
 * may extend this class and provide an implementation that meets their own requirements.
 * @extends Object
 */
struct SoundStage {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	SoundStageInterface *interface;

	/**
	 * @brief The `SDL_AudioDeviceID`.
	 */
	SDL_AudioDeviceID device;

	/**
	 * @brief The `SDL_AudioSpec`.
	 */
	SDL_AudioSpec spec;
};

/**
 * @brief The SoundStage interface.
 */
struct SoundStageInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn SoundStage *SoundStage::init(SoundStage *self)
	 * @brief Initializes this SoundStage.
	 * @param self The SoundStage.
	 * @param device The device name, or `NULL` for the default.
	 * @param spec The SDL_AudioSpec, or `NULL` for `MVC_AUDIO_DEFAULT`.
	 * @return The initialized SoundStage, or `NULL` on error.
	 * @memberof SoundStage
	 */
	SoundStage *(*initWithSpec)(SoundStage *self, const char *device, const SDL_AudioSpec *spec);

	/**
	 * @fn void SoundStage::play(const SoundStage *self, const Sound *sound)
	 * @brief Plays the given Sound.
	 * @param self The SoundStage.
	 * @param sound The Sound.
	 * @memberof SoundStage
	 */
	void (*play)(const SoundStage *self, const Sound *sound);

	/**
	 * @fn void SoundStage::respondToViewEvent(const SoundStage *self, const SDL_Event *event)
	 * @brief Respond to `MVC_VIEW_EVENT`s, playing sounds if appropriate.
	 * @param self The SoundStage.
	 * @param event The SDL_Event.
	 * @memberof SoundStage
	 */
	void (*respondToViewEvent)(const SoundStage *self, const SDL_Event *event);
};

/**
 * @fn Class *SoundStage::_SoundStage(void)
 * @brief The SoundStage archetype.
 * @return The SoundStage Class.
 * @memberof SoundStage
 */
OBJECTIVELYMVC_EXPORT Class *_SoundStage(void);
