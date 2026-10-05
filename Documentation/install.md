Installing ObjectivelyMVC {#install}
=========================

Dependencies, building, and linking against ObjectivelyMVC.

[TOC]

## Releases

Tagged releases are published on the [GitHub releases page](https://github.com/jdolan/ObjectivelyMVC/releases). To build the latest from source, follow the steps below.

## Dependencies

* [Objectively](https://github.com/jdolan/Objectively) >= 2.2.0
* [ObjectivelyGPU](https://github.com/jdolan/ObjectivelyGPU) >= 2.2.0
* [SDL3](https://github.com/libsdl-org/SDL) >= 3.2.0, [SDL3_image](https://github.com/libsdl-org/SDL_image) and [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)

SDL3_image and SDL3_ttf MUST link the same `libSDL3` as ObjectivelyGPU, or the process loads two copies of SDL3.
If ObjectivelyGPU uses the jdolan/SDL fork, Homebrew's `sdl3_image` and `sdl3_ttf` do not qualify, because they
link Homebrew's `sdl3`. See [Installing ObjectivelyGPU](https://jdolan.github.io/ObjectivelyGPU/install.html).

The Xcode workspace builds `SDL3.framework` from the sibling checkout `../SDL`, the same checkout that
ObjectivelyGPU's workspace uses.

## Building

```sh
autoreconf -i
./configure
make && sudo make install
```

## Linking

Compile and link against ObjectivelyMVC with `pkg-config`:

```sh
gcc `pkg-config --cflags --libs ObjectivelyMVC` -o myprogram *.c
```
