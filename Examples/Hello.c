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

#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <Objectively.h>
#include <ObjectivelyMVC.h>

#include "HelloViewController.h"

#include "click.wav.h"
#include "clack.wav.h"

#ifdef SDL_PLATFORM_IOS
# define HELLO_WINDOW_W      0
# define HELLO_WINDOW_H      0
# define HELLO_WINDOW_FLAGS  (SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_FULLSCREEN)
#else
# define HELLO_WINDOW_W      1024
# define HELLO_WINDOW_H      720
# define HELLO_WINDOW_FLAGS  SDL_WINDOW_HIGH_PIXEL_DENSITY
#endif

typedef struct {
  SDL_GPUBuffer *vertexBuffer;
  Framebuffer *framebuffer;
  SDL_GPUGraphicsPipeline *pipeline;
  vec2 angles;
} Scene;

/**
 * @brief SDL application state passed via pointer to callbacks.
 */
typedef struct {

  /**
   * @brief The @c SDL_AudioStream and @c SDL_Window.
   */
  SDL_AudioStream *audioStream;
  SDL_Window *window;

  /**
   * @brief The ObjectivelyGPU @c RenderDevice.
   */
  RenderDevice *renderDevice;

  /**
   * @brief The ObjectiveMVC @c WindowController.
   */
	WindowController *windowController;

  /**
   * @brief Simulation time in milliseconds.
   */
  Uint64 ticks;

  /**
   * @brief Delta frame time in seconds.
   */
  float dt;

  /**
   * @brief The @c Scene.
   */
  Scene scene;
} AppState;

static AppState application;

#pragma mark - Scene management

typedef struct {
  vec3 position;
  vec3 color;
} Vertex;

static const Vertex vertexes[] = {
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

/**
 * @brief Initializes the @c Scene.
 */
static void initScene(AppState *app) {

  Scene *scene = &app->scene;

  int w = 0, h = 0;
  SDL_GetWindowSizeInPixels(app->window, &w, &h);

  scene->framebuffer = $(alloc(Framebuffer), initWithDevice, app->renderDevice,
                      &MakeSize(w, h),
                      SDL_GPU_TEXTUREFORMAT_INVALID,
                      SDL_GPU_TEXTUREFORMAT_D16_UNORM);

  scene->vertexBuffer = $(app->renderDevice, createBufferWithConstMem, SDL_GPU_BUFFERUSAGE_VERTEX, vertexes, sizeof(vertexes));

  SDL_GPUShader *vertexShader = $(app->renderDevice, loadShader, "Hello.vert", &(SDL_GPUShaderCreateInfo) {
    .stage = SDL_GPU_SHADERSTAGE_VERTEX,
    .num_uniform_buffers = 1,
  });

  SDL_GPUShader *fragmentShader = $(app->renderDevice, loadShader, "Hello.frag", &(SDL_GPUShaderCreateInfo) {
    .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
  });

  const SDL_GPUVertexBufferDescription vbd = {
    .slot = 0,
    .pitch = sizeof(Vertex),
    .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
  };

  const SDL_GPUVertexAttribute attrs[] = {
    {
      .location = 0,
      .buffer_slot = 0,
      .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
      .offset = offsetof(Vertex, position),
    },
    {
      .location = 1,
      .buffer_slot = 0,
      .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
      .offset = offsetof(Vertex, color),
    },
  };

  const SDL_GPUColorTargetDescription ctd = {
    .format = $(app->renderDevice, getSwapchainTextureFormat, app->window),
  };

  scene->pipeline = $(app->renderDevice, createGraphicsPipeline, &(SDL_GPUGraphicsPipelineCreateInfo) {
    .vertex_shader = vertexShader,
    .fragment_shader = fragmentShader,
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

  $(app->renderDevice, releaseShader, vertexShader);
  $(app->renderDevice, releaseShader, fragmentShader);
}

/**
 * @brief Renders a single frame of the @c Scene into the given command buffer and swapchain.
 */
static void drawScene(AppState *app, CommandBuffer *cmd, const SwapchainTexture *swapchain) {

  const Uint64 ticks = SDL_GetTicks();
  app->dt = (ticks - app->ticks) / 1000.f;
  app->ticks = ticks;

  Scene *scene = &app->scene;

  $(scene->framebuffer, resize, &swapchain->size);

  scene->angles.x = SDL_fmodf(scene->angles.x + app->dt * 30.f, 360.f);
  scene->angles.y = SDL_fmodf(scene->angles.y + app->dt * 60.f, 360.f);

  mat4 modelView = mat4_rotation(scene->angles.x, vec3_new(1.f, 0.f, 0.f));
  modelView = mat4_mul(mat4_rotation(scene->angles.y, vec3_new(0.f, 1.f, 0.f)), modelView);
  modelView = mat4_mul(mat4_translation(vec3_new(0.f, 0.f, -2.5f)), modelView);

  const mat4 projection = mat4_perspective(45.f, (float) swapchain->size.w / (float) swapchain->size.h, 0.01f, 100.f);
  const mat4 modelViewProjection = mat4_mul(projection, modelView);

  $(cmd, pushVertexUniformData, 0, modelViewProjection.f, sizeof(modelViewProjection));

  const SDL_GPUColorTargetInfo sceneColor = {
    .texture = swapchain->texture,
    .load_op = SDL_GPU_LOADOP_CLEAR,
    .store_op = SDL_GPU_STOREOP_STORE,
    .clear_color = { 0.1f, 0.1f, 0.2f, 1.f },
  };

  const SDL_GPUDepthStencilTargetInfo depthTarget = $(scene->framebuffer, depthTargetInfo, SDL_GPU_LOADOP_CLEAR, SDL_GPU_STOREOP_DONT_CARE, 1.f);

  RenderPass *pass = $(cmd, beginRenderPass, &sceneColor, 1, &depthTarget);
  $(pass, bindPipeline, scene->pipeline);
  $(pass, bindVertexBuffers, 0, &(SDL_GPUBufferBinding) { .buffer = scene->vertexBuffer }, 1);
  $(pass, drawPrimitives, (Uint32) SDL_arraysize(vertexes), 1, 0, 0);
  release(pass);
}

#pragma mark - Event handlers

/**
 * @brief `MVC_VIEW_EVENT` handler.
 */
static void onViewEvent(const SDL_UserEvent *event) {

  AppState *app = event->data2;
  View *view = event->data1;

  if (!instanceof(Control, view)) {
    return;
  }

  switch (event->code) {
    case ViewEventClick:
      SDL_PutAudioStreamData(app->audioStream, click_wav, click_wav_len);
      break;
    case ViewEventChange:
      SDL_PutAudioStreamData(app->audioStream, clack_wav, clack_wav_len);
      break;
    default:
      break;
  }
}

#pragma mark - SDL application callbacks

/**
 * @brief SDL3 application initialization callback.
 */
SDL_AppResult SDL_AppInit(void **appState, int argc, char *argv[]) {

  AppState *app = *appState = &application;

	MVC_LogSetPriority(SDL_LOG_PRIORITY_DEBUG);

	MVC_Assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO), "SDL_Init");

#ifdef EXAMPLES
	$$(Resource, addResourcePath, EXAMPLES);
#endif

	app->audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &(SDL_AudioSpec) {
		.format = SDL_AUDIO_S16LE,
		.channels = 1,
		.freq = 22050,
	}, NULL, NULL);

  MVC_Assert(app->audioStream, "SDL_OpenAudioDeviceStream");
	SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(app->audioStream));

	app->window = SDL_CreateWindow("Hello ObjectivelyMVC", HELLO_WINDOW_W, HELLO_WINDOW_H, HELLO_WINDOW_FLAGS);
	MVC_Assert(app->window, "SDL_CreateWindow");

  app->renderDevice = $(alloc(RenderDevice), initWithWindow, app->window);
	app->windowController = $(alloc(WindowController), initWithDevice, app->renderDevice);

	ViewController *viewController = (ViewController *) $(alloc(HelloViewController), init);
	$(app->windowController, setViewController, viewController);
	release(viewController);

	initScene(app);

	return SDL_APP_CONTINUE;
}

/**
 * @brief SDL3 frame iteration callback.
 */
SDL_AppResult SDL_AppIterate(void *appState) {

  AppState *app = appState;

  CommandBuffer *cmd = $(app->renderDevice, acquireCommandBuffer);

  SwapchainTexture swapchain = { 0 };
  $(cmd, waitAndAcquireSwapchainTexture, &swapchain);

  if (!swapchain.texture) {
    $(cmd, cancel);
    release(cmd);
    return SDL_APP_CONTINUE;
  }

  drawScene(app, cmd, &swapchain);

  const SDL_GPUColorTargetInfo uiColor = {
    .texture = swapchain.texture,
    .load_op = SDL_GPU_LOADOP_LOAD,
    .store_op = SDL_GPU_STOREOP_STORE,
  };

  $(app->windowController, renderWith, cmd, &uiColor, swapchain.size);

  $(app->renderDevice, submit, cmd);
  release(cmd);

	return SDL_APP_CONTINUE;
}

/**
 * @brief SDL3 event callback.
 */
SDL_AppResult SDL_AppEvent(void *appState, SDL_Event *event) {

  AppState *app = appState;

	$(app->windowController, respondToEvent, event);

	if (event->type == MVC_VIEW_EVENT) {
    SDL_UserEvent *user = &event->user;
    user->data2 = appState;
    onViewEvent(user);
	}

	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}

	return SDL_APP_CONTINUE;
}

/**
 * @brief SDL3 quit callback.
 */
void SDL_AppQuit(void *appState, SDL_AppResult result) {

  AppState *app = appState;

	release(app->windowController);

	SDL_DestroyAudioStream(app->audioStream);
	SDL_DestroyWindow(app->window);

	SDL_Quit();
}
