# Theme Guide

For the Russian version, see [THEMES-RU.md](THEMES-RU.md).

This document lists the color themes bundled with ImGuiX and shows how to create and use your own themes.

## Available themes

### Built-in

- **Classic** – ImGui classic colors with ImGuiX spacing and rounding tweaks.
- **Light** – Bright palette suitable for light applications.
- **Dark** – Dark palette similar to ImGui's default dark mode.

### Extra themes

- **Corporate Grey** – Restrained grey palette for business interfaces.
- **Dark Charcoal** – Dark charcoal tones with refined spacing.
- **Dark Graphite** – Graphite-inspired dark scheme.
- **Dark Teal** – Dark base accented with teal highlights.
- **Deep Dark** – Nearly black theme for high-contrast setups.
- **Gold & Black** – Black background with gold accents.
- **Green & Blue** – Dark background with green/blue accents.
- **Light Blue** – Light background with subtle blue accents.
- **Light Green** – Light background with subtle green accents.
- **OSX Light** – macOS-like light appearance.
- **Pearl Light** – Soft light theme with blue highlights.
- **Slate Dark** – Dense slate grey theme with blue accents.
- **Visual Studio Dark** – Theme inspired by Visual Studio.
- **Y2K** – Light chrome/silver surfaces with cobalt/purple accents and glassy panels (early-2000s tech vibe).
- **Cyber-Y2K** – Dark tech base with neon cyan/magenta (plus violet, acid-green highlights); high-contrast with subtle glassy panels.
- **Cyberpunk UI** – HUD-style blue monochrome with neon cyan accents.
- **Night Owl** – Deep navy background with cyan/blue primaries.
- **Nord** – Calm dark scheme based on the Nord palette.
- **Tokyo Night** – Dark theme with stormy navy base and blue/cyan highlights.
- **Tokyo Night Storm** – Storm variant of Tokyo Night with the same accent set.

## Creating a custom theme

Implement the `ImGuiX::Themes::Theme` interface and override the `apply` methods. Use `ApplyDefaultImGuiStyle` to set common spacing and rounding values.
Do not call `ImGui::GetColorU32` outside an active ImGui frame.
Store raw `ImVec4` or `ImU32` values in configuration structs and convert during rendering.

```cpp
class MyTheme final : public ImGuiX::Themes::Theme {
public:
    void apply(ImGuiStyle& style) const override {
        ImGuiX::Themes::ApplyDefaultImGuiStyle(style);
        style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
        // set other colors...
        style.Colors[ImGuiCol_InputTextCursor] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // cursor color
    }
};
```

Register the theme with a `ThemeManager`:

```cpp
ImGuiX::Themes::ThemeManager tm;
tm.registerTheme("my-theme", std::make_unique<MyTheme>());
tm.setTheme("my-theme");
// call every frame to apply when changed
tm.updateCurrentTheme();
```

All built-in themes define the full set of `ImGuiCol_*` colors.

## JSON-backed overrides

Use `JsonBackedTheme` when you want a built-in theme to stay the baseline while colors and style values live in a text JSON file.
This helper is intentionally not included from `imguix/themes.hpp`; include `imguix/themes/JsonBackedTheme.hpp` explicitly.

```cpp
#include <imguix/themes/CorporateGreyTheme.hpp>
#include <imguix/themes/JsonBackedTheme.hpp>

auto& tm = themeManager();
ImGuiX::Themes::registerJsonBackedTheme<ImGuiX::Themes::CorporateGreyTheme>(
    tm,
    "corporate-grey",
    "data/themes/corporate-grey.json");
```

Behavior:

- Missing file: ImGuiX materializes the wrapped base theme and writes a full JSON snapshot.
- Existing file: only recognized keys are applied.
- Unknown keys, unsupported sections, missing sections, and invalid entries are ignored.
- JSON comments are supported because the file is preprocessed with `strip_json_comments(...)`.
- Generated JSON keeps `ImVec2` / `ImVec4` arrays on one line and rounds floats to readable values such as `0.62`.

In `mgc-platform`, the canonical runtime root is `data/` next to each executable, so theme JSON belongs under `data/themes/`.

File layout:

```json
{
  "meta": {
    "schemaVersion": 1
  },
  "imgui": {
    "colors": {
      "ImGuiCol_Text": [1.0, 1.0, 1.0, 1.0]
    },
    "style": {
      "WindowPadding": [8.0, 8.0],
      "WindowRounding": 6.0
    }
  },
  "implot": {
    "colors": {
      "ImPlotCol_AxisTick": [0.45, 0.45, 0.45, 0.70]
    },
    "style": {
      "PlotPadding": [10.0, 10.0]
    }
  },
  "implot3d": {
    "colors": {
      "ImPlot3DCol_FrameBg": [0.42, 0.42, 0.42, 0.54]
    },
    "style": {
      "PlotPadding": [10.0, 10.0]
    }
  }
}
```

Rules:

- Color keys use full enum names: `ImGuiCol_*`, `ImPlotCol_*`, `ImPlot3DCol_*`.
- Non-color style keys match the public field names exactly, such as `WindowPadding`, `FrameRounding`, or `LegendSpacing`.
- `ImVec2` and `ImVec4` values are stored as arrays.
- Scalar float/int/bool values stay scalar.
- Enum and flag fields are serialized as integers.
- `implot` and `implot3d` sections are optional and only matter when the corresponding support is compiled in.

## Custom Theme Values

Use the `custom` root section when a theme needs UI-specific values that do not map to built-in `ImGuiCol_*` colors or public style fields.
This is useful for colors and tuning parameters that belong to your own widgets or screens.
`ImGuiX` exposes `ThemeValueSource`, `ResolvedThemeValue<T>`, `tryExtractThemeValue(...)`,
`resolveCurrentThemeCustomColor(...)`, and `resolveCurrentThemeCustomValue(...)` in
`imguix/core/themes/theme_value_utils.hpp`.

- `custom.colors` is for custom `ImVec4` colors only.
- `custom.values` is for custom `float`, `int`, `bool`, `ImVec2`, and `ImVec4` values.
- `by_name` stores human-readable keys such as `SidebarAccountOnline`.
- `by_id` stores numeric fallback keys as decimal strings such as `"1001"`.
- Combined lookup checks the string key first and only falls back to the numeric id when the string key is absent.
- Invalid array sizes, unsupported JSON types, malformed numeric ids, and missing keys are ignored without failing theme loading.
- Existing valid theme JSON is not reformatted automatically.

Example `custom` section:

```json
"custom": {
    "colors": {
        "by_name": {
            "SidebarAccountOnline": [0.35, 0.74, 0.39, 1.0]
        },
        "by_id": {
            "1001": [0.95, 0.71, 0.22, 1.0]
        }
    },
    "values": {
        "by_name": {
            "BrokerBadgeRounding": 6.0,
            "UseCompactHeader": true,
            "WidgetPadding": [10.0, 6.0],
            "WidgetCornerVector": [2.0, 2.0, 6.0, 6.0]
        },
        "by_id": {
            "2001": 12.0,
            "2002": false,
            "2003": [16.0, 10.0]
        }
    }
}
```

Example lookups:

```cpp
const auto color = ImGuiX::Themes::resolveCurrentThemeCustomColor(
    themeManager(),
    "SidebarAccountOnline",
    1001,
    ImVec4(0.40f, 0.40f, 0.40f, 1.0f));

const auto rounding = ImGuiX::Themes::resolveCurrentThemeCustomValue(
    themeManager(),
    "BrokerBadgeRounding",
    2001,
    4.0f);

const auto compact_header = ImGuiX::Themes::resolveCurrentThemeCustomValue(
    themeManager(),
    "UseCompactHeader",
    false);

const auto padding = ImGuiX::Themes::resolveCurrentThemeCustomValue(
    themeManager(),
    2003,
    ImVec2(8.0f, 4.0f));
```

Notes:

- `custom.colors.*` ignores anything except 4-component arrays.
- `custom.values.*` ignores unsupported JSON types.
- `resolveCurrentThemeCustomColor(...)` resolves only `custom.colors`.
- `resolveCurrentThemeCustomValue(...)` resolves only `custom.values`.
- `float` accepts `float` and `int`; `int` accepts only `int`.
- Type mismatch behaves like a missing key and keeps the caller fallback.
- Combined lookup prefers a valid string-key value over numeric fallback.
- An invalid string-key type does not block numeric fallback.
- Use `.value` to consume the resolved value and `.source` to inspect whether it came from the string key, numeric id, or fallback.
- See `examples/smoke/theme_custom_values_demo.cpp` for a runnable example.

## Applying themes

### Via `ThemeManager`

```cpp
auto& tm = themeManager();
ImGuiX::Themes::registerCorporateGreyTheme(tm); // or other register* helpers
tm.setTheme(IMGUIX_THEME_CORPORATE_GREY);
tm.updateCurrentTheme();
```

### Via widgets

```cpp
auto& tm = themeManager();
ImGuiX::Themes::registerCorporateGreyTheme(tm);
ImGuiX::Widgets::ApplyStoredTheme(this);

if (ImGui::CollapsingHeader("Theme", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGuiX::Widgets::ThemePicker("demo.theme", this);
}
```

`ThemePicker` displays a combo box to choose from registered themes, and `ApplyStoredTheme` restores the last selected theme using the `IMGUIX_THEME_STORAGE_KEY` setting.
