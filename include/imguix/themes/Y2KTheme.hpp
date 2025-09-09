#pragma once
#ifndef _IMGUIX_THEMES_Y2K_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_Y2K_THEME_HPP_INCLUDED

/// \file Y2KTheme.hpp
/// \brief Y2K theme for ImGui/ImPlot, integrated with the ImGuiX theme system.
///
/// Aesthetics:
///  - light chrome/silver surfaces (XP-era vibe)
///  - cobalt primary, azure secondary
///  - optional magenta/lime as tertiary accents (mainly for plots)
///
/// Notes:
///  - repeated colors are grouped in Y2KConstants
///  - layout/rounding is unified via ApplyDefaultImGuiStyle
///  - includes optional ImPlot/ImPlot3D styling

#include <imguix/core/themes/Theme.hpp> // Theme + ApplyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace Y2KConstants
    /// \brief Color constants for the Y2K theme.
    namespace Y2KConstants {
        // Text
        constexpr ImVec4 Text                   = ImVec4(0.070f, 0.090f, 0.140f, 1.000f);  // #121729
        constexpr ImVec4 TextDisabled           = ImVec4(0.420f, 0.470f, 0.540f, 1.000f);

        // Backgrounds (neutral, low-blue)
        constexpr ImVec4 WindowBg               = ImVec4(0.945f, 0.957f, 0.980f, 1.000f);  // #F1F4FA
        constexpr ImVec4 ChildBg                = ImVec4(0.965f, 0.973f, 0.988f, 0.920f);  // #F6F8FC @92%
        constexpr ImVec4 PopupBg                = ImVec4(0.965f, 0.973f, 0.988f, 0.980f);

        // Borders / separators
        constexpr ImVec4 Border                 = ImVec4(0.718f, 0.773f, 0.816f, 0.90f);   // #B7C5D0
        constexpr ImVec4 BorderShadow           = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);
        constexpr ImVec4 Separator              = ImVec4(0.718f, 0.773f, 0.816f, 1.00f);
        constexpr ImVec4 SeparatorHovered       = ImVec4(0.000f, 0.682f, 0.937f, 0.45f);   // azure @45%
        constexpr ImVec4 SeparatorActive        = ImVec4(0.000f, 0.682f, 0.937f, 0.75f);

        // Titles / Menu
        constexpr ImVec4 TitleBg                = ImVec4(0.945f, 0.957f, 0.980f, 1.000f);
        constexpr ImVec4 TitleBgCollapsed       = ImVec4(0.945f, 0.957f, 0.980f, 1.000f);
        constexpr ImVec4 TitleBgActive          = ImVec4(0.867f, 0.914f, 1.000f, 1.000f);  // #DDE9FF
        constexpr ImVec4 MenuBarBg              = ImVec4(0.930f, 0.945f, 0.969f, 1.000f);

        // Accents
        constexpr ImVec4 AccentPrimary          = ImVec4(0.231f, 0.510f, 0.965f, 1.000f); // #3B82F6
        constexpr ImVec4 AccentPrimaryHover     = ImVec4(0.000f, 0.467f, 1.000f, 0.85f);  // #0077FF
        constexpr ImVec4 AccentPrimaryActive    = ImVec4(0.000f, 0.396f, 0.925f, 1.000f); // #0065EC
        constexpr ImVec4 AccentSecondary        = ImVec4(0.000f, 0.682f, 0.937f, 1.000f); // #00AEEF

        // (tertiary, used mainly in plots)
        constexpr ImVec4 AccentMagenta          = ImVec4(1.000f, 0.000f, 0.800f, 1.000f); // #FF00CC
        constexpr ImVec4 AccentLime             = ImVec4(0.725f, 1.000f, 0.000f, 1.000f); // #B9FF00

        // Frames (affects Combo/Input)
        constexpr ImVec4 FrameBg                = ImVec4(0.953f, 0.961f, 0.973f, 0.98f);
        constexpr ImVec4 FrameBgHovered         = ImVec4(0.231f, 0.510f, 0.965f, 0.18f);
        constexpr ImVec4 FrameBgActive          = ImVec4(0.000f, 0.396f, 0.925f, 0.32f);

        // Buttons
        constexpr ImVec4 Button                 = ImVec4(0.231f, 0.510f, 0.965f, 0.45f);
        constexpr ImVec4 ButtonHovered          = AccentPrimaryHover;
        constexpr ImVec4 ButtonActive           = AccentPrimaryActive;

        // Sliders / checks
        constexpr ImVec4 CheckMark              = AccentPrimaryActive;
        constexpr ImVec4 SliderGrab             = AccentPrimary;
        constexpr ImVec4 SliderGrabActive       = AccentPrimaryActive;

        // Headers / Tabs
        constexpr ImVec4 Header                 = ImVec4(0.231f, 0.510f, 0.965f, 0.18f);
        constexpr ImVec4 HeaderHovered          = ImVec4(0.231f, 0.510f, 0.965f, 0.28f);
        constexpr ImVec4 HeaderActive           = ImVec4(0.000f, 0.396f, 0.925f, 0.40f);

        constexpr ImVec4 Tab                    = ImVec4(0.231f, 0.510f, 0.965f, 0.55f);
        constexpr ImVec4 TabHovered             = AccentPrimaryHover;
        constexpr ImVec4 TabActive              = ImVec4(0.231f, 0.510f, 0.965f, 0.80f);
        constexpr ImVec4 TabUnfocused           = ImVec4(0.827f, 0.855f, 0.890f, 0.900f);
        constexpr ImVec4 TabUnfocusedActive     = ImVec4(0.000f, 0.400f, 1.000f, 0.700f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg            = ImVec4(0.945f, 0.957f, 0.980f, 0.85f);
        constexpr ImVec4 ScrollbarGrab          = ImVec4(0.231f, 0.510f, 0.965f, 0.35f);
        constexpr ImVec4 ScrollbarGrabHovered   = ImVec4(0.231f, 0.510f, 0.965f, 0.55f);
        constexpr ImVec4 ScrollbarGrabActive    = ImVec4(0.000f, 0.396f, 0.925f, 0.80f);

        // Resize grips
        constexpr ImVec4 ResizeGrip             = ImVec4(0.000f, 0.400f, 1.000f, 0.40f);
        constexpr ImVec4 ResizeGripHovered      = ImVec4(0.000f, 0.400f, 1.000f, 0.70f);
        constexpr ImVec4 ResizeGripActive       = AccentSecondary;

        // Plots
        constexpr ImVec4 PlotLines              = ImVec4(0.000f, 0.400f, 1.000f, 1.000f); // cobalt
        constexpr ImVec4 PlotLinesHovered       = AccentSecondary;                         // azure
        constexpr ImVec4 PlotHistogram          = AccentLime;
        constexpr ImVec4 PlotHistogramHovered   = AccentMagenta;
        constexpr ImVec4 PlotAxisGrid           = ImVec4(Border.x, Border.y, Border.z, 0.60f);
        constexpr ImVec4 PlotAxisTick           = ImVec4(Text.x,   Text.y,   Text.z,   0.90f);
        constexpr ImVec4 PlotSelection          = ImVec4(0.000f, 0.400f, 1.000f, 0.55f);
        constexpr ImVec4 PlotCrosshairs         = ImVec4(0.000f, 0.400f, 1.000f, 1.00f);

        // Tables
        constexpr ImVec4 TableBorderStrong      = ImVec4(Border.x, Border.y, Border.z, 1.00f);
        constexpr ImVec4 TableBorderLight       = ImVec4(Border.x, Border.y, Border.z, 0.50f);
        constexpr ImVec4 TableRowBg             = ImVec4(1.000f, 1.000f, 1.000f, 0.00f);
        constexpr ImVec4 TableRowBgAlt          = ImVec4(0.000f, 0.000f, 0.000f, 0.03f);

        // Misc / Navigation
        constexpr ImVec4 TextSelectedBg         = ImVec4(0.000f, 0.400f, 1.000f, 0.350f);
        constexpr ImVec4 DragDropTarget         = ImVec4(0.000f, 0.400f, 1.000f, 0.950f);
        constexpr ImVec4 NavHighlight           = HeaderHovered;
        constexpr ImVec4 NavWindowingHighlight  = ImVec4(0.700f, 0.700f, 0.700f, 0.700f);
        constexpr ImVec4 NavWindowingDimBg      = ImVec4(0.000f, 0.000f, 0.000f, 0.20f);
        constexpr ImVec4 ModalWindowDimBg       = ImVec4(0.000f, 0.000f, 0.000f, 0.35f);
    } // namespace Y2KConstants

    /// \class Y2KTheme
    /// \brief Light Y2K theme with cobalt/azure accents and glassy panels.
    class Y2KTheme final : public Theme {
    public:
        /// \brief Apply theme colors to ImGui style.
        /// \param style Target style.
        void apply(ImGuiStyle& style) const override {
            using namespace Y2KConstants;
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

            colors[ImGuiCol_TableHeaderBg]         = TitleBgActive;
            colors[ImGuiCol_TableBorderStrong]     = TableBorderStrong;
            colors[ImGuiCol_TableBorderLight]      = TableBorderLight;

            colors[ImGuiCol_TableRowBg]            = TableRowBg;
            colors[ImGuiCol_TableRowBgAlt]         = TableRowBgAlt;

            colors[ImGuiCol_NavWindowingDimBg]     = NavWindowingDimBg;
            colors[ImGuiCol_ModalWindowDimBg]      = ModalWindowDimBg;
            
#ifdef IMGUIX_HAS_DOCKING
            colors[ImGuiCol_DockingPreview] = AccentSecondary;
            colors[ImGuiCol_DockingEmptyBg] = ImVec4(
                WindowBg.x,
                WindowBg.y,
                WindowBg.z, 1.0f
            );
#endif

            // Unify sizes/roundings/paddings/borders from config
            ApplyDefaultImGuiStyle(style);
        }

#ifdef IMGUI_ENABLE_IMPLOT
        /// \brief Apply theme colors to ImPlot style.
        /// \param style Target style.
        void apply(ImPlotStyle& style) const override {
            using namespace Y2KConstants;

            ImPlot::StyleColorsLight(&style);

            ImVec4 frame = ChildBg; frame.w = 1.0f;
            style.Colors[ImPlotCol_FrameBg]        = frame;
            style.Colors[ImPlotCol_PlotBg]         = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]     = Border;
            
            style.Colors[ImPlotCol_LegendBg]       = PopupBg;
            style.Colors[ImPlotCol_LegendBorder]   = Border;
            style.Colors[ImPlotCol_LegendText]     = Text;

            style.Colors[ImPlotCol_TitleText]      = Text;
            style.Colors[ImPlotCol_InlayText]      = Text;
            style.Colors[ImPlotCol_AxisText]       = Text;

            style.Colors[ImPlotCol_AxisGrid]       = PlotAxisGrid;
            style.Colors[ImPlotCol_AxisTick]       = PlotAxisTick;
            
            style.Colors[ImPlotCol_AxisBgHovered]  = ButtonHovered;
            style.Colors[ImPlotCol_AxisBgActive]   = ButtonActive;

            style.Colors[ImPlotCol_Selection]      = PlotSelection;
            style.Colors[ImPlotCol_Crosshairs]     = PlotCrosshairs;

            ApplyDefaultImPlotStyle(style);
        }
#endif

#ifdef IMGUI_ENABLE_IMPLOT3D
        /// \brief Apply theme colors to ImPlot3D style.
        /// \param style Target style.
        void apply(ImPlot3DStyle& style) const override {
            using namespace Y2KConstants;

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

            style.Colors[ImPlot3DCol_AxisGrid]     = PlotAxisGrid;
            style.Colors[ImPlot3DCol_AxisTick]     = PlotAxisTick;

            ApplyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Registers the Y2K theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "y2k").
    inline void registerY2KTheme(ThemeManager& tm, std::string id = "y2k") {
        tm.registerTheme(std::move(id), std::make_unique<Y2KTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_Y2K_THEME_HPP_INCLUDED
