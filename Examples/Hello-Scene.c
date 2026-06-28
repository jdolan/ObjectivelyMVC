/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and GNU C.
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

/**
 * @file
 * @brief Rotating cube background scene for the ObjectivelyMVC Hello example.
 *
 * Renders a spinning colored cube using SDL_gpu directly into the Renderer's
 * command buffer, before MVC composites the UI on top.
 */

#include <ObjectivelyGPU.h>

#include "Hello-Scene.h"

typedef struct {
	vec3 position;
	vec3 color;
} SceneVertex;

static const SceneVertex vertex_data[] = {
	{ { -0.5f,  0.5f, -0.5f }, { 1, 0, 0 } }, { {  0.5f, -0.5f, -0.5f }, { 0, 0, 1 } }, { { -0.5f, -0.5f, -0.5f }, { 0, 1, 0 } },
	{ { -0.5f,  0.5f, -0.5f }, { 1, 0, 0 } }, { {  0.5f,  0.5f, -0.5f }, { 1, 1, 0 } }, { {  0.5f, -0.5f, -0.5f }, { 0, 0, 1 } },
	{ { -0.5f,  0.5f,  0.5f }, { 1, 1, 1 } }, { { -0.5f, -0.5f, -0.5f }, { 0, 1, 0 } }, { { -0.5f, -0.5f,  0.5f }, { 0, 1, 1 } },
	{ { -0.5f,  0.5f,  0.5f }, { 1, 1, 1 } }, { { -0.5f,  0.5f, -0.5f }, { 1, 0, 0 } }, { { -0.5f, -0.5f, -0.5f }, { 0, 1, 0 } },
	{ { -0.5f,  0.5f,  0.5f }, { 1, 1, 1 } }, { {  0.5f,  0.5f, -0.5f }, { 1, 1, 0 } }, { { -0.5f,  0.5f, -0.5f }, { 1, 0, 0 } },
	{ { -0.5f,  0.5f,  0.5f }, { 1, 1, 1 } }, { {  0.5f,  0.5f,  0.5f }, { 0, 0, 0 } }, { {  0.5f,  0.5f, -0.5f }, { 1, 1, 0 } },
	{ {  0.5f,  0.5f, -0.5f }, { 1, 1, 0 } }, { {  0.5f, -0.5f,  0.5f }, { 1, 0, 1 } }, { {  0.5f, -0.5f, -0.5f }, { 0, 0, 1 } },
	{ {  0.5f,  0.5f, -0.5f }, { 1, 1, 0 } }, { {  0.5f,  0.5f,  0.5f }, { 0, 0, 0 } }, { {  0.5f, -0.5f,  0.5f }, { 1, 0, 1 } },
	{ {  0.5f,  0.5f,  0.5f }, { 0, 0, 0 } }, { { -0.5f, -0.5f,  0.5f }, { 0, 1, 1 } }, { {  0.5f, -0.5f,  0.5f }, { 1, 0, 1 } },
	{ {  0.5f,  0.5f,  0.5f }, { 0, 0, 0 } }, { { -0.5f,  0.5f,  0.5f }, { 1, 1, 1 } }, { { -0.5f, -0.5f,  0.5f }, { 0, 1, 1 } },
	{ { -0.5f, -0.5f, -0.5f }, { 0, 1, 0 } }, { {  0.5f, -0.5f,  0.5f }, { 1, 0, 1 } }, { { -0.5f, -0.5f,  0.5f }, { 0, 1, 1 } },
	{ { -0.5f, -0.5f, -0.5f }, { 0, 1, 0 } }, { {  0.5f, -0.5f, -0.5f }, { 0, 0, 1 } }, { {  0.5f, -0.5f,  0.5f }, { 1, 0, 1 } },
};

static SDL_GPUBuffer *scene_vertexBuffer;
static SDL_GPUGraphicsPipeline *scene_pipeline;
static Framebuffer *scene_framebuffer;
static float scene_angleX;
static float scene_angleY;
static Uint64 scene_lastTicks;

void initScene(Renderer *renderer) {

	RenderDevice *device = renderer->device;

	scene_vertexBuffer = $(device, createBufferWithConstMem,
		SDL_GPU_BUFFERUSAGE_VERTEX, vertex_data, sizeof(vertex_data));

	SDL_GPUShader *vs = $(device, loadShader, "Hello-Scene.vert", &(SDL_GPUShaderCreateInfo) {
		.stage = SDL_GPU_SHADERSTAGE_VERTEX,
		.num_uniform_buffers = 1,
	});

	SDL_GPUShader *fs = $(device, loadShader, "Hello-Scene.frag", &(SDL_GPUShaderCreateInfo) {
		.stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
	});

	const SDL_GPUVertexBufferDescription vbd = {
		.slot = 0,
		.pitch = sizeof(SceneVertex),
		.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
	};

	const SDL_GPUVertexAttribute attrs[] = {
		{
			.location = 0,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
			.offset = offsetof(SceneVertex, position),
		},
		{
			.location = 1,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
			.offset = offsetof(SceneVertex, color),
		},
	};

	const SDL_GPUColorTargetDescription ctd = {
		.format = $(device, getSwapchainTextureFormat, device->window),
	};

	scene_pipeline = $(device, createGraphicsPipeline, &(SDL_GPUGraphicsPipelineCreateInfo) {
		.vertex_shader = vs,
		.fragment_shader = fs,
		.vertex_input_state = {
			.vertex_buffer_descriptions = &vbd,
			.num_vertex_buffers = 1,
			.vertex_attributes = attrs,
			.num_vertex_attributes = (Uint32) SDL_arraysize(attrs),
		},
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.rasterizer_state = {
			.fill_mode = SDL_GPU_FILLMODE_FILL,
			.cull_mode = SDL_GPU_CULLMODE_BACK,
			.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
			.enable_depth_clip = true,
		},
		.depth_stencil_state = {
			.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL,
			.enable_depth_test = true,
			.enable_depth_write = true,
		},
		.target_info = {
			.color_target_descriptions = &ctd,
			.num_color_targets = 1,
			.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
			.has_depth_stencil_target = true,
		},
	});

	$(device, releaseShader, vs);
	$(device, releaseShader, fs);

	int w = 0, h = 0;
	SDL_GetWindowSizeInPixels(device->window, &w, &h);

	scene_framebuffer = $(alloc(Framebuffer), initWithDevice, device,
		&MakeSize(w, h),
		SDL_GPU_TEXTUREFORMAT_INVALID,
		SDL_GPU_TEXTUREFORMAT_D16_UNORM);

	scene_lastTicks = SDL_GetTicks();
}

void drawScene(Renderer *renderer) {

	const RenderDevice *device = renderer->device;
	CommandBuffer *cmd = renderer->cmd;
	const SwapchainTexture swapchain = renderer->swapchain;

	const Uint64 ticks = SDL_GetTicks();
	const float dt = (float) (ticks - scene_lastTicks) / 1000.0f;
	scene_lastTicks = ticks;

	scene_angleX = SDL_fmodf(scene_angleX + dt * 30.0f, 360.0f);
	scene_angleY = SDL_fmodf(scene_angleY + dt * 60.0f, 360.0f);

	$(scene_framebuffer, resize, &swapchain.size);

	mat4 modelView = mat4_rotation(scene_angleX, vec3_new(1.f, 0.f, 0.f));
	modelView = mat4_mul(mat4_rotation(scene_angleY, vec3_new(0.f, 1.f, 0.f)), modelView);
	modelView = mat4_mul(mat4_translation(vec3_new(0.f, 0.f, -2.5f)), modelView);

	const mat4 projection = mat4_perspective(45.f,
		(float) swapchain.size.w / (float) swapchain.size.h, 0.01f, 100.f);
	const mat4 mvp = mat4_mul(projection, modelView);

	const SDL_GPUColorTargetInfo colorTarget = {
		.texture = swapchain.texture,
		.clear_color = { 0.1f, 0.1f, 0.2f, 1.0f },
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_STORE,
	};

	const SDL_GPUDepthStencilTargetInfo depthTarget = $(scene_framebuffer,
		depthTargetInfo, SDL_GPU_LOADOP_CLEAR, SDL_GPU_STOREOP_DONT_CARE, 1.f);

	RenderPass *renderPass = $(cmd, beginRenderPass, &colorTarget, 1, &depthTarget);
	$(renderPass, bindPipeline, scene_pipeline);
	$(renderPass, bindVertexBuffers, 0, &(SDL_GPUBufferBinding) { .buffer = scene_vertexBuffer }, 1);
	$(cmd, pushVertexUniformData, 0, mvp.f, sizeof(mvp));
	$(renderPass, drawPrimitives, (Uint32) SDL_arraysize(vertex_data), 1, 0, 0);
	release(renderPass);
}
