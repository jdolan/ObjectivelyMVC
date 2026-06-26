ObjectivelyMVC {#index}
===============

[ObjectivelyMVC](https://github.com/jdolan/ObjectivelyMVC) is a cross-platform MVC user interface framework for games built with [SDL3](https://libsdl.org) and [GNU C](https://www.gnu.org/software/gnu-c-manual/). It renders via SDL3's GPU API (Metal, Vulkan, Direct3D 12) through [ObjectivelyGPU](https://github.com/jdolan/ObjectivelyGPU), and is built on [Objectively](https://github.com/jdolan/Objectively). It is the UI framework used in [Quetoo](https://github.com/jdolan/quetoo).

## Features

- **Does not hijack your main loop** — call `respondToEvent` and `render` once per frame
- **Complete widget set**: Button, Checkbox, Slider, Select, TableView, CollectionView, TabView, PageView, and more
- **CSS-inspired styling** via Selector, Style, and Stylesheet
- **JSON-driven layouts** for declarative, data-driven UI
- **High-DPI / 4K ready** — automatic Retina detection with SDL_ttf TrueType font rendering
- **iOS and macOS** (xcframework), **Windows** (MSVC / ClangCL), **Linux**

## Screenshot

![ObjectivelyMVC demo](demo.gif)

## API

Browse the [class hierarchy](hierarchy.html) to navigate the API.

## Building

```sh
autoreconf -i
./configure
make && sudo make install
```
