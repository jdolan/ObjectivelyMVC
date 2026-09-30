Installing ObjectivelyMVC {#install}
=========================

Dependencies, building, and linking against ObjectivelyMVC.

[TOC]

## Releases

Tagged releases are published on the [GitHub releases page](https://github.com/jdolan/ObjectivelyMVC/releases). To build the latest from source, follow the steps below.

## Dependencies

* [Objectively](https://github.com/jdolan/Objectively) >= 2.2.0
* [ObjectivelyGPU](https://github.com/jdolan/ObjectivelyGPU) >= 2.2.0
* [SDL3](https://github.com/libsdl-org/SDL) >= 3.2.0 (CI builds against the `ObjectivelyGPU` tag of [jdolan/SDL](https://github.com/jdolan/SDL), which adds the SDL_gpu query API; see [Installing ObjectivelyGPU](https://jdolan.github.io/ObjectivelyGPU/install.html)), [SDL3_image](https://github.com/libsdl-org/SDL_image), [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)

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
