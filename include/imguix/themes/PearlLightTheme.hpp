#pragma once
#ifndef _IMGUIX_THEMES_PEAR_LIGHT_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_PEAR_LIGHT_THEME_HPP_INCLUDED

/// \file PearlLightTheme.hpp
/// \brief Extra themes (PearlLightTheme) implemented in the same pattern as CorporateGreyTheme.
///
/// Each theme:
///  - uses named color constants in its own namespace
///  - calls applyDefaultImGuiStyle(style) to unify paddings/roundings/etc
///  - defines ImPlot colors to match the palette

#include <imguix/core/themes/Theme.hpp> // Theme base + applyDefaultImGuiStyle

namespace ImGuiX::Themes {

    // ======================================================================
    // PearlLightTheme
    // ======================================================================

    /// \namespace PearlLightConstants
    /// \brief Color constants for a soft light theme with the same blue accent.
    namespace PearlLightConstants {
        // Base
        constexpr ImVec4 Text            = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        constexpr ImVec4 TextDisabled    = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
        constexpr ImVec4 WindowBg        = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
        constexpr ImVec4 ChildBg         = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
        constexpr ImVec4 PopupBg         = ImVec4(0.99f, 0.99f, 0.99f, 0.98f);
        constexpr ImVec4 Border          = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
        constexpr ImVec4 BorderShadow    = ImVec4(0.00f, 0.00f, 0.00f, 0.06f);

        // Frames & headers
        constexpr ImVec4 FrameBg         = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
        constexpr ImVec4 FrameBgHovered  = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
        constexpr ImVec4 FrameBgActive   = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);

        constexpr ImVec4 TitleBg         = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
        constexpr ImVec4 TitleBgActive   = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
        constexpr ImVec4 TitleBgCollapsed= ImVec4(0.94f, 0.94f, 0.94f, 0.90f);
        constexpr ImVec4 MenuBarBg       = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg     = ImVec4(0.96f, 0.96f, 0.96f, 0.50f);
        constexpr ImVec4 ScrollbarGrab   = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
        constexpr ImVec4 ScrollbarHover  = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
        constexpr ImVec4 ScrollbarActive = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);

        // Accent (blue)
        constexpr ImVec4 AccentBase      = ImVec4(0.20f, 0.52f, 0.93f, 1.00f);
        constexpr ImVec4 AccentSoft      = ImVec4(0.20f, 0.52f, 0.93f, 0.26f);
        constexpr ImVec4 AccentHover     = ImVec4(0.20f, 0.52f, 0.93f, 0.60f);
        constexpr ImVec4 AccentActive    = ImVec4(0.20f, 0.52f, 0.93f, 0.90f);

        // Buttons/headers
        constexpr ImVec4 Button          = ImVec4(0.82f, 0.82f, 0.82f, 0.60f);
        constexpr ImVec4 ButtonHovered   = ImVec4(0.74f, 0.74f, 0.74f, 0.90f);
        constexpr ImVec4 ButtonActive    = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

        constexpr ImVec4 Header          = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
        constexpr ImVec4 HeaderHovered   = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
        constexpr ImVec4 HeaderActive    = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);

        // Misc
        constexpr ImVec4 Separator       = ImVec4(0.00f, 0.00f, 0.00f, 0.13f);
        constexpr ImVec4 PlotLines       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        constexpr ImVec4 PlotLinesHover  = ImVec4(0.90f, 0.35f, 0.30f, 1.00f);
        constexpr ImVec4 PlotHistogram   = ImVec4(0.85f, 0.65f, 0.08f, 1.00f);
        constexpr ImVec4 PlotHistogramHv = ImVec4(0.95f, 0.74f, 0.10f, 1.00f);
        constexpr ImVec4 TextSelectedBg  = ImVec4(0.20f, 0.52f, 0.93f, 0.25f);
        constexpr ImVec4 ModalDimBg      = ImVec4(0.12f, 0.12f, 0.12f, 0.20f);
        constexpr ImVec4 DragDropTarget  = ImVec4(0.98f, 0.80f, 0.00f, 0.90f);
        constexpr ImVec4 NavDimBg        = ImVec4(0.10f, 0.10f, 0.10f, 0.10f);
        constexpr ImVec4 NavWinHighlight = ImVec4(0.15f, 0.15f, 0.15f, 0.30f);
    }

    /// \class PearlLightTheme
    /// \brief Soft light theme with gentle grays and a blue accent, unified with default layout constants.
    class PearlLightTheme final : public Theme {
    public:
        /// \brief Apply theme colors to ImGui style.
        /// \param style Target style.
        void apply(ImGuiStyle& style) const override {
            using namespace PearlLightConstants;
            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                  = Text;
            colors[ImGuiCol_InputTextCursor]            = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
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

            colors[ImGuiCol_Separator]             = PearlLightConstants::Separator;
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

#ifdef IMGUIX_HAS_DOCKING
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
        /// \brief Apply theme colors to ImPlot style.
        /// \param style Target style.
        void apply(ImPlotStyle& style) const override {
            using namespace PearlLightConstants;

            ImPlot::StyleColorsLight(&style);
            ImVec4 frame = FrameBg; frame.w = 1.0f;
            style.Colors[ImPlotCol_FrameBg]      = frame;
            style.Colors[ImPlotCol_PlotBg]       = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]   = Border;
            style.Colors[ImPlotCol_LegendBg]     = PopupBg;
            style.Colors[ImPlotCol_LegendBorder] = Border;
            style.Colors[ImPlotCol_LegendText]   = Text;
            style.Colors[ImPlotCol_TitleText]    = Text;
            style.Colors[ImPlotCol_InlayText]    = Text;
            style.Colors[ImPlotCol_AxisText]     = Text;
            style.Colors[ImPlotCol_AxisGrid]     = ImVec4(0.75f, 0.75f, 0.75f, 0.60f);
            style.Colors[ImPlotCol_AxisTick]     = ImVec4(0.40f, 0.40f, 0.40f, 0.90f);
            style.Colors[ImPlotCol_AxisBgHovered] = ButtonHovered;
            style.Colors[ImPlotCol_AxisBgActive]  = ButtonActive;
            style.Colors[ImPlotCol_Selection]    = AccentBase;
            style.Colors[ImPlotCol_Crosshairs]   = AccentBase;

            applyDefaultImPlotStyle(style);
        }
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
        /// \brief Apply theme colors to ImPlot3D style.
        /// \param style Target style.
        void apply(ImPlot3DStyle& style) const override {
            using namespace PearlLightConstants;

            ImPlot3D::StyleColorsLight(&style);

            style.Colors[ImPlot3DCol_FrameBg]      = FrameBg;
            style.Colors[ImPlot3DCol_PlotBg]       = WindowBg;
            style.Colors[ImPlot3DCol_PlotBorder]   = Border;
            style.Colors[ImPlot3DCol_LegendBg]     = PopupBg;
            style.Colors[ImPlot3DCol_LegendBorder] = Border;
            style.Colors[ImPlot3DCol_LegendText]   = Text;
            style.Colors[ImPlot3DCol_TitleText]    = Text;
            style.Colors[ImPlot3DCol_InlayText]    = Text;
            style.Colors[ImPlot3DCol_AxisText]     = Text;

            style.Colors[ImPlot3DCol_AxisGrid]     = ImVec4(0.75f, 0.75f, 0.75f, 0.60f);
            style.Colors[ImPlot3DCol_AxisTick]     = ImVec4(0.40f, 0.40f, 0.40f, 0.90f);

            applyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Helper to register PearlLight theme.
    inline void registerPearlLightTheme(ThemeManager& tm, std::string id = "pearl-light") {
        tm.registerTheme(std::move(id), std::make_unique<PearlLightTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_PEAR_LIGHT_THEME_HPP_INCLUDED
