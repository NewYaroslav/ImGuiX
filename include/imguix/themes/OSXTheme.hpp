#pragma once
#ifndef _IMGUIX_THEMES_OSX_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_OSX_THEME_HPP_INCLUDED

/// \file OSXTheme.hpp
/// \brief macOS-like (OSX) light theme for ImGui/ImPlot, ported to the ImGuiX theme system.
///
/// Based on the OSX palette floating around the discussion:
/// https://github.com/ocornut/imgui/issues/707
/// Adaptations:
///  - repeated colors moved to named constants
///  - unified layout via applyDefaultImGuiStyle
///  - added matching ImPlot styling (light)

#include <imguix/core/themes/Theme.hpp> // Theme + applyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace OSXThemeConstants
    /// \brief Color constants for the OSX light theme.
    namespace OSXThemeConstants {
        // Text
        constexpr ImVec4 Text                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        constexpr ImVec4 TextDisabled           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

        // Backgrounds
        constexpr ImVec4 WindowBg               = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
        constexpr ImVec4 ChildBg                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        constexpr ImVec4 PopupBg                = ImVec4(1.00f, 1.00f, 1.00f, 0.94f); // tooltip/menus

        // Borders
        constexpr ImVec4 Border                 = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
        constexpr ImVec4 BorderShadow           = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);

        // Frames
        constexpr ImVec4 FrameBg                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        constexpr ImVec4 FrameBgHovered         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        constexpr ImVec4 FrameBgActive          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);

        // Titles / Menu
        constexpr ImVec4 TitleBg                = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
        constexpr ImVec4 TitleBgCollapsed       = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
        constexpr ImVec4 TitleBgActive          = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
        constexpr ImVec4 MenuBarBg              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg            = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
        constexpr ImVec4 ScrollbarGrab          = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
        constexpr ImVec4 ScrollbarGrabHovered   = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
        constexpr ImVec4 ScrollbarGrabActive    = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);

        // Accents (macOS blue)
        constexpr ImVec4 BlueSoft               = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        constexpr ImVec4 BlueHover              = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        constexpr ImVec4 Blue                   = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        constexpr ImVec4 BlueActive             = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        constexpr ImVec4 BlueStrong             = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

        // Controls
        constexpr ImVec4 CheckMark              = Blue;
        constexpr ImVec4 SliderGrab             = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
        constexpr ImVec4 SliderGrabActive       = Blue;

        constexpr ImVec4 Button                 = BlueSoft;
        constexpr ImVec4 ButtonHovered          = Blue;
        constexpr ImVec4 ButtonActive           = BlueActive;

        constexpr ImVec4 Header                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        constexpr ImVec4 HeaderHovered          = BlueHover;
        constexpr ImVec4 HeaderActive           = Blue;

        // Misc / Separators / Grips
        constexpr ImVec4 ResizeGrip             = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
        constexpr ImVec4 ResizeGripHovered      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        constexpr ImVec4 ResizeGripActive       = BlueStrong;

        constexpr ImVec4 Separator              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        constexpr ImVec4 SeparatorHovered       = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
        constexpr ImVec4 SeparatorActive        = Blue;

        // Plots
        constexpr ImVec4 PlotLines              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        constexpr ImVec4 PlotLinesHovered       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        constexpr ImVec4 PlotHistogram          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        constexpr ImVec4 PlotHistogramHovered   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);

        // Selection / Nav
        constexpr ImVec4 TextSelectedBg         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        constexpr ImVec4 NavHighlight           = HeaderHovered;
        constexpr ImVec4 NavWindowingHighlight  = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    }

    /// \class OSXTheme
    /// \brief macOS-like light theme, unified with default layout config.
    class OSXTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace OSXThemeConstants;
            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                  = Text;
            colors[ImGuiCol_TextDisabled]          = TextDisabled;

            colors[ImGuiCol_WindowBg]              = WindowBg;
            colors[ImGuiCol_ChildBg]               = ChildBg;
            colors[ImGuiCol_PopupBg]               = PopupBg;

            colors[ImGuiCol_Border]                = Border;
            colors[ImGuiCol_BorderShadow]          = BorderShadow;

            colors[ImGuiCol_FrameBg]               = FrameBg;
            colors[ImGuiCol_FrameBgHovered]        = FrameBgHovered;
            colors[ImGuiCol_FrameBgActive]         = FrameBgActive;

            colors[ImGuiCol_TitleBg]               = TitleBg;
            colors[ImGuiCol_TitleBgCollapsed]      = TitleBgCollapsed;
            colors[ImGuiCol_TitleBgActive]         = TitleBgActive;
            colors[ImGuiCol_MenuBarBg]             = MenuBarBg;

            colors[ImGuiCol_ScrollbarBg]           = ScrollbarBg;
            colors[ImGuiCol_ScrollbarGrab]         = ScrollbarGrab;
            colors[ImGuiCol_ScrollbarGrabHovered]  = ScrollbarGrabHovered;
            colors[ImGuiCol_ScrollbarGrabActive]   = ScrollbarGrabActive;

            colors[ImGuiCol_CheckMark]             = CheckMark;
            colors[ImGuiCol_SliderGrab]            = SliderGrab;
            colors[ImGuiCol_SliderGrabActive]      = SliderGrabActive;

            colors[ImGuiCol_Button]                = Button;
            colors[ImGuiCol_ButtonHovered]         = ButtonHovered;
            colors[ImGuiCol_ButtonActive]          = ButtonActive;

            colors[ImGuiCol_Header]                = Header;
            colors[ImGuiCol_HeaderHovered]         = HeaderHovered;
            colors[ImGuiCol_HeaderActive]          = HeaderActive;

            colors[ImGuiCol_Separator]             = Separator;
            colors[ImGuiCol_SeparatorHovered]      = SeparatorHovered;
            colors[ImGuiCol_SeparatorActive]       = SeparatorActive;

            colors[ImGuiCol_ResizeGrip]            = ResizeGrip;
            colors[ImGuiCol_ResizeGripHovered]     = ResizeGripHovered;
            colors[ImGuiCol_ResizeGripActive]      = ResizeGripActive;

            colors[ImGuiCol_PlotLines]             = PlotLines;
            colors[ImGuiCol_PlotLinesHovered]      = PlotLinesHovered;
            colors[ImGuiCol_PlotHistogram]         = PlotHistogram;
            colors[ImGuiCol_PlotHistogramHovered]  = PlotHistogramHovered;

            colors[ImGuiCol_TextSelectedBg]        = TextSelectedBg;
            colors[ImGuiCol_NavHighlight]          = NavHighlight;
            colors[ImGuiCol_NavWindowingHighlight] = NavWindowingHighlight;

            // Unified roundings/paddings/borders from config
            applyDefaultImGuiStyle(style);

#ifdef IMGUI_HAS_DOCKING
            colors[ImGuiCol_DockingEmptyBg]        = WindowBg;
            colors[ImGuiCol_DockingPreview]        = ImVec4(Blue.x, Blue.y, Blue.z, 0.70f);

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
#endif
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            using namespace OSXThemeConstants;

            ImPlot::StyleColorsLight(&style);

            style.Colors[ImPlotCol_PlotBg]        = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]    = Border;
            style.Colors[ImPlotCol_LegendBg]      = PopupBg;
            style.Colors[ImPlotCol_LegendBorder]  = Border;
            style.Colors[ImPlotCol_LegendText]    = Text;

            style.Colors[ImPlotCol_TitleText]     = Text;
            style.Colors[ImPlotCol_InlayText]     = Text;
            style.Colors[ImPlotCol_AxisText]      = Text;

            // Light grid/ticks over bright bg
            style.Colors[ImPlotCol_AxisGrid]      = ImVec4(0.78f, 0.78f, 0.78f, 0.60f);
            style.Colors[ImPlotCol_AxisTick]      = ImVec4(0.35f, 0.35f, 0.35f, 0.90f);

            // Blue selection/crosshair to match controls
            style.Colors[ImPlotCol_Selection]     = ImVec4(Blue.x, Blue.y, Blue.z, 0.55f);
            style.Colors[ImPlotCol_Crosshairs]    = Blue;

            applyDefaultImPlotStyle(style);
        }
#endif
    };

    /// \brief Registers the OSX theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "osx-light").
    inline void registerOSXTheme(ThemeManager& tm, std::string id = "osx-light") {
        tm.registerTheme(std::move(id), std::make_unique<OSXTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_OSX_THEME_HPP_INCLUDED
