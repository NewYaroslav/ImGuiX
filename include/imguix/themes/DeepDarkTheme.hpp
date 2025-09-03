#pragma once
#ifndef _IMGUIX_THEMES_DEEP_DARK_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_DEEP_DARK_THEME_HPP_INCLUDED

/// \file DeepDarkTheme.hpp
/// \brief Deep Dark theme for ImGui/ImPlot, ported to the ImGuiX theme system.
///
/// Based on palette ideas from: https://github.com/ocornut/imgui/issues/707
/// Adaptations:
///  - repeated colors moved to named constants
///  - unified layout via applyDefaultImGuiStyle
///  - added matching ImPlot styling

#include <imguix/core/themes/Theme.hpp> // Theme + applyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace DeepDarkConstants
    /// \brief Color constants for the Deep Dark theme.
    namespace DeepDarkConstants {
        // Text
        constexpr ImVec4 Text                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        constexpr ImVec4 TextDisabled           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

        // Backgrounds
        constexpr ImVec4 WindowBg               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        constexpr ImVec4 ChildBg                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        constexpr ImVec4 PopupBg                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);

        // Borders
        constexpr ImVec4 Border                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
        constexpr ImVec4 BorderShadow           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);

        // Frames
        constexpr ImVec4 FrameBg                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        constexpr ImVec4 FrameBgHovered         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        constexpr ImVec4 FrameBgActive          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);

        // Titles / Menus
        constexpr ImVec4 TitleBg                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        constexpr ImVec4 TitleBgActive          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        constexpr ImVec4 TitleBgCollapsed       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        constexpr ImVec4 MenuBarBg              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        constexpr ImVec4 ScrollbarGrab          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        constexpr ImVec4 ScrollbarGrabHovered   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        constexpr ImVec4 ScrollbarGrabActive    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);

        // Marks / Sliders / Accents
        constexpr ImVec4 CheckMark              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        constexpr ImVec4 SliderGrab             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        constexpr ImVec4 SliderGrabActive       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);

        // Buttons / Headers
        constexpr ImVec4 Button                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        constexpr ImVec4 ButtonHovered          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        constexpr ImVec4 ButtonActive           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);

        constexpr ImVec4 Header                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        constexpr ImVec4 HeaderHovered          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
        constexpr ImVec4 HeaderActive           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);

        // Separators
        constexpr ImVec4 Separator              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        constexpr ImVec4 SeparatorHovered       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        constexpr ImVec4 SeparatorActive        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);

        // Resize grip
        constexpr ImVec4 ResizeGrip             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        constexpr ImVec4 ResizeGripHovered      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        constexpr ImVec4 ResizeGripActive       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);

        // Tabs
        constexpr ImVec4 Tab                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        constexpr ImVec4 TabHovered             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        constexpr ImVec4 TabActive              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
        constexpr ImVec4 TabUnfocused           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        constexpr ImVec4 TabUnfocusedActive     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

        // Tables
        constexpr ImVec4 TableHeaderBg          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        constexpr ImVec4 TableBorderStrong      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        constexpr ImVec4 TableBorderLight       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        constexpr ImVec4 TableRowBg             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        constexpr ImVec4 TableRowBgAlt          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);

        // Plots (red accent)
        constexpr ImVec4 PlotLines              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        constexpr ImVec4 PlotLinesHovered       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        constexpr ImVec4 PlotHistogram          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        constexpr ImVec4 PlotHistogramHovered   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);

        // Misc / Navigation
        constexpr ImVec4 TextSelectedBg         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        constexpr ImVec4 DragDropTarget         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);

        constexpr ImVec4 NavHighlight           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        constexpr ImVec4 NavWindowingHighlight  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
        constexpr ImVec4 NavWindowingDimBg      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
        constexpr ImVec4 ModalWindowDimBg       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
    }

    /// \class DeepDarkTheme
    /// \brief Deep dark theme with cyan and red accents, unified with default layout config.
    class DeepDarkTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace DeepDarkConstants;
            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                  = Text;
            colors[ImGuiCol_InputTextCursor]            = CheckMark;
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

#ifdef IMGUIX_HAS_DOCKING
            // Original snippet had docking colors commented out; we pick sane defaults.
            colors[ImGuiCol_DockingEmptyBg]        = WindowBg;
            colors[ImGuiCol_DockingPreview]        = ImVec4(0.33f, 0.67f, 0.86f, 1.00f); // cyan overlay

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
#endif
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            using namespace DeepDarkConstants;

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

            // Subtle grid/ticks over deep dark bg
            style.Colors[ImPlotCol_AxisGrid]      = ImVec4(0.35f, 0.35f, 0.35f, 0.40f);
            style.Colors[ImPlotCol_AxisTick]      = ImVec4(0.65f, 0.65f, 0.65f, 0.70f);
            style.Colors[ImPlotCol_AxisBgHovered] = ButtonHovered;
            style.Colors[ImPlotCol_AxisBgActive]  = ButtonActive;

            // Selection/crosshair use the red accent to match plot colors
            style.Colors[ImPlotCol_Selection]     = ImVec4(1.00f, 0.00f, 0.00f, 0.55f);
            style.Colors[ImPlotCol_Crosshairs]    = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);

            applyDefaultImPlotStyle(style);
        }
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
        void apply(ImPlot3DStyle& style) const override {
            using namespace DeepDarkConstants;

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

            style.Colors[ImPlot3DCol_AxisGrid]     = ImVec4(0.35f, 0.35f, 0.35f, 0.40f);
            style.Colors[ImPlot3DCol_AxisTick]     = ImVec4(0.65f, 0.65f, 0.65f, 0.70f);

            applyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Registers the Deep Dark theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "deep-dark").
    inline void registerDeepDarkTheme(ThemeManager& tm, std::string id = "deep-dark") {
        tm.registerTheme(std::move(id), std::make_unique<DeepDarkTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_DEEP_DARK_THEME_HPP_INCLUDED
