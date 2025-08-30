# Configuration and Macros

For the Russian version, see [CONFIGURATION-RU.md](CONFIGURATION-RU.md).

ImGuiX exposes several compile-time switches and configuration macros. This document summarizes them and where they come from.

## Compile-Time Macros

These macros enable optional integrations and are defined automatically when matching CMake options are set.

- `IMGUI_ENABLE_FREETYPE` — use FreeType for font rasterization (`IMGUIX_IMGUI_FREETYPE`).
- `IMGUI_ENABLE_IMPLOT` — integrate [ImPlot](https://github.com/epezent/implot) (`IMGUIX_USE_IMPLOT`).
- `IMGUI_ENABLE_IMPLOT3D` — integrate [ImPlot3D](https://github.com/jimgries/implot3d) (`IMGUIX_USE_IMPLOT3D`).

## Configuration Headers

`include/imguix/config/` contains headers that define default paths, colors, icons and other settings. Include `<imguix/config.hpp>` to pull them all in. Values can be overridden by redefining macros before inclusion or via compiler options.

### Paths

- `IMGUIX_CONFIG_DIR` — directory for configuration files. Default `data/config`.
- `IMGUIX_OPTIONS_FILENAME` — options storage filename. Default `options.json`.

### Fonts

- `IMGUIX_FONTS_DIR` — default fonts directory.
- `IMGUIX_FONTS_CONFIG` — default fonts configuration file.
- `IMGUIX_FONTS_CONFIG_BASENAME` — basename of the fonts config.
- `IMGUIX_FONTS_FALLBACK_BODY_BASENAME` — fallback body font.
- `IMGUIX_FONTS_FALLBACK_ICONS_BASENAME` — fallback icon font.

### Internationalization

- `IMGUIX_I18N_DIR` — directory with localization resources.
- `IMGUIX_I18N_JSON_BASENAME` — basename of the localization file.
- `IMGUIX_I18N_PLURALS_FILENAME` — plural rules filename.
- `IMGUIX_RESOLVE_PATHS_REL_TO_EXE` — resolve resource paths relative to the executable when nonzero.

### Options

- `IMGUIX_OPTIONS_SAVE_DELAY_SEC` — delay before saving options in seconds.

### Sizing Templates

- `IMGUIX_SIZING_TIME_SIGNED` — sample signed time string.
- `IMGUIX_SIZING_TIME_UNSIGNED` — sample unsigned time string.
- `IMGUIX_SIZING_HMS` — sample hours/minutes/seconds string.
- `IMGUIX_SIZING_DATE` — sample date string.
- `IMGUIX_SIZING_YEAR_SIGNED` — sample signed year string.
- `IMGUIX_SIZING_YEAR_UNSIGNED` — sample unsigned year string.
- `IMGUIX_SIZING_WEEKDAYS` — comma-separated weekday names.

### Colors

- `IMGUIX_COLOR_ERROR` — error color.
- `IMGUIX_COLOR_WARNING` — warning color.
- `IMGUIX_COLOR_INFO` — info color.
- `IMGUIX_COLOR_SUCCESS` — success color.
- `IMGUIX_COLOR_OHLC_BULL` — bullish OHLC color.
- `IMGUIX_COLOR_OHLC_BEAR` — bearish OHLC color.

### Icons

- `IMGUIX_ICON_KEYBOARD`, `IMGUIX_ICON_HELP`, `IMGUIX_ICON_WARNING`, `IMGUIX_ICON_INFO`, `IMGUIX_ICON_SUCCESS`.
- `IMGUIX_ICON_ADD`, `IMGUIX_ICON_REMOVE`, `IMGUIX_ICON_EYE_SHOW`, `IMGUIX_ICON_EYE_HIDE`, `IMGUIX_ICON_COPY`, `IMGUIX_ICON_PASTE`.

### Notifications

- `IMGUIX_NOTIFY_ICON_SUCCESS`, `IMGUIX_NOTIFY_ICON_WARNING`, `IMGUIX_NOTIFY_ICON_ERROR`, `IMGUIX_NOTIFY_ICON_INFO`, `IMGUIX_NOTIFY_ICON_CLOSE`.
- `IMGUIX_NOTIFY_COLOR_SUCCESS`, `IMGUIX_NOTIFY_COLOR_WARNING`, `IMGUIX_NOTIFY_COLOR_ERROR`, `IMGUIX_NOTIFY_COLOR_INFO`, `IMGUIX_NOTIFY_COLOR_DEFAULT`.

### Theme Constants

`config/theme_config.hpp` provides `ImGuiX::Config` constexpr values for paddings, roundings and similar UI metrics.

