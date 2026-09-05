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
 * @brief A representative game HUD, used as a performance benchmark.
 * @details Renders health, armor and ammo counters, a crosshair, a countdown
 * timer, a chat log and a toggling scoreboard, each updating on an interval.
 * The frame passes (style, layout, draw, endFrame) are timed individually and
 * a summary is printed once per second.
 *
 * Environment:
 *  - `MVC_HUD_FRAMES=N` exits successfully after N frames (for benchmarking).
 *  - `MVC_HUD_HIDDEN=1` creates the window hidden (best effort headless).
 *  - `MVC_HUD_SCALE=N` multiplies the scoreboard row count (default 1),
 *    scaling the View tree to gauge how frame cost grows with UI complexity.
 *  - `MVC_HUD_FONT=path` names a fixed-width face to bake the readouts' BitmapFont from,
 *    ahead of the system faces tried by default.
 *  - `MVC_HUD_BITMAP_FONT=0` leaves the readouts on Font, for comparison.
 *  - `MVC_HUD_DEBUG=1` prints the draw call list once, on the tenth frame.
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
 * @brief Accumulated timing for one frame pass.
 */
typedef struct {
  double sum;
  double max;
} PassStats;

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
   * @brief The atlas behind the icon row and the readouts' BitmapFont.
   */
  ImageAtlas *atlas;

  /**
   * @brief The fixed-width BitmapFont behind the readouts, or `NULL` if no fixed-width face
   * was found on this system.
   */
  BitmapFont *bitmapFont;

  /**
   * @brief Next update deadline per widget, in SDL ticks.
   */
  Uint64 healthDue, armorDue, ammoDue, timerDue, chatDue, scoreboardDue;

  /**
   * @brief Frame counters and per-pass timing since the last report.
   */
  Uint64 frames, maxFrames, reportDue, reportFrames;
  PassStats acquire, style, layout, draw, endFrame, submit;
  size_t draws, vertices;
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
 * @brief Bakes a BitmapFont from the first fixed-width face found on this system, so that the
 * readouts share the icon atlas and cost nothing to change. MVC_HUD_FONT names a face to try
 * first; MVC_HUD_BITMAP_FONT=0 disables the BitmapFont, leaving the readouts on Font, for
 * comparison.
 */
static BitmapFont *bakeBitmapFont(ImageAtlas *atlas, WindowController *windowController) {

  const char *enabled = SDL_getenv("MVC_HUD_BITMAP_FONT");
  if (enabled && *enabled == '0') {
    return NULL;
  }

  const char *paths[] = {
    SDL_getenv("MVC_HUD_FONT"),
    "/System/Library/Fonts/Menlo.ttc",
    "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
    "C:\\Windows\\Fonts\\consola.ttf",
  };

  for (size_t i = 0; i < SDL_arraysize(paths); i++) {
    Data *data = paths[i] ? $$(Data, dataWithContentsOfFile, paths[i]) : NULL;
    if (data) {
      $$(Font, cacheFont, data, "Mono");
      release(data);
      break;
    }
  }

  const FontAttributes attributes = { "Mono", 18, FontStyleRegular };
  Font *font = $(windowController->theme, font, &attributes,
                 SDL_GetWindowPixelDensity(windowController->window));

  Image *heart = icon(32, 0xff0000ff, 0xff0000ff);
  Dictionary *named = $$(Dictionary, dictionaryWithObjectsAndKeys, heart, str("heart"), NULL);

  BitmapFont *bitmapFont = $(alloc(BitmapFont), initWithFont, font, ' ', 95, named, atlas);

  release(named);
  release(heart);

  return bitmapFont;
}

/**
 * @brief Builds the HUD View hierarchy on the given root View.
 */
static void buildHUD(AppState *app, View *root) {

  StackView *status = stackView(root, ViewAlignmentBottomLeft);
  app->health = label((View *) status, "Health 100", ViewAlignmentNone);
  app->armor = label((View *) status, "Armor 100", ViewAlignmentNone);

  app->ammo = label(root, "Ammo 50", ViewAlignmentBottomRight);
  app->timer = label(root, "10:00", ViewAlignmentTopCenter);

  // The readouts change every second or faster: on the BitmapFont, a change costs nothing,
  // and they draw in the same call as the icons that share the atlas
  if (app->bitmapFont) {
    Label *readouts[] = { app->health, app->armor, app->ammo, app->timer };
    for (size_t i = 0; i < SDL_arraysize(readouts); i++) {
      readouts[i]->text->colorEscapes = true;
      $(readouts[i]->text, setBitmapFont, app->bitmapFont);
    }
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

  const Uint32 colors[] = { 0xff0000ff, 0xff00ff00, 0xffff0000, 0xff00ffff, 0xffff00ff, 0xffffff00, 0xffffffff, 0xff808080 };

  for (int i = 0; i < 8; i++) {
    Image *image = icon(16 + 4 * i, colors[i], colors[(i + 1) % 8]);
    AtlasImage *atlasImage = $(app->atlas, addImage, image);
    release(image);

    ImageView *imageView = $(alloc(ImageView), initWithImage, (Image *) atlasImage);
    $((View *) icons, addSubview, (View *) imageView);
    release(imageView);
  }

  MVC_Assert($(app->atlas, compile), "ImageAtlas::compile");

  label(root, "+", ViewAlignmentMiddleCenter);

  app->chat = stackView(root, ViewAlignmentTopLeft);

  Panel *scoreboard = $(alloc(Panel), initWithFrame, NULL);
  scoreboard->control.view.alignment = ViewAlignmentMiddleCenter;

  int rows = 8;

  const char *scale = SDL_getenv("MVC_HUD_SCALE");
  if (scale) {
    rows *= SDL_max(1, SDL_atoi(scale));
  }

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
    if (app->bitmapFont) {
      $(app->health->text, setTextWithFormat, ":heart: ^%d%d", (int) (ticks / 2000 % 10), (int) (25 + ticks / 100 % 75));
    } else {
      $(app->health->text, setTextWithFormat, "Health %d", (int) (25 + ticks / 100 % 75));
    }
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

#pragma mark - Timing

/**
 * @return Elapsed microseconds between the given performance counter values.
 */
static double microseconds(Uint64 start, Uint64 end) {
  return (end - start) * 1e6 / (double) SDL_GetPerformanceFrequency();
}

/**
 * @brief Accumulates one sample into the given PassStats.
 */
static void sample(PassStats *stats, Uint64 start, Uint64 end) {

  const double us = microseconds(start, end);

  stats->sum += us;
  stats->max = SDL_max(stats->max, us);
}

/**
 * @brief Prints the per-pass summary and resets the accumulators.
 */
static void report(AppState *app) {

  const double n = (double) app->reportFrames;

  printf("HUD %llu frames | acquire avg %.1fus max %.1fus | style avg %.1fus max %.1fus | "
         "layout avg %.1fus max %.1fus | draw avg %.1fus max %.1fus | endFrame avg %.1fus max %.1fus | "
         "submit avg %.1fus max %.1fus | draws %zu verts %zu\n",
         (unsigned long long) app->reportFrames,
         app->acquire.sum / n, app->acquire.max,
         app->style.sum / n, app->style.max,
         app->layout.sum / n, app->layout.max,
         app->draw.sum / n, app->draw.max,
         app->endFrame.sum / n, app->endFrame.max,
         app->submit.sum / n, app->submit.max,
         app->draws, app->vertices);

  app->reportFrames = 0;
  app->acquire = app->style = app->layout = app->draw = app->endFrame = app->submit = (PassStats) { 0 };
}

#pragma mark - SDL application callbacks

/**
 * @brief SDL3 application initialization callback.
 */
SDL_AppResult SDL_AppInit(void **appState, int argc, char *argv[]) {

  AppState *app = *appState = &application;

  MVC_LogSetPriority(SDL_LOG_PRIORITY_WARN);

  MVC_Assert(SDL_Init(SDL_INIT_VIDEO), "SDL_Init");

  SDL_WindowFlags flags = SDL_WINDOW_HIGH_PIXEL_DENSITY;

  const char *hidden = SDL_getenv("MVC_HUD_HIDDEN");
  if (hidden && *hidden == '1') {
    flags |= SDL_WINDOW_HIDDEN;
  }

  const char *frames = SDL_getenv("MVC_HUD_FRAMES");
  if (frames) {
    app->maxFrames = SDL_strtoull(frames, NULL, 10);
  }

  app->window = SDL_CreateWindow("ObjectivelyMVC HUD", HUD_WINDOW_W, HUD_WINDOW_H, flags);
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

  Stylesheet *stylesheet = $$(Stylesheet, stylesheetWithCharacters, ".segment { background-color: #40c040; }");
  $(app->windowController->theme, addStylesheet, stylesheet);
  release(stylesheet);

  app->atlas = $(alloc(ImageAtlas), init);
  app->bitmapFont = bakeBitmapFont(app->atlas, app->windowController);

  buildHUD(app, viewController->view);

  app->scoreboardDue = SDL_GetTicks() + 5000;
  app->reportDue = SDL_GetTicks() + 1000;

  return SDL_APP_CONTINUE;
}

/**
 * @brief SDL3 frame iteration callback.
 * @details Hand-rolls WindowController::renderTo in order to time each pass
 * (style, layout, draw, endFrame) individually.
 */
SDL_AppResult SDL_AppIterate(void *appState) {

  AppState *app = appState;

  const Uint64 ticks = SDL_GetTicks();

  updateHUD(app, ticks);

  const Uint64 tAcquire = SDL_GetPerformanceCounter();
  CommandBuffer *commands = $(app->renderDevice, beginFrame);
  if (commands) {

    const SDL_GPUColorTargetInfo color = $(app->framebuffer, colorTargetInfo, 0, SDL_GPU_LOADOP_CLEAR, SDL_GPU_STOREOP_STORE);
    RenderPass *clear = $(commands, beginRenderPass, &color, 1, NULL);
    release(clear);

    WindowController *windowController = app->windowController;
    View *view = windowController->viewController->view;
    Renderer *renderer = windowController->renderer;

    $(renderer, beginFrameWith, commands, app->framebuffer);

    const Uint64 t0 = SDL_GetPerformanceCounter();
    $(view, applyThemeIfNeeded, windowController->theme);

    const Uint64 t1 = SDL_GetPerformanceCounter();
    $(view, layoutIfNeeded);

    MVC_InvalidateRenderFrames();

    const Uint64 t2 = SDL_GetPerformanceCounter();
    $(view, draw, renderer);

    const Uint64 t3 = SDL_GetPerformanceCounter();
    app->draws = renderer->drawArrays->count;
    if (app->frames == 10 && SDL_getenv("MVC_HUD_DEBUG")) {
      for (size_t i = 0; i < renderer->drawArrays->count; i++) {
        const MVC_DrawArrays *d = VectorElement(renderer->drawArrays, MVC_DrawArrays, i);
        printf("draw %zu: texture %p verts %u scissor %d,%d %dx%d\n", i, (void *) d->texture, d->vertexCount, d->scissor.x, d->scissor.y, d->scissor.w, d->scissor.h);
      }
    }
    app->vertices = renderer->vertices->count;
    $(renderer, endFrame);

    const Uint64 t4 = SDL_GetPerformanceCounter();
    $(app->renderDevice, endFrame);

    const Uint64 t5 = SDL_GetPerformanceCounter();

    sample(&app->acquire, tAcquire, t0);
    sample(&app->style, t0, t1);
    sample(&app->layout, t1, t2);
    sample(&app->draw, t2, t3);
    sample(&app->endFrame, t3, t4);
    sample(&app->submit, t4, t5);
    app->reportFrames++;
  }

  app->frames++;

  if (ticks >= app->reportDue && app->reportFrames) {
    app->reportDue = ticks + 1000;
    report(app);
  }

  if (app->maxFrames && app->frames >= app->maxFrames) {
    if (app->reportFrames) {
      report(app);
    }
    return SDL_APP_SUCCESS;
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
  release(app->bitmapFont);
  release(app->atlas);
  release(app->framebuffer);
  release(app->renderDevice);

  SDL_DestroyWindow(app->window);

  SDL_Quit();
}
