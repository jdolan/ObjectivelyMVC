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

/**
 * @file
 * @brief Rotating cube background scene for the ObjectivelyMVC Hello example.
 *
 * Renders a spinning colored cube using SDL_gpu / Metal directly into the
 * Renderer's command buffer, before MVC composites the UI on top.
 * The cube scene uses renderer->cmd and renderer->swapchain, which are valid
 * between Renderer::beginFrame and Renderer::endFrame.
 *
 * Integration pattern:
 *   renderer->clear = false;              // MVC will LOAD instead of CLEAR
 *   $(renderer, beginFrame);
 *   if (renderer->cmd) {
 *     drawScene(renderer);               // cube renders into renderer->cmd
 *     // MVC layout + draw + endFrame happen inside windowController render:
 *   }
 *   $(viewController->view, layoutIfNeeded);
 *   $(viewController->view, draw, renderer);
 *   $(renderer, endFrame);
 */

#include <ObjectivelyGPU.h>

#include "Hello-Scene.h"

static const char *vertex_shader_msl =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"\n"
"struct type_UBO {\n"
"    float4x4 ModelViewProj;\n"
"};\n"
"\n"
"struct main0_out {\n"
"    float4 out_color [[user(locn0)]];\n"
"    float4 gl_Position [[position]];\n"
"};\n"
"\n"
"struct main0_in {\n"
"    float3 in_position [[attribute(0)]];\n"
"    float3 in_color [[attribute(1)]];\n"
"};\n"
"\n"
"vertex main0_out main0(main0_in in [[stage_in]], constant type_UBO& UBO [[buffer(0)]]) {\n"
"    main0_out out = {};\n"
"    out.out_color = float4(in.in_color, 1.0);\n"
"    out.gl_Position = UBO.ModelViewProj * float4(in.in_position, 1.0);\n"
"    return out;\n"
"}\n";

static const char *fragment_shader_msl =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"\n"
"struct main0_in {\n"
"    float4 in_color [[user(locn0)]];\n"
"};\n"
"\n"
"fragment float4 main0(main0_in in [[stage_in]]) {\n"
"    return in.in_color;\n"
"}\n";

typedef struct {
	float x, y, z;
	float r, g, b;
} SceneVertex;

static const SceneVertex vertex_data[] = {
	{ -0.5f,  0.5f, -0.5f,  1, 0, 0 }, {  0.5f, -0.5f, -0.5f, 0, 0, 1 }, { -0.5f, -0.5f, -0.5f, 0, 1, 0 },
	{ -0.5f,  0.5f, -0.5f,  1, 0, 0 }, {  0.5f,  0.5f, -0.5f, 1, 1, 0 }, {  0.5f, -0.5f, -0.5f, 0, 0, 1 },
	{ -0.5f,  0.5f,  0.5f,  1, 1, 1 }, { -0.5f, -0.5f, -0.5f, 0, 1, 0 }, { -0.5f, -0.5f,  0.5f, 0, 1, 1 },
	{ -0.5f,  0.5f,  0.5f,  1, 1, 1 }, { -0.5f,  0.5f, -0.5f, 1, 0, 0 }, { -0.5f, -0.5f, -0.5f, 0, 1, 0 },
	{ -0.5f,  0.5f,  0.5f,  1, 1, 1 }, {  0.5f,  0.5f, -0.5f, 1, 1, 0 }, { -0.5f,  0.5f, -0.5f, 1, 0, 0 },
	{ -0.5f,  0.5f,  0.5f,  1, 1, 1 }, {  0.5f,  0.5f,  0.5f, 0, 0, 0 }, {  0.5f,  0.5f, -0.5f, 1, 1, 0 },
	{  0.5f,  0.5f, -0.5f,  1, 1, 0 }, {  0.5f, -0.5f,  0.5f, 1, 0, 1 }, {  0.5f, -0.5f, -0.5f, 0, 0, 1 },
	{  0.5f,  0.5f, -0.5f,  1, 1, 0 }, {  0.5f,  0.5f,  0.5f, 0, 0, 0 }, {  0.5f, -0.5f,  0.5f, 1, 0, 1 },
	{  0.5f,  0.5f,  0.5f,  0, 0, 0 }, { -0.5f, -0.5f,  0.5f, 0, 1, 1 }, {  0.5f, -0.5f,  0.5f, 1, 0, 1 },
	{  0.5f,  0.5f,  0.5f,  0, 0, 0 }, { -0.5f,  0.5f,  0.5f, 1, 1, 1 }, { -0.5f, -0.5f,  0.5f, 0, 1, 1 },
	{ -0.5f, -0.5f, -0.5f,  0, 1, 0 }, {  0.5f, -0.5f,  0.5f, 1, 0, 1 }, { -0.5f, -0.5f,  0.5f, 0, 1, 1 },
	{ -0.5f, -0.5f, -0.5f,  0, 1, 0 }, {  0.5f, -0.5f, -0.5f, 0, 0, 1 }, {  0.5f, -0.5f,  0.5f, 1, 0, 1 },
};

static SDL_GPUBuffer *scene_vertexBuffer;
static SDL_GPUGraphicsPipeline *scene_pipeline;
static SDL_GPUTexture *scene_depthTexture;
static SDL_Size scene_depthSize;
static float scene_angleX;
static float scene_angleY;
static Uint64 scene_lastTicks;

static void rotate_matrix(float angle, float x, float y, float z, float *r) {
	float radians = angle * SDL_PI_F / 180.0f;
	float c = SDL_cosf(radians), s = SDL_sinf(radians), c1 = 1.0f - c;
	float length = SDL_sqrtf(x*x + y*y + z*z);
	float u[3] = { x/length, y/length, z/length };
	for (int i = 0; i < 16; i++) r[i] = 0.0f;
	r[15] = 1.0f;
	for (int i = 0; i < 3; i++) {
		r[i*4 + (i+1)%3] = u[(i+2)%3] * s;
		r[i*4 + (i+2)%3] = -u[(i+1)%3] * s;
	}
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			r[i*4+j] += c1 * u[i] * u[j] + (i == j ? c : 0.0f);
}

static void perspective_matrix(float fovy, float aspect, float znear, float zfar, float *r) {
	float f = 1.0f / SDL_tanf((fovy / 180.0f) * SDL_PI_F * 0.5f);
	for (int i = 0; i < 16; i++) r[i] = 0.0f;
	r[0] = f / aspect; r[5] = f;
	r[10] = (znear + zfar) / (znear - zfar); r[11] = -1.0f;
	r[14] = (2.0f * znear * zfar) / (znear - zfar);
}

static void multiply_matrix(const float *lhs, const float *rhs, float *r) {
	float tmp[16];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			tmp[j*4+i] = 0.0f;
			for (int k = 0; k < 4; k++)
				tmp[j*4+i] += lhs[k*4+i] * rhs[j*4+k];
		}
	for (int i = 0; i < 16; i++) r[i] = tmp[i];
}

void initScene(Renderer *renderer) {

	const RenderDevice *device = renderer->device;

	scene_vertexBuffer = $(device, createBuffer, &(SDL_GPUBufferCreateInfo) {
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
		.size = sizeof(vertex_data),
	});

	SDL_GPUTransferBuffer *transfer = $(device, createTransferBuffer, &(SDL_GPUTransferBufferCreateInfo) {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = sizeof(vertex_data),
	});
	void *mapped = $(device, mapTransferBuffer, transfer, false);
	SDL_memcpy(mapped, vertex_data, sizeof(vertex_data));
	$(device, unmapTransferBuffer, transfer);

	CommandBuffer *cmd = $(device, acquireCommandBuffer);
	CopyPass *copyPass = $(cmd, beginCopyPass);
	$(copyPass, uploadBuffer,
	  &(SDL_GPUTransferBufferLocation) { .transfer_buffer = transfer },
	  &(SDL_GPUBufferRegion) { .buffer = scene_vertexBuffer, .size = sizeof(vertex_data) },
	  false);
	release(copyPass);
	$(cmd, submit);
	release(cmd);
	$(device, releaseTransferBuffer, transfer);

	SDL_GPUShader *vs = $(device, createShader, &(SDL_GPUShaderCreateInfo) {
		.code_size = SDL_strlen(vertex_shader_msl),
		.code = (const Uint8 *) vertex_shader_msl,
		.entrypoint = "main0",
		.format = SDL_GPU_SHADERFORMAT_MSL,
		.stage = SDL_GPU_SHADERSTAGE_VERTEX,
		.num_uniform_buffers = 1,
	});
	SDL_GPUShader *fs = $(device, createShader, &(SDL_GPUShaderCreateInfo) {
		.code_size = SDL_strlen(fragment_shader_msl),
		.code = (const Uint8 *) fragment_shader_msl,
		.entrypoint = "main0",
		.format = SDL_GPU_SHADERFORMAT_MSL,
		.stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
	});

	const SDL_GPUVertexBufferDescription vbd = {
		.slot = 0,
		.pitch = sizeof(SceneVertex),
		.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
	};
	const SDL_GPUVertexAttribute attrs[] = {
		{ .location = 0, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, .offset = 0 },
		{ .location = 1, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, .offset = 12 },
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

	scene_lastTicks = SDL_GetTicks();
}

void drawScene(Renderer *renderer) {

	const RenderDevice *device = renderer->device;
	CommandBuffer *cmd = renderer->cmd;
	const Swapchain swapchain = renderer->swapchain;

	const Uint64 ticks = SDL_GetTicks();
	const float dt = (float) (ticks - scene_lastTicks) / 1000.0f;
	scene_lastTicks = ticks;

	scene_angleX = SDL_fmodf(scene_angleX + dt * 30.0f, 360.0f);
	scene_angleY = SDL_fmodf(scene_angleY + dt * 60.0f, 360.0f);

	if (scene_depthTexture == NULL || swapchain.size.w != scene_depthSize.w || swapchain.size.h != scene_depthSize.h) {
		scene_depthSize = swapchain.size;
		$(device, releaseTexture, scene_depthTexture);
		scene_depthTexture = $(device, createTexture, &(SDL_GPUTextureCreateInfo) {
			.type = SDL_GPU_TEXTURETYPE_2D,
			.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
			.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
			.width = (Uint32) scene_depthSize.w,
			.height = (Uint32) scene_depthSize.h,
			.layer_count_or_depth = 1,
			.num_levels = 1,
			.sample_count = SDL_GPU_SAMPLECOUNT_1,
		}, NULL);
	}

	float mv[16], rot[16], proj[16], mvp[16];
	rotate_matrix(scene_angleX, 1.0f, 0.0f, 0.0f, mv);
	rotate_matrix(scene_angleY, 0.0f, 1.0f, 0.0f, rot);
	multiply_matrix(rot, mv, mv);
	mv[14] -= 2.5f;
	perspective_matrix(45.0f, (float) swapchain.size.w / (float) swapchain.size.h, 0.01f, 100.0f, proj);
	multiply_matrix(proj, mv, mvp);

	const SDL_GPUColorTargetInfo colorTarget = {
		.texture = swapchain.texture,
		.clear_color = { 0.1f, 0.1f, 0.2f, 1.0f },
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_STORE,
	};
	const SDL_GPUDepthStencilTargetInfo depthTarget = {
		.texture = scene_depthTexture,
		.clear_depth = 1.0f,
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_DONT_CARE,
		.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
		.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE,
	};

	RenderPass *renderPass = $(cmd, beginRenderPass, &colorTarget, 1, &depthTarget);
	$(renderPass, bindPipeline, scene_pipeline);
	$(renderPass, bindVertexBuffers, 0, &(SDL_GPUBufferBinding) { .buffer = scene_vertexBuffer }, 1);
	$(cmd, pushVertexUniformData, 0, mvp, sizeof(mvp));
	$(renderPass, drawPrimitives, (Uint32) SDL_arraysize(vertex_data), 1, 0, 0);
	release(renderPass);
}

