ObjectivelyMVC
==============
Object oriented MVC framework for SDL3 and C.

[View on GitHub](https://github.com/jdolan/ObjectivelyMVC) — Zlib license.

## About

[ObjectivelyMVC](https://github.com/jdolan/ObjectivelyMVC) is a cross-platform user interface and interaction framework for games built with [SDL3](https://libsdl.org) and [GNU C](https://www.gnu.org/software/gnu-c-manual/). It renders via SDL3's GPU API (Metal, Vulkan, Direct3D 12) through [ObjectivelyGPU](https://github.com/jdolan/ObjectivelyGPU), and is built on [Objectively](https://github.com/jdolan/Objectively). It is the user interface framework used in [Quetoo](https://github.com/jdolan/quetoo).

## Features

- **Does not hijack your main loop** — call `respondToEvent` and `render` once per frame
- **Complete widget set**: Button, Checkbox, Slider, Select, TextView, TableView, CollectionView, TabView, PageView, and more
- **Programmatic or JSON-driven layouts** for declarative, data-driven UI
- **Fully themable** via a CSS-inspired Selector / Style / Stylesheet system
- **High-DPI / 4K ready** — automatic Retina detection with SDL_ttf TrueType font rendering
- **iOS and macOS** (xcframework), **Windows** (MSVC / ClangCL), and **Linux**

## Class Hierarchy

Browse the [class hierarchy](hierarchy.html) to navigate the full API.

## Getting Started

```sh
autoreconf -i
./configure
make && sudo make install
```

See the @subpage guide for a full tour of fonts, views and controls, JSON layout, theming, and resource loading.

## Examples

![ObjectivelyMVC demo](demo.gif)
