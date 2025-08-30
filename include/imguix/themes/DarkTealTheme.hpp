#pragma once
#ifndef _IMGUIX_THEMES_DARK_TEAL_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_DARK_TEAL_THEME_HPP_INCLUDED

/// \file DarkTealTheme.hpp
/// \brief Dark Teal theme for ImGui/ImPlot, ported to ImGuiX theme system.
///
/// Based on palette from: https://github.com/ocornut/imgui/issues/707 (dark variants),
/// adapted to:
///  - use named color constants
///  - unify paddings/roundings via applyDefaultImGuiStyle
///  - provide matching ImPlot styling

#include <imguix/core/themes/Theme.hpp> // Theme + applyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace DarkTealConstants
    /// \brief Color constants for the Dark Teal V2 theme.
    namespace DarkTealConstants {
        // Text
        constexpr ImVec4 Text                  = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        constexpr ImVec4 TextDisabled          = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);

        // Backgrounds
        constexpr ImVec4 WindowBg              = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        constexpr ImVec4 ChildBg               = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        constexpr ImVec4 PopupBg               = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);

        // Borders
        constexpr ImVec4 Border                = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        constexpr ImVec4 BorderShadow          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        // Frames
        constexpr ImVec4 FrameBg               = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        constexpr ImVec4 FrameBgHovered        = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
        constexpr ImVec4 FrameBgActive         = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);

        // Titles / Menu
        constexpr ImVec4 TitleBg               = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
        constexpr ImVec4 TitleBgActive         = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        constexpr ImVec4 TitleBgCollapsed      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        constexpr ImVec4 MenuBarBg             = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg           = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
        constexpr ImVec4 ScrollbarGrab         = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        constexpr ImVec4 ScrollbarGrabHovered  = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
        constexpr ImVec4 ScrollbarGrabActive   = ImVec4(0.09f, 0.21f, 0.31f, 1.00f); // teal-ish

        // Accent (blue)
        constexpr ImVec4 AccentBase            = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        constexpr ImVec4 AccentHover           = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        constexpr ImVec4 AccentSoft            = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        constexpr ImVec4 AccentSoft25          = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        constexpr ImVec4 AccentSoft67          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        constexpr ImVec4 AccentSoft95          = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

        // Marks / Sliders
        constexpr ImVec4 CheckMark             = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        constexpr ImVec4 SliderGrab            = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        constexpr ImVec4 SliderGrabActive      = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);

        // Buttons / Headers
        constexpr ImVec4 Button                = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        constexpr ImVec4 ButtonHovered         = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        constexpr ImVec4 ButtonActive          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);

        constexpr ImVec4 Header                = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
        constexpr ImVec4 HeaderHovered         = AccentHover;
        constexpr ImVec4 HeaderActive          = AccentBase;

        // Separators
        constexpr ImVec4 Separator             = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        constexpr ImVec4 SeparatorHovered      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        constexpr ImVec4 SeparatorActive       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);

        // Resize grip
        constexpr ImVec4 ResizeGrip            = AccentSoft25;
        constexpr ImVec4 ResizeGripHovered     = AccentSoft67;
        constexpr ImVec4 ResizeGripActive      = AccentSoft95;

        // Tabs
        constexpr ImVec4 Tab                   = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        constexpr ImVec4 TabHovered            = AccentHover;
        constexpr ImVec4 TabActive             = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        constexpr ImVec4 TabUnfocused          = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        constexpr ImVec4 TabUnfocusedActive    = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);

        // Plots
        constexpr ImVec4 PlotLines             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        constexpr ImVec4 PlotLinesHovered      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        constexpr ImVec4 PlotHistogram         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        constexpr ImVec4 PlotHistogramHovered  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);

        // Misc / Navigation
        constexpr ImVec4 TextSelectedBg        = AccentSoft;
        constexpr ImVec4 DragDropTarget        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        constexpr ImVec4 NavHighlight          = AccentBase;
        constexpr ImVec4 NavWindowingHighlight = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        constexpr ImVec4 NavWindowingDimBg     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        constexpr ImVec4 ModalWindowDimBg      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    /// \class DarkTealTheme
    /// \brief Dark teal theme with blue accent, consistent with ImGuiX default layout config.
    class DarkTealTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace DarkTealConstants;
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

            // Unify sizes/roundings from config
            applyDefaultImGuiStyle(style);

#ifdef IMGUI_HAS_DOCKING
            colors[ImGuiCol_DockingEmptyBg]        = WindowBg;
            colors[ImGuiCol_DockingPreview]        = ImVec4(0.26f, 0.59f, 0.98f, 0.55f); // accent overlay

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
#endif
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            using namespace DarkTealConstants;

            ImPlot::StyleColorsDark(&style);

            style.Colors[ImPlotCol_PlotBg]        = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]    = Border;
            style.Colors[ImPlotCol_LegendBg]      = PopupBg;
            style.Colors[ImPlotCol_LegendBorder]  = Border;
            style.Colors[ImPlotCol_LegendText]    = Text;

            style.Colors[ImPlotCol_TitleText]     = Text;
            style.Colors[ImPlotCol_InlayText]     = Text;
            style.Colors[ImPlotCol_AxisText]      = Text;

            // Subtle bluish grid/ticks for dark teal background
            style.Colors[ImPlotCol_AxisGrid]      = ImVec4(0.25f, 0.35f, 0.42f, 0.50f);
            style.Colors[ImPlotCol_AxisTick]      = ImVec4(0.55f, 0.65f, 0.70f, 0.85f);

            // Selection/crosshair use the same accent
            style.Colors[ImPlotCol_Selection]     = ImVec4(0.26f, 0.59f, 0.98f, 0.65f);
            style.Colors[ImPlotCol_Crosshairs]    = AccentBase;

            applyDefaultImPlotStyle(style);
        }
#endif
    };

    /// \brief Registers the Dark Teal theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "dark-teal").
    inline void registerDarkTealTheme(ThemeManager& tm, std::string id = "dark-teal") {
        tm.registerTheme(std::move(id), std::make_unique<DarkTealTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_DARK_TEAL_V2_THEME_HPP_INCLUDED
