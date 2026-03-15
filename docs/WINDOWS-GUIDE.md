# Windows Guide

For the Russian version, see [WINDOWS-GUIDE-RU.md](WINDOWS-GUIDE-RU.md).

This guide explains how windowing works in ImGuiX today, with a practical focus on:

- `include/imguix/core/window/WindowInstance.hpp`
- `include/imguix/core/window/WindowInterface.hpp`
- `include/imguix/windows/ImGuiFramedWindow.hpp`

It is intended for developers who are new to ImGuiX and need to configure, style, and extend windows without reading all source files first.

## 1. Which base class to use

| Use case | Recommended base class | Why |
| --- | --- | --- |
| You need full custom render loop and custom chrome (or no chrome helpers) | `WindowInstance` | Lowest-level window abstraction with full lifecycle control. |
| You want a ready custom frame: title bar, optional side panel, optional corner icon area, system buttons | `Windows::ImGuiFramedWindow` | Built on top of `WindowInstance`, adds window chrome/layout helpers and style flags. |

Rule of thumb:

- Start with `ImGuiFramedWindow` for operator apps and tools.
- Drop to `WindowInstance` only when framed layout rules become a constraint.

## 2. Runtime lifecycle and frame pipeline

Two levels are involved:

1. `WindowManager` orchestrates all windows.
2. Each `WindowInstance` processes its own frame.

### WindowManager lifecycle

Source: `include/imguix/core/window/WindowManager.ipp`.

- `prepareFrame()`:
  - `flushPending()`
  - `initializePending()` -> `window->fontsStartInit()`, `window->onInit()`, `window->buildFonts()`
  - `removeClosed()`
  - `initializeControllers()`
- `processFrame()` order:
  - `handleEvents()`
  - `tickAll()`
  - `drawContentAll()`
  - `drawUiAll()`
  - `presentAll()`
  - `loadIniAll()`
  - `saveIniAll()`

### Per-window frame contract

Base hooks in `WindowInstance`:

- `handleEvents()`
- `tick()`
- `drawContent()`
- `drawUi()`
- `present()`

`ImGuiFramedWindow` overrides `drawUi()` (backend-specific `.ipp` files) and internally dispatches into:

- `drawClassicLayout(...)` when `WindowFlags::HasCornerIconArea` is not set
- `drawCornerLayout(...)` when `WindowFlags::HasCornerIconArea` is set

## 3. WindowInterface and WindowInstance API map

### 3.1 Geometry and state

Core API (via `WindowInterface` / `WindowInstance`):

- Identity: `id()`, `name()`
- Size: `width()`, `height()`, `setSize(int, int)`
- Visibility and activation: `setVisible(bool)`, `setActive(bool)`, `isActive()`
- Open/close: `isOpen()`, `close()`
- Window state: `minimize()`, `maximize()`, `restore()`, `isMaximized()`, `toggleMaximizeRestore()`

### 3.2 Theme, options, bus, resources

- Theme: `setTheme(std::string)`, `themeManager()`
- Options store: `options()` (control/view)
- Event bus: `eventBus()`
- Resources: `registry()`
- Notifications: `notifications()`

### 3.3 Fonts and i18n

- Font lookup: `getFont(FontRole)`
- Font manager access: `fontsView()`, `fontsControl()`
- Language: `langStore()`, `requestLanguageChange(...)`, `applyPendingLanguageChange()`
- Manual font build hooks (init-only):
  - `fontsBeginManual()`
  - `fontsSetLocale(...)`
  - `fontsSetRangesPreset(...)`
  - `fontsSetRangesExplicit(...)`
  - `fontsClearRanges()`
  - `fontsAddBody(...)`
  - `fontsAddHeadline(...)`
  - `fontsAddMerge(...)`
  - `fontsBuildNow()`

### 3.4 ImGui ini persistence

- `iniPath()`
- `initIni()`
- `loadIni()`
- `saveIniNow()`

## 4. ImGuiFramedWindow model

`ImGuiFramedWindow` is a framed host over `WindowInstance`.

Main customization hooks:

- `drawTitleBarText()`
- `drawSidePanel()`
- `drawCornerIcon()`
- `drawMenuBar()`

Layout modes:

1. Classic mode:
- active when `WindowFlags::HasCornerIconArea` is not set.
- title row always exists.
- side panel exists only if `side_panel_width > 0`.

2. Corner mode:
- active when `WindowFlags::HasCornerIconArea` is set.
- top-left icon area is reserved.
- side width:
  - if `side_panel_width > 0`, explicit width
  - if `side_panel_width <= 0`, auto width based on `corner_icon_mode_area_width` or fallback formula.

## 5. WindowFlags matrix

Source: `include/imguix/windows/window_flags.hpp`.

| Flag | Effect | Notes |
| --- | --- | --- |
| `HasMenuBar` | Enables menu bar region and `drawMenuBar()` calls | In corner mode, placement is controlled by `corner_menu_bar_placement`. |
| `EnableTransparency` | Requests transparent native clear path | Backend-dependent behavior; see section 7. |
| `DisableBackground` | Makes root ImGui host background transparent | Useful with custom frame strokes still visible. |
| `ShowControlButtons` | Shows minimize/maximize/close | Position/size depends on control button style and frame config. |
| `MacStyledControlButtons` | Circular mac-like buttons | Mutually exclusive with other style flags. |
| `ImGuiStyledControlButtons` | `Widgets::SystemButton` style | Default when conflicts occur in release fallback. |
| `ClassicStyledControlButtons` | Text button style (`Button`) | Mutually exclusive with other style flags. |
| `HasCornerIconArea` | Enables corner layout with icon slot | Switches from classic to corner layout. |
| `CornerModeRounding` | Enables rounded title/side geometry in corner mode | Radius from `corner_icon_mode_rounding_radius`. |
| `CornerModeBorder` | Enables title/side border strokes in corner mode | Uses `frame_stroke_thickness`. |
| `DefaultControlButtons` | `ShowControlButtons | ImGuiStyledControlButtons` | Convenience alias. |

Control button style conflict policy (implemented in `resolveControlButtonsStyle()`):

- Debug builds: assert if multiple style flags are set.
- Release builds: fallback to ImGui style.

## 6. ImGuiFramedWindowConfig matrix

Source: `include/imguix/windows/ImGuiFramedWindow.hpp`.

### 6.1 Geometry and frame

| Field | Meaning | Default / auto semantics |
| --- | --- | --- |
| `min_width`, `min_height` | Minimum client size (Win32 hit-test path uses these) | Defaults `640x480` |
| `frame_corner_radius` | Native rounded region radius and root host rounding source | Default `8` |
| `resize_border` | Manual resize hit-test thickness (Win32 borderless window) | Default `8` |
| `title_bar_height` | Title row height | Default `32` |
| `side_panel_width` | Classic: `<=0` disables side panel; Corner: `<=0` means auto width | Default `0` |
| `frame_stroke_thickness` | Title/side separator stroke thickness | Default `1.0f` |
| `frame_outer_stroke_thickness` | Outer host frame stroke thickness | Default `2.0f` |
| `frame_inner_stroke_thickness` | Inner host frame stroke thickness | Default `1.0f` |

### 6.2 Content insets

| Field | Meaning | Auto rule |
| --- | --- | --- |
| `title_content_left_inset` | Baseline left inset for title content | `<0`: style-based auto inset |
| `side_panel_content_left_inset` | Baseline left inset for side panel content | `<0`: style-based auto inset |

### 6.3 Corner mode options

| Field | Meaning | Notes |
| --- | --- | --- |
| `corner_icon_mode_rounding_radius` | Radius for corner mode title/side rounding | Used only when `CornerModeRounding` is enabled |
| `corner_rounding_style` | Corner rounding policy (`Legacy`, `NoTopLeftOnTitleAndSide`) | Effective only with `CornerModeRounding` |
| `corner_icon_mode_area_width` | Reserved width for corner icon area | `<0`: fallback formula |
| `corner_icon_mode_icon_size` | Icon content size in corner slot | `<0`: auto-fit with frame-aware compensation |
| `corner_icon_mode_gap` | Gap between icon area and title/side | `<0`: runtime `style.WindowPadding.x` |
| `corner_menu_bar_placement` | `MainRegion`, `InTitleBar`, `BelowTitleBar` | Works only if `HasMenuBar` is set |

### 6.4 Buttons and clear color

- `close_button_text`, `minimize_button_text`, `maximize_button_text`: label + ImGui ID suffix for classic text buttons.
- `clear_color`: backend clear color when transparency is not active.

## 7. Transparency and background behavior

Three independent controls are often confused:

1. `WindowFlags::EnableTransparency`
- Affects backend clear/native surface path.
- SFML Win32 path also applies DWM setup in `SfmlImGuiFramedWindow.ipp`.

2. `WindowFlags::DisableBackground`
- Makes root ImGui host background transparent while still rendering frame/border paths.

3. `setDisableBackground(bool)`
- Runtime toggle equivalent to background-disable behavior.

Practical pattern:

- Start with `DisableBackground` for visual experimentation.
- Add `EnableTransparency` only when backend/native transparency is required.

## 8. Backend differences you must account for

### SFML backend (`SfmlImGuiFramedWindow.ipp`)

- Win32 path uses:
  - borderless native window + custom hit testing
  - `SetWindowRgn(CreateRoundRectRgn(...))` for rounded native region
  - DWM extension when transparency is enabled
- `setWindowIcon(...)` in `ImGuiFramedWindow` loads both native icon and corner icon texture on SFML.

Important consequence:

- Pixels outside `SetWindowRgn` do not exist. Border strokes must be visually aligned inside region-aware geometry.

### GLFW / SDL2 framed backends

- No Win32 rounded-region shaping in current framed backend code.
- `EnableTransparency` affects OpenGL clear color path, but not SFML-specific native region logic.
- `ImGuiFramedWindow::setWindowIcon(...)` returns `false` outside SFML in current implementation.

## 9. Practical recipes

### 9.1 Classic layout: title + menu + side panel

```cpp
using namespace ImGuiX::Windows;

ImGuiFramedWindowConfig cfg{};
cfg.title_bar_height = 40;
cfg.side_panel_width = 48;

WindowFlags flags =
    WindowFlags::HasMenuBar |
    WindowFlags::DefaultControlButtons;
```

Override:

- `drawTitleBarText()`
- `drawMenuBar()`
- `drawSidePanel()`

### 9.2 Corner V3 style: icon + side panel, no menu bar

```cpp
using namespace ImGuiX::Windows;

ImGuiFramedWindowConfig cfg{};
cfg.title_bar_height = 40;
cfg.side_panel_width = 0; // corner auto-width
cfg.corner_menu_bar_placement = CornerMenuBarPlacement::MainRegion;

WindowFlags flags =
    WindowFlags::DefaultControlButtons |
    WindowFlags::HasCornerIconArea |
    WindowFlags::CornerModeRounding |
    WindowFlags::CornerModeBorder;
```

Call `setWindowIcon("...")` in `onInit()` for SFML corner icon rendering.

### 9.3 Corner style without top-left rounding on title/side

```cpp
cfg.corner_rounding_style = CornerRoundingStyle::NoTopLeftOnTitleAndSide;
```

Keep `WindowFlags::CornerModeRounding` enabled; otherwise this style does nothing.

### 9.4 Mac-style control buttons

```cpp
WindowFlags flags =
    WindowFlags::ShowControlButtons |
    WindowFlags::MacStyledControlButtons |
    WindowFlags::HasCornerIconArea |
    WindowFlags::CornerModeRounding |
    WindowFlags::CornerModeBorder;
```

Do not combine multiple style flags (`MacStyledControlButtons`, `ImGuiStyledControlButtons`, `ClassicStyledControlButtons`).

## 10. Smoke demo map

Source: `examples/smoke/`.

| Demo | What it demonstrates |
| --- | --- |
| `corner_icon_area_off_demo` | Classic layout, menu + side panel, no corner icon mode |
| `corner_icon_area_off_no_side_demo` | Classic layout, menu, no side panel |
| `corner_icon_area_off_no_menu_demo` | Classic layout with title only (no menu row) |
| `corner_icon_area_off_no_menu_odd_title_demo` | Classic layout with odd title height alignment checks |
| `corner_icon_area_demo` | Corner layout + menu in title bar |
| `corner_icon_area_demo_v2` | Corner layout + menu below title bar |
| `corner_icon_area_demo_v3` | Corner layout, no menu bar, V3 baseline |
| `corner_icon_area_demo_v3_no_top_left` | Corner layout with `NoTopLeftOnTitleAndSide` |
| `corner_icon_area_demo_v3_mac` | Corner layout with Mac control button style |

Build one smoke target from repo root:

```powershell
cmake --build external/ImGuiX/build-mingw --target corner_icon_area_demo_v3 --parallel 36
```

For full smoke build profile, see `agents/imguix-smoke-build.md`.

## 11. Common gotchas

1. Rounded region clipping on SFML/Win32
- With `SetWindowRgn`, outside pixels are clipped by OS region. If border appears "missing" at corners, verify stroke geometry is drawn inside effective region.

2. `setWindowIcon` behavior mismatch by backend
- In current `ImGuiFramedWindow`, meaningful success is SFML-only. Non-SFML returns `false`.

3. `side_panel_width` semantic changes by layout mode
- Classic: `<= 0` disables side panel.
- Corner: `<= 0` enables auto width.

4. `corner_menu_bar_placement` has no effect without `HasMenuBar`
- Placement is evaluated only when menu bar flag is active.

5. Control button style flag conflicts
- Multiple style flags trigger debug assert and release fallback.

## 12. Quick implementation checklist for new windows

1. Choose base class (`WindowInstance` or `ImGuiFramedWindow`).
2. Define `WindowFlags` first (layout mode and button style).
3. Tune `ImGuiFramedWindowConfig` geometry/strokes.
4. Implement only needed virtual hooks.
5. Verify on at least one corner demo and one classic demo scenario.
6. If using SFML/Win32 rounded corners, validate corner border continuity during resize.
