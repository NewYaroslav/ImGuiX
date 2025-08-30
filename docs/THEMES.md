# Theme Guide

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

## Creating a custom theme

Implement the `ImGuiX::Themes::Theme` interface and override the `apply` methods. Use `applyDefaultImGuiStyle` to set common spacing and rounding values.

```cpp
class MyTheme final : public ImGuiX::Themes::Theme {
public:
    void apply(ImGuiStyle& style) const override {
        ImGuiX::Themes::applyDefaultImGuiStyle(style);
        style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
        // set other colors...
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

