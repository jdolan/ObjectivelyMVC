ObjectivelyMVC Documentation {#index}
===

Object oriented MVC framework for OpenGL, SDL2 and GNU C
---

ObjectivelyMVC is a user interface and interaction framework for [SDL2](http://www.libsdl.org) inspired by 
Apple's [AppKit](https://developer.apple.com/reference/appkit). 
It is geared towards building high-quality, modern looking user interfaces within [OpenGL](http://www.opengl.org/) 
video games that are already using SDL2. It is written in [GNU C](http://www.gnu.org/software/gnu-c-manual/), and requires `gcc` or `clang`.


Features
---
### Object-oriented Model-View-Controller implementation in C

ObjectivelyMVC is built on [Objectively](https://github.com/jdolan/Objectively), an ultra-lightweight Object-oriented framework for [GNU C](http://www.gnu.org/software/gnu-c-manual/). ObjectivelyMVC delivers the elegance of OO / MVC without imposing C++ on your project. If you're' using C++ or Objective-C, ObjectivelyMVC is perfectly happy alongside those, too.

```c
WindowController *windowController = $(alloc(WindowController), initWithWindow, window);
```

### Easily embeddable in any SDL2 / OpenGL application

ObjectivelyMVC is purpose-built for video games. Unlike Gtk+, Qt, wxWidgets, FLTK, ..ObjectivelyMVC **does not hijack the main loop**. ObjectivelyMVC does not create a window, manage an OpenGL context, or originate events. Your game already does that, because it has to. Like your mother, ObjectivelyMVC only asks that you give it a call once in a while. That's it.

```c
$(windowController, respondToEvent, &event);
...
$(windowController, render, renderer);
```

### Beautiful, discoverable TrueType fonts

ObjectivelyMVC uses [Fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/) and [SDL_ttf](https://www.libsdl.org/projects/SDL_ttf/) to discover and render the TrueType fonts that are available on your system. It also automatically detects High-DPI (Retina, 4K) displays, and scales fonts accordingly. The result is crisp, beautiful vector-based fonts that look native, because they are.

```c
Array *fonts = $$(Font, allFonts);
...
release(fonts);
...
Font *verdana = $(alloc(Font), initWithAttributes, "Verdana", 24, 0); // will render at 48pt on Retina displays
```

### Full suite of Views and Controls

ObjectivelyMVC provides a robust set of containers, views and form elements. Stack and arrange components with `Box`, `Panel` and `StackView`. Add `Buttons`, `Checkboxes`, `Selects`, `Sliders`, editable `TextViews` and more by simply instantiating them. Bind `Actions` to `SDL_Event` types on each element, or use the specialized _delegate_ callbacks for convenience.

```c
$((Control *) button, addActionForEventType, SDL_MOUSEBUTTONUP, my_callback, my_sender, my_data);
```

```c
Select *select = $(alloc(Select), initWithFrame, NULL, ControlStyleDefault);
...
select->delegate.didSelectOption = my_callback;
```

Examples
---

### HelloViewController
An example application that creates a Window, enters its main loop and draws a scene before rendering a simple menu:

![Hello](../demo.gif)

* [Hello.c](Examples/Hello.c) - The application source code
* [HelloViewController.h](Examples/HelloViewController.h) - The `HelloViewController` header.
* [HelloViewController.c](Examples/HelloViewController.c) - The `HelloViewController` source code. 

### Quetoo
[Quetoo](https://github.com/jdolan/quetoo) is an open source FPS based on [idTech2](https://en.wikipedia.org/wiki/Quake_II_engine). Quetoo uses ObjectivelyMVC for its in-game user interface:

![Quetoo](../quetoo.jpg)

Documentation
---

Browse the [Class hierarchy](hierarchy.html) to grok the API.

Dependencies
---

* [Objectively](https://github.com/jdolan/Objectively)
* [Fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/)
* [SDL2](https://www.libsdl.org/)
* [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
* [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)
