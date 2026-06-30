ObjectivelyMVC Guide {#guide}
=====================

A tour of building user interfaces with ObjectivelyMVC.

[TOC]

## Object oriented Model-View-Controller in C

ObjectivelyMVC is built on [Objectively](https://github.com/jdolan/Objectively), a lightweight object oriented framework for C. ObjectivelyMVC delivers the elegance of OO / MVC without imposing C++ on your project. If you *are* using C++, ObjectivelyMVC is perfectly happy to coexist with it.

```c
WindowController *windowController = $(alloc(WindowController), initWithDevice, renderDevice);
```

## Easily embeddable in any SDL3 application

ObjectivelyMVC is purpose-built for video games. Unlike Gtk+, Qt, wxWidgets, FLTK, ...ObjectivelyMVC **does not hijack the main loop**. ObjectivelyMVC does not create a window, claim the GPU device, or originate events. Your game already does that, because it has to. Like your mother, ObjectivelyMVC only asks that you give it a call once in a while. That's it.

```c
$(windowController, respondToEvent, &event);
...
$(windowController, render, commands, framebuffer);
```

## Beautiful 4K-ready fonts

ObjectivelyMVC uses [SDL_ttf](https://www.libsdl.org/projects/SDL_ttf/) to render TrueType fonts. It also automatically detects High-DPI (Retina, 4K) displays, and scales fonts accordingly. The result is crisp, beautiful vector-based fonts that look native, because they are.

```c
Data *data = $$(Data, dataWithContentsOfFile, "Verdana.ttf");
...
$$(Font, cacheFont, data, "Verdana");
...
Font *verdana = $$(Font, cachedFont, "Verdana", 24, FontStyleRegular); // will render at 48pt on Retina displays
```

## Full suite of Views and Controls

ObjectivelyMVC provides a robust set of containers, views and controls. Stack and arrange components with `Box`, `Panel` and `StackView`. Add `Buttons`, `Checkboxes`, `Selects`, `Sliders`, editable `TextViews` and more by simply instantiating them. Display tabular data or a thumbnail gallery with `TableView` and `CollectionView`. Split complex interfaces into multiple tabs with `TabView` and `TabViewController`. Use the specialized _delegate_ callbacks to respond to events.

```c
Select *select = $(alloc(Select), initWithFrame, NULL);
...
select->delegate.didSelectOption = my_callback;
```

## Programmatic or JSON-based layout options

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

## Fully themable via Cascading Stylesheets

ObjectivelyMVC uses a custom CSS3 dialect to manage all aspects of its presentation. Nothing about the look and feel of ObjectivelyMVC is hard-coded, and authoring and attaching new `Stylesheets` is trivial and straightforward.

```css
/* MyView.css */

Box {
	border-color: #1e4e62;
}

Panel {
	background-color: #08151aaa;
}

.columns {
	autoresizing-mask: contain | width;
	axis: horizontal;
	distribution: fill;
}

.columns > .column {
	autoresizing-mask: contain | height;
	distribution: fill;
	width: 384;
}
```

## Simple, flexible resource loading

ObjectivelyMVC makes loading `Fonts`, `Images`, `Stylesheets`, and `Views` painless. For direct filesystem access, add directories to its search path with `$$(Resource, addResourcePath, "/my/path")`. To search your game's filesystem abstraction, implement a custom `ResourceProvider` function, and register it:

```c
static Data *resourceProvider(const char *name) {
	uint8_t *bytes;

	const ssize_t length = read_file_from_gamefs(name, &bytes);

	return length != -1 ? $$(Data, dataWithBytes, bytes, length) : NULL;
}

// ...

$$(Resource, addResourceProvider, resourceProvider);
```

## Example: Hello

The [Hello](https://github.com/jdolan/ObjectivelyMVC/blob/master/Examples/Hello.c) example creates a window, enters its main loop, and renders a themed menu over a 3D scene using JSON layout:

- [Hello.c](https://github.com/jdolan/ObjectivelyMVC/blob/master/Examples/Hello.c) — the application and main loop
- [HelloViewController.h](https://github.com/jdolan/ObjectivelyMVC/blob/master/Examples/HelloViewController.h) — the `HelloViewController` interface
- [HelloViewController.c](https://github.com/jdolan/ObjectivelyMVC/blob/master/Examples/HelloViewController.c) — the `HelloViewController` implementation
- [HelloViewController.json](https://github.com/jdolan/ObjectivelyMVC/blob/master/Examples/HelloViewController.json) — the JSON view hierarchy
