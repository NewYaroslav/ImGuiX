#pragma once
#ifndef _IMGUIX_CORE_THEME_HPP_INCLUDED
#define _IMGUIX_CORE_THEME_HPP_INCLUDED

/// \file Theme.hpp
/// \brief Theme definitions and helpers.

#include <imgui.h>
#ifdef IMGUI_ENABLE_IMPLOT
#   include <implot.h>
#endif

#include <imguix/config/theme_config.hpp>

namespace ImGuiX::Themes {

    /// \brief Interface for style themes.
    class Theme {
    public:
        virtual ~Theme() = default;

        /// \brief Apply theme to ImGui style.
        /// \param style Style to modify.
        virtual void apply(ImGuiStyle& style) const = 0;

#       ifdef IMGUI_ENABLE_IMPLOT
        /// \brief Apply theme to ImPlot style.
        /// \param style Style to modify.
        virtual void apply(ImPlotStyle& style) const = 0;
#       endif

    };

    /// \brief Set baseline ImGui style parameters.
    /// \param style Style to modify.
    /// \details Used by all themes before applying color scheme.
    inline void applyDefaultImGuiStyle(ImGuiStyle& style) {
        using namespace ImGuiX::Config;

        style.ChildRounding     = CHILD_ROUNDING;
        style.FrameBorderSize   = FRAME_BORDER_SIZE;
        style.FrameRounding     = FRAME_ROUNDING;
        style.GrabMinSize       = GRAB_MIN_SIZE;
        style.PopupRounding     = POPUP_ROUNDING;
        style.ScrollbarRounding = SCROLLBAR_ROUNDING;
        style.TabBorderSize     = TAB_BORDER_SIZE;
        style.TabRounding       = TAB_ROUNDING;
        style.WindowRounding    = WINDOW_ROUNDING;

        style.WindowPadding     = WINDOW_PADDING;
        style.ItemSpacing       = ITEM_SPACING;
        style.ScrollbarSize     = SCROLLBAR_SIZE;
        style.WindowBorderSize  = WINDOW_BORDER_SIZE;
        style.ChildBorderSize   = CHILD_BORDER_SIZE;
        style.PopupBorderSize   = POPUP_BORDER_SIZE;
        style.GrabRounding      = GRAB_ROUNDING;
    }
    
    /// \brief Classic ImGui theme.
    class ClassicTheme final : public Theme {
    public:
        /// \copydoc Theme::apply
        void apply(ImGuiStyle& style) const override {
            ImGui::StyleColorsClassic(&style);
            applyDefaultImGuiStyle(style);
        }

#       ifdef IMGUI_ENABLE_IMPLOT
        /// \copydoc Theme::apply
        void apply(ImPlotStyle& style) const override {
            ImPlot::StyleColorsClassic(&style);
        }
#       endif
    };

    /// \brief Light color theme.
    class LightTheme final : public Theme {
    public:
        /// \copydoc Theme::apply
        void apply(ImGuiStyle& style) const override {
            ImGui::StyleColorsLight(&style);
            applyDefaultImGuiStyle(style);
        }

#       ifdef IMGUI_ENABLE_IMPLOT
        /// \copydoc Theme::apply
        void apply(ImPlotStyle& style) const override {
            ImPlot::StyleColorsLight(&style);
        }
#       endif
    };

    /// \brief Dark color theme.
    class DarkTheme final : public Theme {
    public:

        /// \copydoc Theme::apply
        void apply(ImGuiStyle& style) const override {
            ImGui::StyleColorsDark(&style);
            applyDefaultImGuiStyle(style);
        }

#       ifdef IMGUI_ENABLE_IMPLOT
        /// \copydoc Theme::apply
        void apply(ImPlotStyle& style) const override {
            ImPlot::StyleColorsDark(&style);
        }
#       endif
    };

} // namespace ImGuiX::Themes

#endif // _IMGUIX_CORE_THEME_HPP_INCLUDED
