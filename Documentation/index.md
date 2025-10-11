ObjectivelyMVC Documentation {#index}
===

Cross-platform object oriented MVC framework for OpenGL, SDL2 and GNU C
---

ObjectivelyMVC is a cross-platform user interface and interaction framework for [SDL2](http://www.libsdl.org) inspired by Apple's [AppKit](https://developer.apple.com/reference/appkit). It is geared towards building high-quality, modern looking user interfaces within [OpenGL](http://www.opengl.org/) video games that are already using SDL2. It is built on [Objectively](https://github.com/jdolan/Objectively), written in [GNU C](http://www.gnu.org/software/gnu-c-manual/), and requires `gcc` or `clang`.

Features
---

### Cross platform

_Wait, `gcc` or `clang`? Is this a Linux-only framework?_ Not at all. ObjectivelyMVC supports Windows XP and later, Apple OS X, and Linux. Support for other Unix derivatives is likely there, but untested. To build ObjectivelyMVC for Windows, you can use either MinGW-w64 (cross-compile or native), or recent [Visual Studio 15](https://www.visualstudio.com/), which includes Clang.

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
$(windowController, render);
```

### Beautiful 4K-ready fonts

ObjectivelyMVC uses [SDL_ttf](https://www.libsdl.org/projects/SDL_ttf/) to render TrueType fonts. It also automatically detects High-DPI (Retina, 4K) displays, and scales fonts accordingly. The result is crisp, beautiful vector-based fonts that look native, because they are.

```c
Data *data = $$(Data, dataWithContentsOfFile, "Verdana.ttf");
...
$$(Font, cacheFont, data, "Verdana");
...
Font *verdana = $$(Font, cachedFont, "Verdana", 24, FontStyleRegular); // will render at 48pt on Retina displays
```

### Full suite of Views and Controls

ObjectivelyMVC provides a robust set of containers, views and controls. Stack and arrange components with `Box`, `Panel` and `StackView`. Add `Buttons`, `Checkboxes`, `Selects`, `Sliders`, editable `TextViews` and more by simply instantiating them. Display tabular data or a thumbnail gallery with `TableView` and `CollectionView`. Split complex interfaces into multiple tabs with `TabView` and `TabViewController`. Use the _delegate_ callbacks to respond to events.


```c
Select *select = $(alloc(Select), initWithFrame, NULL);
...
select->delegate.didSelectOption = my_callback;
```

### Programmatic or JSON-based layout options

ObjectivelyMVC allows you to define your View hierarchy programmatically, via JSON, or using any combination of both. Programmatic layout gives you explicit control over the big picture, while JSON layout allows you to reduce boilerplate and avoid common pitfalls like memory leaks.

```json
{
	"class": "Panel",
	"style": {
		"top": 50,
		"left": 50
	},
	"contentView": {
		"subviews": [{
			"class": "Input",
			"control": {
				"class": "Checkbox",
				"identifier": "checkbox"
			},
			"label": {
				"text": "This is a checkbox:"
			}
		}]
	}
}
```

Examples
---

### HelloViewController
An example application that creates a Window, enters its main loop and draws a scene before rendering a simple menu:

![Hello](demo.gif)

* [Hello.c](Examples/Hello.c) - The application source code
* [HelloViewController.h](Examples/HelloViewController.h) - The `HelloViewController` header.
* [HelloViewController.c](Examples/HelloViewController.c) - The `HelloViewController` source code. 

### Quetoo
[Quetoo](https://github.com/jdolan/quetoo) is an open source FPS based on [idTech2](https://en.wikipedia.org/wiki/Quake_II_engine). Quetoo uses ObjectivelyMVC for its in-game user interface:

![Quetoo](quetoo1.jpg)
![Quetoo](quetoo2.jpg)
![Quetoo](quetoo3.jpg)


Installation
---

See [INSTALL.md](INSTALL.md) for installation and linking instructions.

API documentation
---

Browse the [Class hierarchy](hierarchy.html) to grok the API.
