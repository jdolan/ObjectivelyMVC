Installing and Compiling with ObjectivelyMVC
---

### Dependencies

 * [Objectively](https://github.com/jdolan/Objectively)
 * [SDL2](https://www.libsdl.org/)
 * [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
 * [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)

### GNU Autotools

1) Do the Autotools dance.

```shell
autoreconf -i; ./configure; make; sudo make install
```

2) Include the main header file in your source.

```c
#include <ObjectivelyMVC.h>
```

3) Compile and link with ObjectivelyMVC.

```shell
gcc `pkg-config --cflags --libs ObjectivelyMVC` -o myprogram *.c
```

### Xcode 8

1. Open the versioned Xcode 8 workspace.
1. Locate the shared libraries required by the build targets.
1. Profit

### Visual Studio

1. Open the Visual Studio project file.
2. Profit.
