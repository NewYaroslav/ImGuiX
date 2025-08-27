#pragma once
#ifndef _IMGUIX_THEMES_THEMES_HPP_INCLUDED
#define _IMGUIX_THEMES_THEMES_HPP_INCLUDED

/// \file themes.hpp
/// \brief Public header aggregating theme facilities.
///
/// Example usage:
/// \code
/// struct MyTheme : ImGuiX::Themes::Theme {
///     void apply(ImGuiStyle& style) const override {
///         style.FrameRounding = 0.0f;
///     }
/// };
///
/// class MyController : public ImGuiX::Controller {
/// public:
///     using Controller::Controller;
///
///     void onInit() {
///         auto& tm = getThemeManager();
///         tm.registerTheme("custom", std::make_unique<MyTheme>());
///         tm.setTheme("custom");
///     }
/// };
/// \endcode
///
/// A controller can register a custom theme and switch to it.

#include <memory>

#include "Theme.hpp"
#include "ThemeManager.hpp"

namespace ImGuiX::Themes {

    inline void registerLightTheme(ThemeManager& mgr) {
        mgr.registerTheme("light", std::make_unique<LightTheme>());
    }

    inline void registerDarkTheme(ThemeManager& mgr) {
        mgr.registerTheme("dark", std::make_unique<DarkTheme>());
    }

    inline void setLightTheme(ThemeManager& mgr) {
        mgr.setTheme("light");
    }

    inline void setDarkTheme(ThemeManager& mgr) {
        mgr.setTheme("dark");
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_THEMES_HPP_INCLUDED
