#pragma once
#ifndef _IMGUIX_THEMES_GREEN_BLUE_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_GREEN_BLUE_THEME_HPP_INCLUDED

/// \file GreenBlueTheme.hpp
/// \brief Green & Blue theme for ImGui/ImPlot, ported to the ImGuiX theme system.
///
/// Based on palette ideas from: https://github.com/ocornut/imgui/issues/707
/// Adaptations:
///  - repeated colors moved to named constants
///  - unified layout via applyDefaultImGuiStyle
///  - added matching ImPlot styling

#include <imguix/core/themes/Theme.hpp> // Theme + applyDefaultImGuiStyle
#ifdef IMGUI_ENABLE_IMPLOT3D
#   include <implot3d.h>
#endif

namespace ImGuiX::Themes {

    /// \namespace GreenBlueConstants
    /// \brief Color constants for the Green & Blue theme.
    namespace GreenBlueConstants {
        // Text
        constexpr ImVec4 Text                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        constexpr ImVec4 TextDisabled           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

        // Backgrounds
        constexpr ImVec4 WindowBg               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
        constexpr ImVec4 ChildBg                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        constexpr ImVec4 PopupBg                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);

        // Borders
        constexpr ImVec4 Border                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        constexpr ImVec4 BorderShadow           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        // Frames (base greys)
        constexpr ImVec4 FrameBg                = ImVec4(0.44f, 0.44f, 0.44f, 0.60f);
        constexpr ImVec4 FrameBgHovered         = ImVec4(0.57f, 0.57f, 0.57f, 0.70f);
        constexpr ImVec4 FrameBgActive          = ImVec4(0.76f, 0.76f, 0.76f, 0.80f);

        // Title / Menu
        constexpr ImVec4 TitleBg                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
        constexpr ImVec4 TitleBgActive          = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        constexpr ImVec4 TitleBgCollapsed       = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
        constexpr ImVec4 MenuBarBg              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        constexpr ImVec4 ScrollbarGrab          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        constexpr ImVec4 ScrollbarGrabHovered   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        constexpr ImVec4 ScrollbarGrabActive    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);

        // Green–Blue accents (three intensities)
        constexpr ImVec4 AccentGreen            = ImVec4(0.13f, 0.75f, 0.55f, 0.80f);
        constexpr ImVec4 AccentTeal             = ImVec4(0.13f, 0.75f, 0.75f, 0.80f);
        constexpr ImVec4 AccentCyan             = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);

        // Controls (buttons/headers/separators use the same triad with varying alphas)
        constexpr ImVec4 Button                 = ImVec4(0.13f, 0.75f, 0.55f, 0.40f);
        constexpr ImVec4 ButtonHovered          = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
        constexpr ImVec4 ButtonActive           = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);

        constexpr ImVec4 Header                 = ImVec4(0.13f, 0.75f, 0.55f, 0.40f);
        constexpr ImVec4 HeaderHovered          = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
        constexpr ImVec4 HeaderActive           = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);

        constexpr ImVec4 Separator              = ImVec4(0.13f, 0.75f, 0.55f, 0.40f);
        constexpr ImVec4 SeparatorHovered       = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
        constexpr ImVec4 SeparatorActive        = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);

        constexpr ImVec4 ResizeGrip             = ImVec4(0.13f, 0.75f, 0.55f, 0.40f);
        constexpr ImVec4 ResizeGripHovered      = ImVec4(0.13f, 0.75f, 0.75f, 0.60f);
        constexpr ImVec4 ResizeGripActive       = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);

        // Tabs
        constexpr ImVec4 Tab                    = ImVec4(0.13f, 0.75f, 0.55f, 0.80f);
        constexpr ImVec4 TabHovered             = ImVec4(0.13f, 0.75f, 0.75f, 0.80f);
        constexpr ImVec4 TabActive              = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);
        constexpr ImVec4 TabUnfocused           = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        constexpr ImVec4 TabUnfocusedActive     = ImVec4(0.36f, 0.36f, 0.36f, 0.54f);

        // Plots
        constexpr ImVec4 PlotLines              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        constexpr ImVec4 PlotLinesHovered       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        constexpr ImVec4 PlotHistogram          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        constexpr ImVec4 PlotHistogramHovered   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);

        // Tables
        constexpr ImVec4 TableHeaderBg          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        constexpr ImVec4 TableBorderStrong      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        constexpr ImVec4 TableBorderLight       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        constexpr ImVec4 TableRowBg             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        constexpr ImVec4 TableRowBgAlt          = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);

        // Misc / Navigation
        constexpr ImVec4 CheckMark              = ImVec4(0.13f, 0.75f, 0.55f, 0.80f);
        constexpr ImVec4 SliderGrab             = ImVec4(0.13f, 0.75f, 0.75f, 0.80f);
        constexpr ImVec4 SliderGrabActive       = ImVec4(0.13f, 0.75f, 1.00f, 0.80f);

        constexpr ImVec4 TextSelectedBg         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        constexpr ImVec4 DragDropTarget         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        constexpr ImVec4 NavHighlight           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        constexpr ImVec4 NavWindowingHighlight  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        constexpr ImVec4 NavWindowingDimBg      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        constexpr ImVec4 ModalWindowDimBg       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    /// \class GreenBlueTheme
    /// \brief Green & Blue theme with tri-tone accent (green/teal/cyan), unified with default layout config.
    class GreenBlueTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace GreenBlueConstants;
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
            colors[ImGuiCol_TitleBgActive]         = TitleBgActive;
            colors[ImGuiCol_TitleBgCollapsed]      = TitleBgCollapsed;
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

            colors[ImGuiCol_Tab]                   = Tab;
            colors[ImGuiCol_TabHovered]            = TabHovered;
            colors[ImGuiCol_TabActive]             = TabActive;
            colors[ImGuiCol_TabUnfocused]          = TabUnfocused;
            colors[ImGuiCol_TabUnfocusedActive]    = TabUnfocusedActive;

            // Tables
            colors[ImGuiCol_TableHeaderBg]         = TableHeaderBg;
            colors[ImGuiCol_TableBorderStrong]     = TableBorderStrong;
            colors[ImGuiCol_TableBorderLight]      = TableBorderLight;
            colors[ImGuiCol_TableRowBg]            = TableRowBg;
            colors[ImGuiCol_TableRowBgAlt]         = TableRowBgAlt;

            // Plots
            colors[ImGuiCol_PlotLines]             = PlotLines;
            colors[ImGuiCol_PlotLinesHovered]      = PlotLinesHovered;
            colors[ImGuiCol_PlotHistogram]         = PlotHistogram;
            colors[ImGuiCol_PlotHistogramHovered]  = PlotHistogramHovered;

            // Misc / Navigation
            colors[ImGuiCol_TextSelectedBg]        = TextSelectedBg;
            colors[ImGuiCol_DragDropTarget]        = DragDropTarget;

            colors[ImGuiCol_NavHighlight]          = NavHighlight;
            colors[ImGuiCol_NavWindowingHighlight] = NavWindowingHighlight;
            colors[ImGuiCol_NavWindowingDimBg]     = NavWindowingDimBg;
            colors[ImGuiCol_ModalWindowDimBg]      = ModalWindowDimBg;

            // Unified roundings/paddings/borders from config
            applyDefaultImGuiStyle(style);

#ifdef IMGUI_HAS_DOCKING
            // Original snippet had docking colors commented; apply consistent values.
            colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.13f, 0.13f, 0.13f, 0.80f);
            colors[ImGuiCol_DockingPreview]        = ImVec4(0.13f, 0.75f, 0.55f, 0.80f);

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
#endif
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            using namespace GreenBlueConstants;

            ImPlot::StyleColorsDark(&style);

            style.Colors[ImPlotCol_PlotBg]        = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]    = Border;
            style.Colors[ImPlotCol_LegendBg]      = PopupBg;
            style.Colors[ImPlotCol_LegendBorder]  = Border;
            style.Colors[ImPlotCol_LegendText]    = Text;

            style.Colors[ImPlotCol_TitleText]     = Text;
            style.Colors[ImPlotCol_InlayText]     = Text;
            style.Colors[ImPlotCol_AxisText]      = Text;

            // Slightly warm/neutral grid & ticks to sit over dark bg
            style.Colors[ImPlotCol_AxisGrid]      = ImVec4(0.35f, 0.35f, 0.36f, 0.55f);
            style.Colors[ImPlotCol_AxisTick]      = ImVec4(0.60f, 0.60f, 0.62f, 0.85f);

            // Match selection/crosshair to green-blue accents
            style.Colors[ImPlotCol_Selection]     = ImVec4(0.13f, 0.75f, 1.00f, 0.65f); // cyan, semi-opaque
            style.Colors[ImPlotCol_Crosshairs]    = ImVec4(0.13f, 0.75f, 0.75f, 1.00f); // teal

            applyDefaultImPlotStyle(style);
        }
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
        void apply(ImPlot3DStyle& style) const override {
            using namespace GreenBlueConstants;

            ImPlot3D::StyleColorsDark(&style);

            style.Colors[ImPlot3DCol_FrameBg]      = FrameBg;
            style.Colors[ImPlot3DCol_PlotBg]       = WindowBg;
            style.Colors[ImPlot3DCol_PlotBorder]   = Border;
            style.Colors[ImPlot3DCol_LegendBg]     = PopupBg;
            style.Colors[ImPlot3DCol_LegendBorder] = Border;
            style.Colors[ImPlot3DCol_LegendText]   = Text;
            style.Colors[ImPlot3DCol_TitleText]    = Text;
            style.Colors[ImPlot3DCol_InlayText]    = Text;
            style.Colors[ImPlot3DCol_AxisText]     = Text;

            style.Colors[ImPlot3DCol_AxisGrid]     = ImVec4(0.35f, 0.35f, 0.36f, 0.55f);
            style.Colors[ImPlot3DCol_AxisTick]     = ImVec4(0.60f, 0.60f, 0.62f, 0.85f);

            applyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Registers the Green & Blue theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "green-blue").
    inline void registerGreenBlueTheme(ThemeManager& tm, std::string id = "green-blue") {
        tm.registerTheme(std::move(id), std::make_unique<GreenBlueTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_GREEN_BLUE_THEME_HPP_INCLUDED
