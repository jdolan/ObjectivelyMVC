/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
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

#include <SDL3/SDL_gpu.h>

#include <ObjectivelyMVC/Types.h>

/**
 * @file
 * @brief SDL_gpu convenience layer for Renderer and embedding applications.
 * @details Provides the module-level `mvc_gpu_device` pointer (populated by
 * Renderer on `renderDeviceDidReset`) and helpers for texture lifetime management.
 * Any code sharing the same GPU context may use these directly.
 */

/**
 * @brief The module-level SDL_GPUDevice, populated by Renderer on init/reset.
 * @details Views and other objects that need to release GPU textures outside of
 * a Renderer callback may call MVC_ReleaseGPUTexture() which uses this device.
 */
OBJECTIVELYMVC_EXPORT SDL_GPUDevice *mvc_gpu_device;

/**
 * @brief The window currently being rendered.
 * @details Set by Renderer::beginFrame; used by Font and Text to query pixel
 * density without requiring a Renderer reference.
 */
OBJECTIVELYMVC_EXPORT SDL_Window *mvc_current_window;

/**
 * @brief Releases a GPU texture using the module-level device.
 * @param texture The texture to release, or NULL (no-op).
 * @remark Safe to call outside of a Renderer callback; uses mvc_gpu_device.
 */
OBJECTIVELYMVC_EXPORT void MVC_ReleaseGPUTexture(SDL_GPUTexture *texture);

/**
 * @brief MSL source for the vertex shader.
 * @details Expects:
 *   - Vertex attribute 0: float2 position
 *   - Vertex attribute 1: float2 texcoord
 *   - Vertex uniform buffer 0: float4x4 projection
 */
OBJECTIVELYMVC_EXPORT const char *MVC_VertexShaderMSL;

/**
 * @brief MSL source for the fragment shader.
 * @details Expects:
 *   - Fragment texture 0: sampler2D
 *   - Fragment sampler 0: sampler
 *   - Fragment uniform buffer 0: float4 color
 */
OBJECTIVELYMVC_EXPORT const char *MVC_FragmentShaderMSL;
