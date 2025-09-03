#pragma once
#ifndef _IMGUIX_THEMES_LIGHT_GREEN_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_LIGHT_GREEN_THEME_HPP_INCLUDED

/// \file LightGreenTheme.hpp
/// \brief Light-Green theme for ImGui/ImPlot, ported to the ImGuiX theme system.
///
/// Based on palette ideas from: https://github.com/ocornut/imgui/issues/707
/// Adaptations:
///  - repeated colors moved to named constants
///  - unified layout via applyDefaultImGuiStyle
///  - added matching ImPlot styling (light)

#include <imguix/core/themes/Theme.hpp> // Theme + applyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace LightGreenConstants
    /// \brief Color constants for the Light-Green theme.
    namespace LightGreenConstants {
        // Text
        constexpr ImVec4 Text                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        constexpr ImVec4 TextDisabled           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

        // Backgrounds
        constexpr ImVec4 WindowBg               = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
        constexpr ImVec4 ChildBg                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        constexpr ImVec4 PopupBg                = ImVec4(0.93f, 0.93f, 0.93f, 0.98f);

        // Borders
        constexpr ImVec4 Border                 = ImVec4(0.71f, 0.71f, 0.71f, 0.08f);
        constexpr ImVec4 BorderShadow           = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);

        // Frames
        constexpr ImVec4 FrameBg                = ImVec4(0.71f, 0.71f, 0.71f, 0.55f);
        constexpr ImVec4 FrameBgHovered         = ImVec4(0.94f, 0.94f, 0.94f, 0.55f);
        constexpr ImVec4 FrameBgActive          = ImVec4(0.71f, 0.78f, 0.69f, 0.98f);

        // Titles / Menu
        constexpr ImVec4 TitleBg                = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
        constexpr ImVec4 TitleBgCollapsed       = ImVec4(0.82f, 0.78f, 0.78f, 0.51f);
        constexpr ImVec4 TitleBgActive          = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
        constexpr ImVec4 MenuBarBg              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg            = ImVec4(0.20f, 0.25f, 0.30f, 0.61f);
        constexpr ImVec4 ScrollbarGrab          = ImVec4(0.90f, 0.90f, 0.90f, 0.30f);
        constexpr ImVec4 ScrollbarGrabHovered   = ImVec4(0.92f, 0.92f, 0.92f, 0.78f);
        constexpr ImVec4 ScrollbarGrabActive    = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

        // Accents
        constexpr ImVec4 CheckMark              = ImVec4(0.184f, 0.407f, 0.193f, 1.00f);
        constexpr ImVec4 BlueSoft               = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
        constexpr ImVec4 Blue                   = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

        // Buttons / Headers (greenish range)
        constexpr ImVec4 Button                 = ImVec4(0.71f, 0.78f, 0.69f, 0.40f);
        constexpr ImVec4 ButtonHovered          = ImVec4(0.725f, 0.805f, 0.702f, 1.00f);
        constexpr ImVec4 ButtonActive           = ImVec4(0.793f, 0.900f, 0.836f, 1.00f);

        constexpr ImVec4 Header                 = ImVec4(0.71f, 0.78f, 0.69f, 0.31f);
        constexpr ImVec4 HeaderHovered          = ImVec4(0.71f, 0.78f, 0.69f, 0.80f);
        constexpr ImVec4 HeaderActive           = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);

        // Separators / grips
        constexpr ImVec4 Separator              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        constexpr ImVec4 SeparatorHovered       = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
        constexpr ImVec4 SeparatorActive        = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);

        constexpr ImVec4 ResizeGrip             = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
        constexpr ImVec4 ResizeGripHovered      = ImVec4(0.26f, 0.59f, 0.98f, 0.45f);
        constexpr ImVec4 ResizeGripActive       = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);

        // Tabs
        constexpr ImVec4 Tab                    = ImVec4(0.71f, 0.78f, 0.69f, 0.80f);
        constexpr ImVec4 TabHovered             = ImVec4(0.71f, 0.78f, 0.69f, 0.80f); // close to HeaderHovered
        constexpr ImVec4 TabActive              = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);
        constexpr ImVec4 TabUnfocused           = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        constexpr ImVec4 TabUnfocusedActive     = ImVec4(0.36f, 0.36f, 0.36f, 0.54f);

        // Plots
        constexpr ImVec4 PlotLines              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        constexpr ImVec4 PlotLinesHovered       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        constexpr ImVec4 PlotHistogram          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        constexpr ImVec4 PlotHistogramHovered   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);

        // Misc / Navigation
        constexpr ImVec4 TextSelectedBg         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        constexpr ImVec4 DragDropTarget         = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        constexpr ImVec4 NavHighlight           = HeaderHovered; // as in the original snippet
        constexpr ImVec4 NavWindowingHighlight  = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    }

    /// \class LightGreenTheme
    /// \brief Light theme with soft green accents, unified with default layout config.
    class LightGreenTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace LightGreenConstants;
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
            colors[ImGuiCol_TitleBgCollapsed]      = TitleBgCollapsed;
            colors[ImGuiCol_TitleBgActive]         = TitleBgActive;
            colors[ImGuiCol_MenuBarBg]             = MenuBarBg;

            colors[ImGuiCol_ScrollbarBg]           = ScrollbarBg;
            colors[ImGuiCol_ScrollbarGrab]         = ScrollbarGrab;
            colors[ImGuiCol_ScrollbarGrabHovered]  = ScrollbarGrabHovered;
            colors[ImGuiCol_ScrollbarGrabActive]   = ScrollbarGrabActive;

            colors[ImGuiCol_CheckMark]             = CheckMark;
            colors[ImGuiCol_SliderGrab]            = BlueSoft;
            colors[ImGuiCol_SliderGrabActive]      = Blue;

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
            // Note: NavWindowingDimBg / ModalWindowDimBg not set in the original snippet.

            // Unify sizes/roundings/paddings/borders from config
            applyDefaultImGuiStyle(style);

#ifdef IMGUIX_HAS_DOCKING
            colors[ImGuiCol_DockingPreview] = ButtonHovered;
            colors[ImGuiCol_DockingEmptyBg] = WindowBg;
#endif
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            using namespace LightGreenConstants;

            ImPlot::StyleColorsLight(&style);

            ImVec4 frame = FrameBg; frame.w = 1.0f;
            style.Colors[ImPlotCol_FrameBg]        = frame;
            style.Colors[ImPlotCol_PlotBg]         = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]    = Border;
            style.Colors[ImPlotCol_LegendBg]      = PopupBg;
            style.Colors[ImPlotCol_LegendBorder]  = Border;
            style.Colors[ImPlotCol_LegendText]    = Text;

            style.Colors[ImPlotCol_TitleText]     = Text;
            style.Colors[ImPlotCol_InlayText]     = Text;
            style.Colors[ImPlotCol_AxisText]      = Text;

            // Light-ish grid/ticks over bright bg
            style.Colors[ImPlotCol_AxisGrid]       = ImVec4(0.75f, 0.75f, 0.75f, 0.60f);
            style.Colors[ImPlotCol_AxisTick]       = ImVec4(0.40f, 0.40f, 0.40f, 0.90f);
            style.Colors[ImPlotCol_AxisBgHovered]  = ButtonHovered;
            style.Colors[ImPlotCol_AxisBgActive]   = ButtonActive;

            // Use the blue accent for selection/crosshairs to contrast green elements
            style.Colors[ImPlotCol_Selection]     = ImVec4(0.26f, 0.59f, 0.98f, 0.55f);
            style.Colors[ImPlotCol_Crosshairs]    = Blue;

            applyDefaultImPlotStyle(style);
        }
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
        void apply(ImPlot3DStyle& style) const override {
            using namespace LightGreenConstants;

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

    /// \brief Registers the Light-Green theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "light-green").
    inline void registerLightGreenTheme(ThemeManager& tm, std::string id = "light-green") {
        tm.registerTheme(std::move(id), std::make_unique<LightGreenTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_LIGHT_GREEN_THEME_HPP_INCLUDED
