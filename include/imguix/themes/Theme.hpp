#pragma once
#ifndef _IMGUIX_THEMES_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_THEME_HPP_INCLUDED

/// \file Theme.hpp
/// \brief Theme definitions and helpers.

#include <imgui.h>

namespace ImGuiX::Themes {

    class Theme {
    public:
        virtual ~Theme() = default;
        virtual void apply(ImGuiStyle& style) const = 0;
    };

    class LightTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            ImGui::StyleColorsLight(&style);
        }
    };

    class DarkTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            ImGui::StyleColorsDark(&style);
        }
    };

    inline void applyLightTheme(ImGuiStyle& style) {
        LightTheme{}.apply(style);
    }

    inline void applyDarkTheme(ImGuiStyle& style) {
        DarkTheme{}.apply(style);
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_THEME_HPP_INCLUDED
