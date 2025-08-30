#pragma once
#ifndef _IMGUIX_CORE_THEME_HPP_INCLUDED
#define _IMGUIX_CORE_THEME_HPP_INCLUDED

/// \file Theme.hpp
/// \brief Theme definitions and helpers.

#include <imgui.h>
#ifdef IMGUI_ENABLE_IMPLOT
#   include <implot.h>
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
#   include <implot3d.h>
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

#       ifdef IMGUI_ENABLE_IMPLOT3D
        /// \brief Apply theme to ImPlot3D style.
        /// \param style Style to modify.
        virtual void apply(ImPlot3DStyle& style) const = 0;
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

#   ifdef IMGUI_ENABLE_IMPLOT
    /// \brief Set baseline ImPlot style parameters.
    /// \param style Style to modify.
    /// \details Used by all themes before applying color scheme.
    inline void applyDefaultImPlotStyle(ImPlotStyle& style) {
        using namespace ImGuiX::Config;

        style.UseISO8601     = true;
        style.Use24HourClock = true;

        style.PlotBorderSize = FRAME_BORDER_SIZE;
        style.PlotPadding    = WINDOW_PADDING;
        style.LabelPadding   = ITEM_SPACING;

        style.LegendPadding        = WINDOW_PADDING;
        style.LegendInnerPadding   = WINDOW_PADDING;
        style.LegendSpacing        = ITEM_SPACING;
        style.MousePosPadding      = WINDOW_PADDING;
        style.AnnotationPadding    = ITEM_SPACING;
    }
#   endif

#   ifdef IMGUI_ENABLE_IMPLOT3D
    /// \brief Set baseline ImPlot3D style parameters.
    /// \param style Style to modify.
    /// \details Used by all themes before applying color scheme.
    inline void applyDefaultImPlot3DStyle(ImPlot3DStyle& style) {
        using namespace ImGuiX::Config;

        style.PlotPadding    = WINDOW_PADDING;
        style.LabelPadding   = ITEM_SPACING;

        style.LegendPadding      = WINDOW_PADDING;
        style.LegendInnerPadding = WINDOW_PADDING;
        style.LegendSpacing      = ITEM_SPACING;
    }
#   endif
    
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
            applyDefaultImPlotStyle(style);
        }
#       endif

#       ifdef IMGUI_ENABLE_IMPLOT3D
        /// \copydoc Theme::apply
        void apply(ImPlot3DStyle& style) const override {
            ImPlot3D::StyleColorsClassic(&style);
            applyDefaultImPlot3DStyle(style);
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
            applyDefaultImPlotStyle(style);
        }
#       endif

#       ifdef IMGUI_ENABLE_IMPLOT3D
        /// \copydoc Theme::apply
        void apply(ImPlot3DStyle& style) const override {
            ImPlot3D::StyleColorsLight(&style);
            applyDefaultImPlot3DStyle(style);
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
            applyDefaultImPlotStyle(style);
        }
#       endif

#       ifdef IMGUI_ENABLE_IMPLOT3D
        /// \copydoc Theme::apply
        void apply(ImPlot3DStyle& style) const override {
            ImPlot3D::StyleColorsDark(&style);
            applyDefaultImPlot3DStyle(style);
        }
#       endif
    };

} // namespace ImGuiX::Themes

#endif // _IMGUIX_CORE_THEME_HPP_INCLUDED
