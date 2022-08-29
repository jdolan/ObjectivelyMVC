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

#include <ObjectivelyMVC/Sound.h>

/**
 * @file
 * @brief Sound playback.
 */

typedef struct SoundStage SoundStage;
typedef struct SoundStageInterface SoundStageInterface;

/**
 * @brief The SoundStage type.
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
	 * @brief Initializes this SoundStage with the default device and configuration.
	 * @param self The SoundStage.
	 * @return The initialized SoundStage, or `NULL` on error.
	 * @memberof SoundStage
	 */
	SoundStage *(*init)(SoundStage *self);

	/**
	 * @fn void SoundStage::play(const SoundStage *self, const char *sound)
	 * @brief Enqueues the given Sound for playback.
	 * @param self The SoundStage.
	 * @param sound The Sound.
	 * @memberof SoundStage
	 */
	void (*play)(const SoundStage *self, const Sound *sound);
};

/**
 * @fn Class *SoundStage::_SoundStage(void)
 * @brief The SoundStage archetype.
 * @return The SoundStage Class.
 * @memberof SoundStage
 */
OBJECTIVELYMVC_EXPORT Class *_SoundStage(void);

/**
 * @brief Plays the specified Sound through the current SoundStage (if any).
 * @param sound The Sound to play.
 * @remarks This is a convenience function, allowing Views to emit sounds in an ad-hoc way,
 * without obtaining a reference to the SoundStage.
 */
OBJECTIVELYMVC_EXPORT void MVC_PlaySound(const Sound *sound);

OBJECTIVELYMVC_EXPORT Sound *_click;
OBJECTIVELYMVC_EXPORT Sound *_clack;
