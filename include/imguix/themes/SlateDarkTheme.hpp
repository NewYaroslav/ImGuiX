#pragma once
#ifndef _IMGUIX_THEMES_STALE_DARK_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_STALE_DARK_THEME_HPP_INCLUDED

/// \file SlateDarkTheme.hpp
/// \brief Extra themes (SlateDark) implemented in the same pattern as CorporateGreyTheme.
///
/// Each theme:
///  - uses named color constants in its own namespace
///  - calls applyDefaultImGuiStyle(style) to unify paddings/roundings/etc
///  - defines ImPlot colors to match the palette

#include <imguix/core/themes/Theme.hpp> // Theme base + applyDefaultImGuiStyle

namespace ImGuiX::Themes {

    // ======================================================================
    // SlateDarkTheme
    // ======================================================================

    /// \namespace SlateDarkConstants
    /// \brief Color constants for a dense, slate-dark theme with blue accents.
    namespace SlateDarkConstants {
        // Base
        constexpr ImVec4 Text            = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
        constexpr ImVec4 TextDisabled    = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        constexpr ImVec4 WindowBg        = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        constexpr ImVec4 ChildBg         = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        constexpr ImVec4 PopupBg         = ImVec4(0.10f, 0.10f, 0.10f, 0.98f);
        constexpr ImVec4 Border          = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        constexpr ImVec4 BorderShadow    = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);

        // Frames & headers
        constexpr ImVec4 FrameBg         = ImVec4(0.20f, 0.20f, 0.20f, 0.70f);
        constexpr ImVec4 FrameBgHovered  = ImVec4(0.26f, 0.26f, 0.26f, 0.85f);
        constexpr ImVec4 FrameBgActive   = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);

        constexpr ImVec4 TitleBg         = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        constexpr ImVec4 TitleBgActive   = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        constexpr ImVec4 TitleBgCollapsed= ImVec4(0.08f, 0.08f, 0.08f, 0.90f);
        constexpr ImVec4 MenuBarBg       = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg     = ImVec4(0.10f, 0.10f, 0.10f, 0.50f);
        constexpr ImVec4 ScrollbarGrab   = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
        constexpr ImVec4 ScrollbarHover  = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
        constexpr ImVec4 ScrollbarActive = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);

        // Accent (blue)
        constexpr ImVec4 AccentBase      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        constexpr ImVec4 AccentSoft      = ImVec4(0.26f, 0.59f, 0.98f, 0.34f);
        constexpr ImVec4 AccentHover     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        constexpr ImVec4 AccentActive    = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

        // Buttons/headers
        constexpr ImVec4 Button          = ImVec4(0.35f, 0.35f, 0.35f, 0.45f);
        constexpr ImVec4 ButtonHovered   = ImVec4(0.42f, 0.42f, 0.42f, 0.80f);
        constexpr ImVec4 ButtonActive    = ScrollbarActive;

        constexpr ImVec4 Header          = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        constexpr ImVec4 HeaderHovered   = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        constexpr ImVec4 HeaderActive    = ScrollbarActive;

        // Misc
        constexpr ImVec4 Separator       = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
        constexpr ImVec4 PlotLines       = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
        constexpr ImVec4 PlotLinesHover  = ImVec4(1.00f, 0.45f, 0.35f, 1.00f);
        constexpr ImVec4 PlotHistogram   = ImVec4(0.95f, 0.77f, 0.16f, 1.00f);
        constexpr ImVec4 PlotHistogramHv = ImVec4(1.00f, 0.86f, 0.28f, 1.00f);
        constexpr ImVec4 TextSelectedBg  = ImVec4(0.30f, 0.55f, 0.90f, 0.35f);
        constexpr ImVec4 ModalDimBg      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        constexpr ImVec4 DragDropTarget  = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        constexpr ImVec4 NavDimBg        = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        constexpr ImVec4 NavWinHighlight = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    }

    /// \class SlateDarkTheme
    /// \brief Dense slate-dark theme with blue accent, consistent with default layout constants.
    class SlateDarkTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace SlateDarkConstants;
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
            colors[ImGuiCol_ScrollbarGrabHovered]  = ScrollbarHover;
            colors[ImGuiCol_ScrollbarGrabActive]   = ScrollbarActive;

            colors[ImGuiCol_CheckMark]             = ScrollbarActive;
            colors[ImGuiCol_SliderGrab]            = ScrollbarHover;
            colors[ImGuiCol_SliderGrabActive]      = ScrollbarActive;

            colors[ImGuiCol_Button]                = Button;
            colors[ImGuiCol_ButtonHovered]         = ButtonHovered;
            colors[ImGuiCol_ButtonActive]          = ButtonActive;

            colors[ImGuiCol_Header]                = Header;
            colors[ImGuiCol_HeaderHovered]         = HeaderHovered;
            colors[ImGuiCol_HeaderActive]          = HeaderActive;

            colors[ImGuiCol_Separator]             = SlateDarkConstants::Separator;
            colors[ImGuiCol_SeparatorHovered]      = ScrollbarHover;
            colors[ImGuiCol_SeparatorActive]       = ScrollbarActive;

            colors[ImGuiCol_ResizeGrip]            = AccentSoft;
            colors[ImGuiCol_ResizeGripHovered]     = AccentHover;
            colors[ImGuiCol_ResizeGripActive]      = AccentActive;

            colors[ImGuiCol_PlotLines]             = PlotLines;
            colors[ImGuiCol_PlotLinesHovered]      = PlotLinesHover;
            colors[ImGuiCol_PlotHistogram]         = PlotHistogram;
            colors[ImGuiCol_PlotHistogramHovered]  = PlotHistogramHv;

            colors[ImGuiCol_TextSelectedBg]        = TextSelectedBg;
            colors[ImGuiCol_ModalWindowDimBg]      = ModalDimBg;
            colors[ImGuiCol_DragDropTarget]        = DragDropTarget;

            colors[ImGuiCol_NavHighlight]          = AccentBase;
            colors[ImGuiCol_NavWindowingHighlight] = NavWinHighlight;
            colors[ImGuiCol_NavWindowingDimBg]     = NavDimBg;

            // Unify sizes/roundings from theme_config.hpp
            applyDefaultImGuiStyle(style);

#ifdef IMGUI_HAS_DOCKING
            colors[ImGuiCol_DockingEmptyBg]        = ChildBg;
            colors[ImGuiCol_Tab]                   = FrameBg;
            colors[ImGuiCol_TabHovered]            = FrameBgHovered;
            colors[ImGuiCol_TabActive]             = FrameBgActive;
            colors[ImGuiCol_TabUnfocused]          = FrameBg;
            colors[ImGuiCol_TabUnfocusedActive]    = FrameBgActive;
            colors[ImGuiCol_DockingPreview]        = AccentHover;

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
#endif
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            using namespace SlateDarkConstants;

            ImPlot::StyleColorsDark(&style);
            style.Colors[ImPlotCol_PlotBg]       = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]   = Border;
            style.Colors[ImPlotCol_LegendBg]     = PopupBg;
            style.Colors[ImPlotCol_LegendBorder] = Border;
            style.Colors[ImPlotCol_LegendText]   = Text;
            style.Colors[ImPlotCol_TitleText]    = Text;
            style.Colors[ImPlotCol_InlayText]    = Text;
            style.Colors[ImPlotCol_AxisText]     = Text;
            style.Colors[ImPlotCol_AxisGrid]     = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
            style.Colors[ImPlotCol_AxisTick]     = ImVec4(0.55f, 0.55f, 0.55f, 0.80f);
            style.Colors[ImPlotCol_Selection]    = AccentBase;
            style.Colors[ImPlotCol_Crosshairs]   = AccentBase;

            applyDefaultImPlotStyle(style);
        }
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
        void apply(ImPlot3DStyle& style) const override {
            using namespace SlateDarkConstants;

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

            style.Colors[ImPlot3DCol_AxisGrid]     = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
            style.Colors[ImPlot3DCol_AxisTick]     = ImVec4(0.55f, 0.55f, 0.55f, 0.80f);

            applyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Helper to register SlateDark theme.
    inline void registerSlateDarkTheme(ThemeManager& tm, std::string id = "slate-dark") {
        tm.registerTheme(std::move(id), std::make_unique<SlateDarkTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_STALE_DARK_THEME_HPP_INCLUDED
