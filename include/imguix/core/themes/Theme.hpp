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
#       ifdef IMGUIX_HAS_DOCKING
        style.WindowRounding    = 0.0f;
#       endif
    }

#   ifdef IMGUI_ENABLE_IMPLOT
    /// \brief Set baseline ImPlot style parameters.
    /// \param style Style to modify.
    /// \details Used by all themes before applying color scheme.
    inline void applyDefaultImPlotStyle(ImPlotStyle& style) {
        using namespace ImGuiX::Config;
        const ImGuiStyle& ig = ImGui::GetStyle();

        style.UseISO8601     = true;
        style.Use24HourClock = true;

        style.PlotBorderSize = FRAME_BORDER_SIZE;
        style.PlotPadding    = WINDOW_PADDING;
        style.LabelPadding   = ig.ItemInnerSpacing;

        style.LegendPadding      = WINDOW_PADDING;
        style.LegendInnerPadding = ig.ItemInnerSpacing;
        style.LegendSpacing      = ITEM_SPACING;
        style.MousePosPadding    = ig.ItemInnerSpacing;
        style.AnnotationPadding  = ImVec2(
                ig.ItemInnerSpacing.x * ANNOTATION_PADDING_SCALE,
                ig.ItemInnerSpacing.y * ANNOTATION_PADDING_SCALE
        );

        // Grid and ticks: minor lines slightly thinner
        style.MinorAlpha   = PLOT_MINOR_ALPHA;
        const float px     = ig.FrameBorderSize > 0.0f ? ig.FrameBorderSize : FRAME_BORDER_SIZE;
        style.MajorTickLen = PLOT_MAJOR_TICK_LEN;
        style.MinorTickLen = PLOT_MINOR_TICK_LEN;
        style.MajorTickSize = ImVec2(px, px);
        style.MinorTickSize = ImVec2(px * PLOT_MINOR_SCALE, px * PLOT_MINOR_SCALE);
        style.MajorGridSize = ImVec2(px, px);
        style.MinorGridSize = ImVec2(px * PLOT_MINOR_SCALE, px * PLOT_MINOR_SCALE);

        // Plot sizes
        style.PlotDefaultSize = PLOT_DEFAULT_SIZE;
        style.PlotMinSize     = PLOT_MIN_SIZE;
    }
    
    // вызывайте каждый раз после смены темы ImGui
    inline void SyncImPlotWithImGui() {
        ImGuiStyle& s = ImGui::GetStyle();
        ImPlotStyle& p = ImPlot::GetStyle();

        const ImVec4 child = s.Colors[ImGuiCol_ChildBg];
        const ImVec4 win   = s.Colors[ImGuiCol_WindowBg];
        const ImVec4 frame = s.Colors[ImGuiCol_FrameBg];
        const ImVec4 text  = s.Colors[ImGuiCol_Text];
        const ImVec4 border= s.Colors[ImGuiCol_Border];

        // Главное — увести чёрный фон графика
        p.Colors[ImPlotCol_PlotBg]        = child; // внутренняя область
        p.Colors[ImPlotCol_FrameBg]       = frame; // рамка вокруг графика
        p.Colors[ImPlotCol_AxisBgHovered] = child;
        p.Colors[ImPlotCol_AxisBgActive]  = child;
        p.Colors[ImPlotCol_LegendBg]      = win;
        p.Colors[ImPlotCol_LegendBorder]  = border;

        // (опционально) сетка/оси в тон теме
        ImVec4 grid = ImVec4(text.x, text.y, text.z, 0.25f);
        p.Colors[ImPlotCol_AxisGrid] = grid;
        p.Colors[ImPlotCol_AxisTick] = text;
    }

#   endif

#   ifdef IMGUI_ENABLE_IMPLOT3D
    /// \brief Set baseline ImPlot3D style parameters.
    /// \param style Style to modify.
    /// \details Used by all themes before applying color scheme.
    inline void applyDefaultImPlot3DStyle(ImPlot3DStyle& style) {
        using namespace ImGuiX::Config;

        const ImGuiStyle& ig = ImGui::GetStyle();
        const float px = ig.FrameBorderSize > 0.0f ? ig.FrameBorderSize : FRAME_BORDER_SIZE;

        style.LineWeight   = px;
        style.MarkerWeight = px;

        style.PlotPadding    = WINDOW_PADDING;
        style.LabelPadding   = ig.ItemInnerSpacing;

        style.LegendPadding      = WINDOW_PADDING;
        style.LegendInnerPadding = ig.ItemInnerSpacing;
        style.LegendSpacing      = ITEM_SPACING;

        style.PlotDefaultSize = PLOT_DEFAULT_SIZE;
        style.PlotMinSize     = PLOT_MIN_SIZE;
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
            SyncImPlotWithImGui();
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
            SyncImPlotWithImGui();
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
            SyncImPlotWithImGui();
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
