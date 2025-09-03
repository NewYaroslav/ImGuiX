#pragma once
#ifndef _IMGUIX_THEMES_GOLD_BLACK_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_GOLD_BLACK_THEME_HPP_INCLUDED

/// \file GoldBlackTheme.hpp
/// \brief Gold & Black theme for ImGui/ImPlot, ported to the ImGuiX theme system.
///
/// Based on palette ideas from: https://github.com/ocornut/imgui/issues/707
/// Adaptations:
///  - repeated colors moved to named constants
///  - unified layout via applyDefaultImGuiStyle
///  - added matching ImPlot styling

#include <imguix/core/themes/Theme.hpp> // Theme + applyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace GoldBlackConstants
    /// \brief Color constants for the Gold & Black theme.
    namespace GoldBlackConstants {
        // Text
        constexpr ImVec4 Text                   = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
        constexpr ImVec4 TextDisabled           = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);

        // Backgrounds
        constexpr ImVec4 WindowBg               = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        constexpr ImVec4 ChildBg                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        constexpr ImVec4 PopupBg                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);

        // Gold accents
        constexpr ImVec4 GoldBorder             = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
        constexpr ImVec4 GoldMed                = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
        constexpr ImVec4 GoldBright             = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
        constexpr ImVec4 GoldHeaderActive       = ImVec4(0.93f, 0.65f, 0.14f, 1.00f);

        // Frames / common greys
        constexpr ImVec4 FrameBg                = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
        constexpr ImVec4 FrameBgHovered         = GoldBorder;
        constexpr ImVec4 FrameBgActive          = GoldMed;

        // Titles / Menu
        constexpr ImVec4 TitleBg                = GoldBorder;
        constexpr ImVec4 TitleBgActive          = GoldBright;
        constexpr ImVec4 TitleBgCollapsed       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        constexpr ImVec4 MenuBarBg              = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg            = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
        constexpr ImVec4 ScrollbarGrab          = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
        constexpr ImVec4 ScrollbarGrabHovered   = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
        constexpr ImVec4 ScrollbarGrabActive    = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);

        // Controls
        constexpr ImVec4 CheckMark              = GoldMed;
        constexpr ImVec4 SliderGrab             = GoldBright;
        constexpr ImVec4 SliderGrabActive       = GoldBright;

        constexpr ImVec4 Button                 = GoldBorder;
        constexpr ImVec4 ButtonHovered          = GoldBright;
        constexpr ImVec4 ButtonActive           = GoldMed;

        constexpr ImVec4 Header                 = GoldBorder;
        constexpr ImVec4 HeaderHovered          = GoldBright;
        constexpr ImVec4 HeaderActive           = GoldHeaderActive;

        // Separators / grips
        constexpr ImVec4 Separator              = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
        constexpr ImVec4 SeparatorHovered       = GoldBright;
        constexpr ImVec4 SeparatorActive        = GoldMed;

        constexpr ImVec4 ResizeGrip             = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
        constexpr ImVec4 ResizeGripHovered      = GoldBright;
        constexpr ImVec4 ResizeGripActive       = GoldMed;

        // Tabs
        constexpr ImVec4 Tab                    = GoldBorder;
        constexpr ImVec4 TabHovered             = GoldBright;
        constexpr ImVec4 TabActive              = GoldMed;
        constexpr ImVec4 TabUnfocused           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f); // as original snippet
        constexpr ImVec4 TabUnfocusedActive     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f); // as original snippet

        // Plots
        constexpr ImVec4 PlotLines              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        constexpr ImVec4 PlotLinesHovered       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        constexpr ImVec4 PlotHistogram          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        constexpr ImVec4 PlotHistogramHovered   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);

        // Misc / Navigation
        constexpr ImVec4 TextSelectedBg         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        constexpr ImVec4 DragDropTarget         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        constexpr ImVec4 NavHighlight           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        constexpr ImVec4 NavWindowingHighlight  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        constexpr ImVec4 NavWindowingDimBg      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        constexpr ImVec4 ModalWindowDimBg       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        // Border shadow
        constexpr ImVec4 BorderShadow           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    }

    /// \class GoldBlackTheme
    /// \brief Gold & Black theme with warm golden accents, unified with default layout config.
    class GoldBlackTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace GoldBlackConstants;
            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                  = Text;
            colors[ImGuiCol_InputTextCursor]            = GoldBright;
            colors[ImGuiCol_TextDisabled]          = TextDisabled;

            colors[ImGuiCol_WindowBg]              = WindowBg;
            colors[ImGuiCol_ChildBg]               = ChildBg;
            colors[ImGuiCol_PopupBg]               = PopupBg;

            colors[ImGuiCol_Border]                = GoldBorder;
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

            // Unify sizes/roundings/paddings/borders from config
            applyDefaultImGuiStyle(style);

#ifdef IMGUIX_HAS_DOCKING
            colors[ImGuiCol_DockingEmptyBg]        = WindowBg;
            colors[ImGuiCol_DockingPreview]        = ImVec4(GoldBright.x, GoldBright.y, GoldBright.z, 0.70f); // warm overlay

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
#endif
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            using namespace GoldBlackConstants;

            ImPlot::StyleColorsDark(&style);

            ImVec4 frame = FrameBg; frame.w = 1.0f;
            style.Colors[ImPlotCol_FrameBg]      = frame;
            style.Colors[ImPlotCol_PlotBg]       = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]    = GoldBorder;
            style.Colors[ImPlotCol_LegendBg]      = PopupBg;
            style.Colors[ImPlotCol_LegendBorder]  = GoldBorder;
            style.Colors[ImPlotCol_LegendText]    = Text;

            style.Colors[ImPlotCol_TitleText]     = Text;
            style.Colors[ImPlotCol_InlayText]     = Text;
            style.Colors[ImPlotCol_AxisText]      = Text;

            // Warm grid/ticks to match gold-ish palette
            style.Colors[ImPlotCol_AxisGrid]      = ImVec4(0.40f, 0.32f, 0.18f, 0.45f);
            style.Colors[ImPlotCol_AxisTick]      = ImVec4(0.75f, 0.60f, 0.30f, 0.85f);
            style.Colors[ImPlotCol_AxisBgHovered] = ButtonHovered;
            style.Colors[ImPlotCol_AxisBgActive]  = ButtonActive;

            // Selection/crosshair in golden accent
            style.Colors[ImPlotCol_Selection]     = ImVec4(GoldBright.x, GoldBright.y, GoldBright.z, 0.65f);
            style.Colors[ImPlotCol_Crosshairs]    = GoldBright;

            applyDefaultImPlotStyle(style);
        }
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
        void apply(ImPlot3DStyle& style) const override {
            using namespace GoldBlackConstants;

            ImPlot3D::StyleColorsDark(&style);

            style.Colors[ImPlot3DCol_FrameBg]      = FrameBg;
            style.Colors[ImPlot3DCol_PlotBg]       = WindowBg;
            style.Colors[ImPlot3DCol_PlotBorder]   = GoldBorder;
            style.Colors[ImPlot3DCol_LegendBg]     = PopupBg;
            style.Colors[ImPlot3DCol_LegendBorder] = GoldBorder;
            style.Colors[ImPlot3DCol_LegendText]   = Text;
            style.Colors[ImPlot3DCol_TitleText]    = Text;
            style.Colors[ImPlot3DCol_InlayText]    = Text;
            style.Colors[ImPlot3DCol_AxisText]     = Text;

            style.Colors[ImPlot3DCol_AxisGrid]     = ImVec4(0.40f, 0.32f, 0.18f, 0.45f);
            style.Colors[ImPlot3DCol_AxisTick]     = ImVec4(0.75f, 0.60f, 0.30f, 0.85f);

            applyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Registers the Gold & Black theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "gold-black").
    inline void registerGoldBlackTheme(ThemeManager& tm, std::string id = "gold-black") {
        tm.registerTheme(std::move(id), std::make_unique<GoldBlackTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_GOLD_BLACK_THEME_HPP_INCLUDED
