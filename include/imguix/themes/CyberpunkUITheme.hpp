#pragma once
#ifndef _IMGUIX_THEMES_CYBERPUNK_UI_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_CYBERPUNK_UI_THEME_HPP_INCLUDED

/// \file CyberpunkUITheme.hpp
/// \brief HUD–blue monochrome (Cyberpunk UI) theme for ImGui/ImPlot, integrated with the ImGuiX theme system.
///
/// Aesthetics:
///  - very dark tech base (#0A0F14 / #0E141C)
///  - single-hue neon cyan/azure accents (HUD look), no magenta/lime
///  - high-contrast hovers/actives, subtle glass panels
///
/// Notes:
///  - repeated colors are grouped in CyberpunkUIConstants
///  - layout/rounding is unified via applyDefaultImGuiStyle
///  - includes optional ImPlot/ImPlot3D styling

#include <imguix/core/themes/Theme.hpp> // Theme + applyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace CyberpunkUIConstants
    /// \brief Color constants for the Cyberpunk UI theme.
    namespace CyberpunkUIConstants {
        // Base palette (hex reference):
        // bg0: #0A0F14, bg1: #0E141C, border glow: cyan @ ~35%
        // text: #E6F1FF, cyan: #00E5FF, cyan-active: #0090FF

        // Text
        constexpr ImVec4 Text                   = ImVec4(0.902f, 0.945f, 1.000f, 1.000f); // #E6F1FF
        constexpr ImVec4 TextDisabled           = ImVec4(0.902f, 0.945f, 1.000f, 0.450f);

        // Backgrounds
        constexpr ImVec4 WindowBg               = ImVec4(0.039f, 0.059f, 0.078f, 1.000f); // #0A0F14
        constexpr ImVec4 ChildBg                = ImVec4(0.055f, 0.078f, 0.110f, 0.960f); // #0E141C
        constexpr ImVec4 PopupBg                = ImVec4(0.055f, 0.078f, 0.110f, 0.980f); // #0E141C

        // Borders
        constexpr ImVec4 Border                 = ImVec4(0.000f, 0.898f, 1.000f, 0.350f); // cyan @ 35%
        constexpr ImVec4 BorderShadow           = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);

        // Frames
        constexpr ImVec4 FrameBg                = ImVec4(0.055f, 0.078f, 0.110f, 0.900f); // bg1
        constexpr ImVec4 FrameBgHovered         = ImVec4(0.000f, 0.898f, 1.000f, 0.220f); // cyan veil
        constexpr ImVec4 FrameBgActive          = ImVec4(0.000f, 0.564f, 1.000f, 0.350f); // #0090FF @ 35%

        // Titles / Menu
        constexpr ImVec4 TitleBg                = ImVec4(0.055f, 0.078f, 0.110f, 0.950f);
        constexpr ImVec4 TitleBgCollapsed       = ImVec4(0.055f, 0.078f, 0.110f, 0.750f);
        constexpr ImVec4 TitleBgActive          = ImVec4(0.000f, 0.898f, 1.000f, 0.200f);
        constexpr ImVec4 MenuBarBg              = ImVec4(0.055f, 0.078f, 0.110f, 1.000f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg            = ImVec4(0.055f, 0.078f, 0.110f, 0.850f);
        constexpr ImVec4 ScrollbarGrab          = ImVec4(0.000f, 0.898f, 1.000f, 0.450f);
        constexpr ImVec4 ScrollbarGrabHovered   = ImVec4(0.000f, 0.898f, 1.000f, 0.700f);
        constexpr ImVec4 ScrollbarGrabActive    = ImVec4(0.000f, 0.564f, 1.000f, 0.900f); // active cyan

        // Accents (single hue)
        constexpr ImVec4 Cyan                   = ImVec4(0.000f, 0.898f, 1.000f, 1.000f); // #00E5FF
        constexpr ImVec4 CyanActive             = ImVec4(0.000f, 0.564f, 1.000f, 1.000f); // #0090FF

        // Buttons / Headers
        constexpr ImVec4 Button                 = ImVec4(0.000f, 0.898f, 1.000f, 0.550f);
        constexpr ImVec4 ButtonHovered          = ImVec4(0.000f, 0.898f, 1.000f, 0.850f);
        constexpr ImVec4 ButtonActive           = ImVec4(0.000f, 0.564f, 1.000f, 1.000f);

        constexpr ImVec4 Header                 = ImVec4(0.000f, 0.898f, 1.000f, 0.300f);
        constexpr ImVec4 HeaderHovered          = ImVec4(0.000f, 0.898f, 1.000f, 0.450f);
        constexpr ImVec4 HeaderActive           = ImVec4(0.000f, 0.564f, 1.000f, 0.650f);

        // Separators / grips
        constexpr ImVec4 Separator              = ImVec4(0.000f, 0.898f, 1.000f, 0.350f);
        constexpr ImVec4 SeparatorHovered       = ImVec4(0.000f, 0.898f, 1.000f, 0.750f);
        constexpr ImVec4 SeparatorActive        = ImVec4(0.000f, 0.564f, 1.000f, 1.000f);

        constexpr ImVec4 ResizeGrip             = ImVec4(0.000f, 0.898f, 1.000f, 0.400f);
        constexpr ImVec4 ResizeGripHovered      = ImVec4(0.000f, 0.898f, 1.000f, 0.700f);
        constexpr ImVec4 ResizeGripActive       = ImVec4(0.000f, 0.564f, 1.000f, 0.900f);

        // Tabs
        constexpr ImVec4 Tab                    = ImVec4(0.000f, 0.898f, 1.000f, 0.550f);
        constexpr ImVec4 TabHovered             = ImVec4(0.000f, 0.898f, 1.000f, 0.850f);
        constexpr ImVec4 TabActive              = ImVec4(0.000f, 0.564f, 1.000f, 0.900f); // #0090FF @ 90%
        constexpr ImVec4 TabUnfocused           = ImVec4(0.055f, 0.078f, 0.110f, 0.900f);
        constexpr ImVec4 TabUnfocusedActive     = ImVec4(0.000f, 0.898f, 1.000f, 0.700f);

        // Plots
        constexpr ImVec4 PlotLines              = ImVec4(0.000f, 0.898f, 1.000f, 1.000f);
        constexpr ImVec4 PlotLinesHovered       = ImVec4(0.000f, 0.564f, 1.000f, 1.000f);
        constexpr ImVec4 PlotHistogram          = ImVec4(0.000f, 0.898f, 1.000f, 1.000f);
        constexpr ImVec4 PlotHistogramHovered   = ImVec4(0.000f, 0.564f, 1.000f, 1.000f);

        // Misc / Navigation
        constexpr ImVec4 TextSelectedBg         = ImVec4(0.000f, 0.898f, 1.000f, 0.350f);
        constexpr ImVec4 DragDropTarget         = ImVec4(0.000f, 0.898f, 1.000f, 0.950f);
        constexpr ImVec4 NavHighlight           = HeaderHovered;
        constexpr ImVec4 NavWindowingHighlight  = ImVec4(0.700f, 0.700f, 0.700f, 0.700f);
    } // namespace CyberpunkUIConstants

    /// \class CyberpunkUITheme
    /// \brief Dark neon mono-cyan theme (HUD look).
    class CyberpunkUITheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace CyberpunkUIConstants;
            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                  = Text;
            colors[ImGuiCol_TextCursor]            = Cyan;
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

            colors[ImGuiCol_CheckMark]             = CyanActive;
            colors[ImGuiCol_SliderGrab]            = Cyan;
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
            colors[ImGuiCol_TabActive]             = TabActive; // deeper cyan for active tabs
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

            // Baseline sizes/roundings from config
            applyDefaultImGuiStyle(style);

#if IMGUIX_HAS_DOCKING
            colors[ImGuiCol_DockingPreview] = Cyan;
            colors[ImGuiCol_DockingEmptyBg] = WindowBg;
#endif
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            using namespace CyberpunkUIConstants;

            ImPlot::StyleColorsDark(&style);

            // Use opaque frame based on our Child/Frame background
            ImVec4 frame = FrameBg; frame.w = 1.0f;
            style.Colors[ImPlotCol_FrameBg]        = frame;
            style.Colors[ImPlotCol_PlotBg]         = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]     = Border;

            style.Colors[ImPlotCol_LegendBg]       = frame;
            style.Colors[ImPlotCol_LegendBorder]   = Border;
            style.Colors[ImPlotCol_LegendText]     = Text;

            style.Colors[ImPlotCol_TitleText]      = Text;
            style.Colors[ImPlotCol_InlayText]      = Text;
            style.Colors[ImPlotCol_AxisText]       = Text;

            // Grid/ticks over dark bg
            style.Colors[ImPlotCol_AxisGrid]       = ImVec4(0.164f, 0.204f, 0.251f, 0.60f); // #2A3440
            style.Colors[ImPlotCol_AxisTick]       = ImVec4(0.902f, 0.945f, 1.000f, 0.90f); // Text @ 0.9
            style.Colors[ImPlotCol_AxisBgHovered]  = ButtonHovered;
            style.Colors[ImPlotCol_AxisBgActive]   = ButtonActive;

            // Accents
            style.Colors[ImPlotCol_Selection]      = ImVec4(0.000f, 0.898f, 1.000f, 0.55f);
            style.Colors[ImPlotCol_Crosshairs]     = ImVec4(0.000f, 0.898f, 1.000f, 1.00f);

            applyDefaultImPlotStyle(style);
        }
#endif

#ifdef IMGUI_ENABLE_IMPLOT3D
        void apply(ImPlot3DStyle& style) const override {
            using namespace CyberpunkUIConstants;

            ImPlot3D::StyleColorsDark(&style);

            ImVec4 frame = FrameBg; frame.w = 1.0f;
            style.Colors[ImPlot3DCol_FrameBg]      = frame;
            style.Colors[ImPlot3DCol_PlotBg]       = WindowBg;
            style.Colors[ImPlot3DCol_PlotBorder]   = Border;
            style.Colors[ImPlot3DCol_LegendBg]     = frame;
            style.Colors[ImPlot3DCol_LegendBorder] = Border;
            style.Colors[ImPlot3DCol_LegendText]   = Text;

            style.Colors[ImPlot3DCol_TitleText]    = Text;
            style.Colors[ImPlot3DCol_InlayText]    = Text;
            style.Colors[ImPlot3DCol_AxisText]     = Text;

            style.Colors[ImPlot3DCol_AxisGrid]     = ImVec4(0.164f, 0.204f, 0.251f, 0.60f);
            style.Colors[ImPlot3DCol_AxisTick]     = ImVec4(0.902f, 0.945f, 1.000f, 0.90f);

            applyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Registers the Cyberpunk UI theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "cyberpunk-ui").
    inline void registerCyberpunkUITheme(ThemeManager& tm, std::string id = "cyberpunk-ui") {
        tm.registerTheme(std::move(id), std::make_unique<CyberpunkUITheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_CYBERPUNK_UI_THEME_HPP_INCLUDED
