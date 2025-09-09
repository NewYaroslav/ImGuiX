#pragma once
#ifndef _IMGUIX_THEMES_DARK_GRAPHITE_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_DARK_GRAPHITE_THEME_HPP_INCLUDED

/// \file DarkGraphiteTheme.hpp
/// \brief Dark Graphite theme for ImGui/ImPlot, ported to the ImGuiX theme system.
///
/// Based on palette ideas from: https://github.com/ocornut/imgui/issues/707
/// Adaptations:
///  - repeated colors moved to named constants
///  - unified layout via ApplyDefaultImGuiStyle
///  - added matching ImPlot styling

#include <imguix/core/themes/Theme.hpp> // Theme + ApplyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace DarkGraphiteV3Constants
    /// \brief Color constants for the Dark Graphite V3 theme.
    namespace DarkGraphiteConstants {
        // Text
        constexpr ImVec4 Text                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        constexpr ImVec4 TextDisabled          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

        // Backgrounds
        constexpr ImVec4 WindowBg              = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        constexpr ImVec4 ChildBg               = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        constexpr ImVec4 PopupBg               = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);

        // Borders
        constexpr ImVec4 Border                = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        constexpr ImVec4 BorderShadow          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        // Frames
        constexpr ImVec4 FrameBg               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        constexpr ImVec4 FrameBgHovered        = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        constexpr ImVec4 FrameBgActive         = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);

        // Titles / Menus
        constexpr ImVec4 TitleBg               = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        constexpr ImVec4 TitleBgActive         = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        constexpr ImVec4 TitleBgCollapsed      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        constexpr ImVec4 MenuBarBg             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        constexpr ImVec4 ScrollbarGrab         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        constexpr ImVec4 ScrollbarGrabHovered  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        constexpr ImVec4 ScrollbarGrabActive   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);

        // Accents (blue/cyan family)
        constexpr ImVec4 AccentBlueStrong      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        constexpr ImVec4 AccentBlue            = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        constexpr ImVec4 AccentBlueSoft        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        constexpr ImVec4 CyanBase              = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        constexpr ImVec4 CyanActive            = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);

        // Buttons / Headers
        constexpr ImVec4 Button                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        constexpr ImVec4 ButtonHovered         = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        constexpr ImVec4 ButtonActive          = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);

        constexpr ImVec4 Header                = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        constexpr ImVec4 HeaderHovered         = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
        constexpr ImVec4 HeaderActive          = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);

        // Separators
        constexpr ImVec4 Separator             = Border; // equals Border from original
        constexpr ImVec4 SeparatorHovered      = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
        constexpr ImVec4 SeparatorActive       = AccentBlueStrong;

        // Resize grip
        constexpr ImVec4 ResizeGrip            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        constexpr ImVec4 ResizeGripHovered     = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
        constexpr ImVec4 ResizeGripActive      = AccentBlueStrong;

        // Tabs
        constexpr ImVec4 Tab                   = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
        constexpr ImVec4 TabHovered            = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
        constexpr ImVec4 TabActive             = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
        constexpr ImVec4 TabUnfocused          = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        constexpr ImVec4 TabUnfocusedActive    = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);

        // Plots
        constexpr ImVec4 PlotLines             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        constexpr ImVec4 PlotLinesHovered      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        constexpr ImVec4 PlotHistogram         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        constexpr ImVec4 PlotHistogramHovered  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);

        // Misc / Navigation
        constexpr ImVec4 TextSelectedBg        = CyanBase;
        constexpr ImVec4 DragDropTarget        = CyanBase;
        constexpr ImVec4 NavHighlight          = CyanBase;
        constexpr ImVec4 NavWindowingHighlight = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        constexpr ImVec4 NavWindowingDimBg     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        constexpr ImVec4 ModalWindowDimBg      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    /// \class DarkGraphiteTheme
    /// \brief Dark graphite theme with blue/cyan accents, unified with default layout config.
    class DarkGraphiteTheme final : public Theme {
    public:
        /// \brief Apply theme colors to ImGui style.
        /// \param style Target style.
        void apply(ImGuiStyle& style) const override {
            using namespace DarkGraphiteConstants;
            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                  = Text;
            colors[ImGuiCol_InputTextCursor]            = CyanBase;
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

            colors[ImGuiCol_CheckMark]             = CyanBase;
            colors[ImGuiCol_SliderGrab]            = CyanBase;
            colors[ImGuiCol_SliderGrabActive]      = CyanActive;

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

            colors[ImGuiCol_PlotLines]             = PlotLines;
            colors[ImGuiCol_PlotLinesHovered]      = PlotLinesHovered;
            colors[ImGuiCol_PlotHistogram]         = PlotHistogram;
            colors[ImGuiCol_PlotHistogramHovered]  = PlotHistogramHovered;

            colors[ImGuiCol_TextSelectedBg]        = TextSelectedBg;
            colors[ImGuiCol_DragDropTarget]        = DragDropTarget;

            colors[ImGuiCol_NavHighlight]          = NavHighlight;
            colors[ImGuiCol_NavWindowingHighlight] = NavWindowingHighlight;
            colors[ImGuiCol_NavWindowingDimBg]     = NavWindowingDimBg;
            colors[ImGuiCol_ModalWindowDimBg]      = ModalWindowDimBg;

            // Apply unified roundings/paddings/borders from config
            ApplyDefaultImGuiStyle(style);

#ifdef IMGUIX_HAS_DOCKING
            colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            colors[ImGuiCol_DockingPreview]        = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
#endif
        }

#ifdef IMGUI_ENABLE_IMPLOT
        /// \brief Apply theme colors to ImPlot style.
        /// \param style Target style.
        void apply(ImPlotStyle& style) const override {
            using namespace DarkGraphiteConstants;

            ImPlot::StyleColorsDark(&style);

            ImVec4 frame = FrameBg; frame.w = 1.0f;
            style.Colors[ImPlotCol_FrameBg]      = frame;
            style.Colors[ImPlotCol_PlotBg]       = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]    = Border;
            style.Colors[ImPlotCol_LegendBg]      = PopupBg;
            style.Colors[ImPlotCol_LegendBorder]  = Border;
            style.Colors[ImPlotCol_LegendText]    = Text;

            style.Colors[ImPlotCol_TitleText]     = Text;
            style.Colors[ImPlotCol_InlayText]     = Text;
            style.Colors[ImPlotCol_AxisText]      = Text;

            style.Colors[ImPlotCol_AxisGrid]      = ImVec4(0.34f, 0.35f, 0.38f, 0.55f);
            style.Colors[ImPlotCol_AxisTick]      = ImVec4(0.62f, 0.64f, 0.68f, 0.90f);
            style.Colors[ImPlotCol_AxisBgHovered] = ButtonHovered;
            style.Colors[ImPlotCol_AxisBgActive]  = ButtonActive;

            style.Colors[ImPlotCol_Selection]     = AccentBlue;
            style.Colors[ImPlotCol_Crosshairs]    = AccentBlue;

            ApplyDefaultImPlotStyle(style);
        }
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
        /// \brief Apply theme colors to ImPlot3D style.
        /// \param style Target style.
        void apply(ImPlot3DStyle& style) const override {
            using namespace DarkGraphiteConstants;

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

            style.Colors[ImPlot3DCol_AxisGrid]     = ImVec4(0.34f, 0.35f, 0.38f, 0.55f);
            style.Colors[ImPlot3DCol_AxisTick]     = ImVec4(0.62f, 0.64f, 0.68f, 0.90f);

            ApplyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Registers the Dark Graphite theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "dark-graphite").
    inline void registerDarkGraphiteTheme(ThemeManager& tm, std::string id = "dark-graphite") {
        tm.registerTheme(std::move(id), std::make_unique<DarkGraphiteTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_DARK_GRAPHITE_THEME_HPP_INCLUDED
