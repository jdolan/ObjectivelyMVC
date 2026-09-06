/*
 * ObjectivelyMVC: Object oriented MVC framework for SDL3 and C.
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
 * @brief A representative game HUD, for game developers to take as a starting point.
 * @details Renders health, armor and ammo counters, a crosshair, a countdown timer, a chat
 * log and a toggling scoreboard, each updating on its own interval rather than every frame,
 * as a real HUD would. The readouts use a monospaced Font, which ObjectivelyMVC draws from a
 * baked glyph atlas, so that changing them costs nothing.
 */

#define SDL_MAIN_USE_CALLBACKS

#include <stdio.h>

#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <Objectively.h>
#include <ObjectivelyMVC.h>

#define HUD_WINDOW_W 1024
#define HUD_WINDOW_H 720

/**
 * @brief SDL application state passed via pointer to callbacks.
 */
typedef struct {

  SDL_Window *window;
  RenderDevice *renderDevice;
  Framebuffer *framebuffer;
  WindowController *windowController;

  /**
   * @brief HUD widgets, borrowed references into the View hierarchy.
   */
  Label *health, *armor, *ammo, *timer;
  StackView *chat;
  Panel *scoreboard;

  /**
   * @brief Next update deadline per widget, in SDL ticks.
   */
  Uint64 healthDue, armorDue, ammoDue, timerDue, chatDue, scoreboardDue;
} AppState;

static AppState application;

#pragma mark - HUD construction

/**
 * @brief Creates a Label with the given text, aligned within its superview.
 */
static Label *label(View *superview, const char *text, ViewAlignment alignment) {

  Label *label = $(alloc(Label), initWithText, text, NULL);
  label->view.alignment = alignment;

  $(superview, addSubview, (View *) label);
  release(label);

  return label;
}

/**
 * @brief Creates a StackView aligned within its superview.
 */
static StackView *stackView(View *superview, ViewAlignment alignment) {

  StackView *stack = $(alloc(StackView), initWithFrame, NULL);
  stack->view.alignment = alignment;
  stack->view.autoresizingMask = ViewAutoresizingContain;
  stack->spacing = 4;

  $(superview, addSubview, (View *) stack);
  release(stack);

  return stack;
}

/**
 * @brief A two-tone icon: the left half in `left`, the right half in `right`, so that a
 * texture coordinate error shows as a wrong or missing half.
 */
static Image *icon(int size, Uint32 left, Uint32 right) {

  SDL_Surface *surface = SDL_CreateSurface(size, size, SDL_PIXELFORMAT_RGBA32);

  SDL_FillSurfaceRect(surface, &(SDL_Rect) { 0, 0, size / 2, size }, left);
  SDL_FillSurfaceRect(surface, &(SDL_Rect) { size / 2, 0, size - size / 2, size }, right);

  Image *image = $$(Image, imageWithSurface, surface);
  SDL_DestroySurface(surface);

  return image;
}

/**
 * @brief Builds the HUD View hierarchy on the given root View.
 */
static void buildHUD(AppState *app, View *root) {

  StackView *status = stackView(root, ViewAlignmentBottomLeft);
  app->health = label((View *) status, ":heart: 100", ViewAlignmentNone);
  app->armor = label((View *) status, "Armor 100", ViewAlignmentNone);

  app->ammo = label(root, "Ammo 50", ViewAlignmentBottomRight);
  app->timer = label(root, "10:00", ViewAlignmentTopCenter);

  // The readouts change every second or faster: the stylesheet gives them a monospaced Font,
  // so each change is a handful of quads rather than a rasterization
  Label *readouts[] = { app->health, app->armor, app->ammo, app->timer };
  for (size_t i = 0; i < SDL_arraysize(readouts); i++) {
    $((View *) readouts[i], addClassName, "readout");
  }

  // Ten solid segments sharing the Renderer's white texture: with a shared scissor these
  // collapse into one draw call, so their count is the direct check that merging works.
  StackView *segments = stackView(root, ViewAlignmentBottomCenter);
  segments->axis = StackViewAxisHorizontal;

  for (int i = 0; i < 10; i++) {
    View *segment = $(alloc(View), initWithFrame, &MakeRect(0, 0, 24, 8));
    $(segment, addClassName, "segment");
    $((View *) segments, addSubview, segment);
    release(segment);
  }

  // Eight icons of varied sizes packed into one atlas: one draw call for the whole row
  StackView *icons = stackView(root, ViewAlignmentTopRight);
  icons->axis = StackViewAxisHorizontal;

  Theme *theme = app->windowController->theme;

  const Uint32 colors[] = { 0xff0000ff, 0xff00ff00, 0xffff0000, 0xff00ffff, 0xffff00ff, 0xffffff00, 0xffffffff, 0xff808080 };

  for (int i = 0; i < 8; i++) {
    char name[16];
    snprintf(name, sizeof(name), "icon-%d", i);

    Image *image = icon(16 + 4 * i, colors[i], colors[(i + 1) % 8]);
    $(theme, addIcon, name, image);
    release(image);

    AtlasImage *atlasImage = $(theme, icon, name);

    ImageView *imageView = $(alloc(ImageView), initWithImage, (Image *) atlasImage);
    $((View *) icons, addSubview, (View *) imageView);
    release(imageView);
  }

  // Registered icons render inline in any Text as `:name:`, on both Font paths
  Image *heart = icon(32, 0xff0000ff, 0xff0000ff);
  $(theme, addIcon, "heart", heart);
  release(heart);

  label(root, "+", ViewAlignmentMiddleCenter);

  app->chat = stackView(root, ViewAlignmentTopLeft);

  Panel *scoreboard = $(alloc(Panel), initWithFrame, NULL);
  scoreboard->control.view.alignment = ViewAlignmentMiddleCenter;

  const int rows = 8;

  for (int i = 0; i < rows; i++) {
    StackView *row = $(alloc(StackView), initWithFrame, NULL);
    row->axis = StackViewAxisHorizontal;
    row->spacing = 32;
    row->view.autoresizingMask = ViewAutoresizingContain;

    Label *name = $(alloc(Label), initWithText, "Player", NULL);
    $(name->text, setTextWithFormat, "Player %d", i + 1);
    $((View *) row, addSubview, (View *) name);
    release(name);

    Label *score = $(alloc(Label), initWithText, "0", NULL);
    $(score->text, setTextWithFormat, "%d", (8 - i) * 5);
    $((View *) row, addSubview, (View *) score);
    release(score);

    $((View *) scoreboard->contentView, addSubview, (View *) row);
    release(row);
  }

  $((View *) scoreboard, setHidden, true);
  $(root, addSubview, (View *) scoreboard);
  release(scoreboard);

  app->scoreboard = scoreboard;
}

#pragma mark - HUD updates

/**
 * @brief Applies interval-based updates to the HUD widgets.
 * @details Deliberately NOT per-frame: most frames are idle, which is what a
 * real HUD looks like, and what exposes both the idle cost and the cost of a
 * single widget update.
 */
static void updateHUD(AppState *app, Uint64 ticks) {

  if (ticks >= app->healthDue) {
    app->healthDue = ticks + 2000;
    $(app->health->text, setTextWithFormat, ":heart: %d", (int) (25 + ticks / 100 % 75));
  }

  if (ticks >= app->armorDue) {
    app->armorDue = ticks + 3000;
    $(app->armor->text, setTextWithFormat, "Armor %d", (int) (ticks / 200 % 100));
  }

  if (ticks >= app->ammoDue) {
    app->ammoDue = ticks + 700;
    $(app->ammo->text, setTextWithFormat, "Ammo %d", (int) (50 - ticks / 700 % 50));
  }

  if (ticks >= app->timerDue) {
    app->timerDue = ticks + 1000;
    const int remaining = (int) (600 - ticks / 1000 % 600);
    $(app->timer->text, setTextWithFormat, "%d:%02d", remaining / 60, remaining % 60);
  }

  if (ticks >= app->chatDue) {
    app->chatDue = ticks + 4000;

    Label *message = $(alloc(Label), initWithText, NULL, NULL);
    $(message->text, setTextWithFormat, "Player %d: message at %d", (int) (ticks / 4000 % 8 + 1), (int) ticks);
    $((View *) app->chat, addSubview, (View *) message);
    release(message);

    const Array *messages = (Array *) app->chat->view.subviews;
    if (messages->count > 5) {
      View *first = $(messages, firstObject);
      $((View *) app->chat, removeSubview, first);
    }
  }

  if (ticks >= app->scoreboardDue) {
    app->scoreboardDue = ticks + 5000;
    $((View *) app->scoreboard, setHidden, !app->scoreboard->control.view.hidden);
  }
}

#pragma mark - SDL application callbacks

/**
 * @brief SDL3 application initialization callback.
 */
SDL_AppResult SDL_AppInit(void **appState, int argc, char *argv[]) {

  AppState *app = *appState = &application;

  MVC_LogSetPriority(SDL_LOG_PRIORITY_WARN);

  MVC_Assert(SDL_Init(SDL_INIT_VIDEO), "SDL_Init");

  app->window = SDL_CreateWindow("ObjectivelyMVC HUD", HUD_WINDOW_W, HUD_WINDOW_H,
                                  SDL_WINDOW_HIGH_PIXEL_DENSITY);
  MVC_Assert(app->window, "SDL_CreateWindow");

  app->renderDevice = $(alloc(RenderDevice), initWithWindow, app->window, NULL);

  int w = 0, h = 0;
  SDL_GetWindowSizeInPixels(app->window, &w, &h);

  const SDL_GPUTextureFormat format = $(app->renderDevice, getSwapchainTextureFormat);

  app->framebuffer = $(app->renderDevice, createFramebuffer, &(GPU_FramebufferCreateInfo) {
    .size = MakeSize(w, h),
    .colorAttachments = { { .format = format, .clearColor = { 0.05f, 0.05f, 0.1f, 1.f } } },
    .numColorTargets = 1,
    .sampleCount = SDL_GPU_SAMPLECOUNT_1,
  });

  $(app->renderDevice, setFramebuffer, app->framebuffer);

  app->windowController = $(alloc(WindowController), initWithDevice, app->renderDevice);

  ViewController *viewController = $(alloc(ViewController), init);
  $(app->windowController, setViewController, viewController);
  release(viewController);

  Stylesheet *stylesheet = $$(Stylesheet, stylesheetWithCharacters,
    ".segment { background-color: #40c040; } "
    ".readout Text { font-family: " DEFAULT_MONOSPACE_FONT_FAMILY "; font-size: 18; }");
  $(app->windowController->theme, addStylesheet, stylesheet);
  release(stylesheet);

  buildHUD(app, viewController->view);

  app->scoreboardDue = SDL_GetTicks() + 5000;

  return SDL_APP_CONTINUE;
}

/**
 * @brief SDL3 frame iteration callback.
 */
SDL_AppResult SDL_AppIterate(void *appState) {

  AppState *app = appState;

  updateHUD(app, SDL_GetTicks());

  CommandBuffer *commands = $(app->renderDevice, beginFrame);
  if (commands) {

    const SDL_GPUColorTargetInfo color = $(app->framebuffer, colorTargetInfo, 0, SDL_GPU_LOADOP_CLEAR, SDL_GPU_STOREOP_STORE);
    RenderPass *clear = $(commands, beginRenderPass, &color, 1, NULL);
    release(clear);

    $(app->windowController, render);

    $(app->renderDevice, endFrame);
  }

  return SDL_APP_CONTINUE;
}

/**
 * @brief SDL3 event callback.
 */
SDL_AppResult SDL_AppEvent(void *appState, SDL_Event *event) {

  AppState *app = appState;

  $(app->windowController, respondToEvent, event);

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

  $(app->renderDevice, waitForIdle);

  release(app->windowController);
  release(app->framebuffer);
  release(app->renderDevice);

  SDL_DestroyWindow(app->window);

  SDL_Quit();
}
