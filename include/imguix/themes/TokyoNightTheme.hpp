#pragma once
#ifndef _IMGUIX_THEMES_TOKYO_NIGHT_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_TOKYO_NIGHT_THEME_HPP_INCLUDED

/// \file TokyoNightTheme.hpp
/// \brief Tokyo Night (Storm) inspired dark theme for ImGui/ImPlot.
///
/// Aesthetics:
///  - stormy navy background (#24283B)
///  - soft light text (#C0CAF5)
///  - blue/cyan primaries (#7AA2F7 / #7DCFFF), purple/orange/yellow accents
///
/// Notes:
///  - repeated colors are grouped in TokyoNightConstants
///  - layout/rounding is unified via applyDefaultImGuiStyle
///  - includes optional ImPlot/ImPlot3D styling

#include <imguix/core/themes/Theme.hpp>

namespace ImGuiX::Themes {

    /// \namespace TokyoNightConstants
    /// \brief Color constants for the Tokyo Night theme (Storm variant).
    namespace TokyoNightConstants {
        // Text
        constexpr ImVec4 Text                   = ImVec4(0.753f, 0.792f, 0.961f, 1.000f); // #C0CAF5
        constexpr ImVec4 TextDisabled           = ImVec4(0.337f, 0.373f, 0.537f, 1.000f); // #565F89

        // Backgrounds
        constexpr ImVec4 WindowBg               = ImVec4(0.141f, 0.157f, 0.231f, 1.000f); // #24283B
        constexpr ImVec4 ChildBg                = ImVec4(0.122f, 0.137f, 0.208f, 0.980f); // #1F2335 @98%
        constexpr ImVec4 PopupBg                = ImVec4(0.102f, 0.106f, 0.149f, 0.980f); // #1A1B26 @98%

        // Borders / separators
        constexpr ImVec4 Border                 = ImVec4(0.231f, 0.259f, 0.380f, 0.90f);  // #3B4261
        constexpr ImVec4 BorderShadow           = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);
        constexpr ImVec4 Separator              = ImVec4(0.231f, 0.259f, 0.380f, 1.00f);
        constexpr ImVec4 SeparatorHovered       = ImVec4(0.478f, 0.635f, 0.969f, 0.55f);  // #7AA2F7 @55%
        constexpr ImVec4 SeparatorActive        = ImVec4(0.490f, 0.812f, 1.000f, 0.70f);  // #7DCFFF @70%

        // Titles / Menu
        constexpr ImVec4 TitleBg                = ImVec4(0.122f, 0.137f, 0.208f, 1.000f); // #1F2335
        constexpr ImVec4 TitleBgCollapsed       = ImVec4(0.122f, 0.137f, 0.208f, 1.000f);
        constexpr ImVec4 TitleBgActive          = ImVec4(0.231f, 0.259f, 0.380f, 1.000f); // #3B4261
        constexpr ImVec4 MenuBarBg              = ImVec4(0.122f, 0.137f, 0.208f, 1.000f);

        // Accents
        constexpr ImVec4 AccentPrimary          = ImVec4(0.478f, 0.635f, 0.969f, 1.000f); // #7AA2F7
        constexpr ImVec4 AccentPrimaryHover     = ImVec4(0.600f, 0.740f, 1.000f, 1.000f);
        constexpr ImVec4 AccentPrimaryActive    = ImVec4(0.380f, 0.560f, 0.980f, 1.000f);
        constexpr ImVec4 AccentCyan             = ImVec4(0.490f, 0.812f, 1.000f, 1.000f); // #7DCFFF
        constexpr ImVec4 AccentPurple           = ImVec4(0.733f, 0.604f, 0.969f, 1.000f); // #BB9AF7
        constexpr ImVec4 AccentOrange           = ImVec4(1.000f, 0.620f, 0.392f, 1.000f); // #FF9E64
        constexpr ImVec4 AccentYellow           = ImVec4(0.878f, 0.686f, 0.408f, 1.000f); // #E0AF68
        constexpr ImVec4 AccentGreen            = ImVec4(0.620f, 0.808f, 0.416f, 1.000f); // #9ECE6A
        constexpr ImVec4 AccentRed              = ImVec4(0.969f, 0.463f, 0.557f, 1.000f); // #F7768E

        // Frames (inputs/combo)
        constexpr ImVec4 FrameBg                = ImVec4(0.231f, 0.259f, 0.380f, 0.80f);  // #3B4261
        constexpr ImVec4 FrameBgHovered         = ImVec4(0.478f, 0.635f, 0.969f, 0.25f);
        constexpr ImVec4 FrameBgActive          = ImVec4(0.490f, 0.812f, 1.000f, 0.35f);

        // Buttons
        constexpr ImVec4 Button                 = ImVec4(0.478f, 0.635f, 0.969f, 0.28f);
        constexpr ImVec4 ButtonHovered          = ImVec4(0.478f, 0.635f, 0.969f, 0.44f);
        constexpr ImVec4 ButtonActive           = ImVec4(0.490f, 0.812f, 1.000f, 0.55f);

        // Sliders / checks
        constexpr ImVec4 CheckMark              = AccentCyan;
        constexpr ImVec4 SliderGrab             = AccentPrimary;
        constexpr ImVec4 SliderGrabActive       = AccentPrimaryActive;

        // Headers / Tabs
        constexpr ImVec4 Header                 = ImVec4(0.114f, 0.160f, 0.267f, 0.75f);  // ~mix
        constexpr ImVec4 HeaderHovered          = ImVec4(0.114f, 0.160f, 0.267f, 0.95f);
        constexpr ImVec4 HeaderActive           = ImVec4(0.102f, 0.106f, 0.149f, 1.000f); // #1A1B26

        constexpr ImVec4 Tab                    = ImVec4(0.114f, 0.160f, 0.267f, 0.90f);
        constexpr ImVec4 TabHovered             = AccentPrimaryHover;
        constexpr ImVec4 TabActive              = ImVec4(0.478f, 0.635f, 0.969f, 0.60f);
        constexpr ImVec4 TabUnfocused           = ImVec4(0.122f, 0.137f, 0.208f, 0.80f);
        constexpr ImVec4 TabUnfocusedActive     = ImVec4(0.478f, 0.635f, 0.969f, 0.45f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg            = ImVec4(0.141f, 0.157f, 0.231f, 0.85f);
        constexpr ImVec4 ScrollbarGrab          = ImVec4(0.231f, 0.259f, 0.380f, 0.85f);
        constexpr ImVec4 ScrollbarGrabHovered   = ImVec4(0.231f, 0.259f, 0.380f, 1.00f);
        constexpr ImVec4 ScrollbarGrabActive    = ImVec4(0.380f, 0.560f, 0.980f, 1.00f);

        // Resize grips
        constexpr ImVec4 ResizeGrip             = ImVec4(0.478f, 0.635f, 0.969f, 0.25f);
        constexpr ImVec4 ResizeGripHovered      = ImVec4(0.478f, 0.635f, 0.969f, 0.45f);
        constexpr ImVec4 ResizeGripActive       = ImVec4(0.490f, 0.812f, 1.000f, 0.70f);

        // Plots
        constexpr ImVec4 PlotLines              = AccentPrimary;  // blue
        constexpr ImVec4 PlotLinesHovered       = AccentCyan;
        constexpr ImVec4 PlotHistogram          = AccentOrange;
        constexpr ImVec4 PlotHistogramHovered   = AccentPurple;
        constexpr ImVec4 PlotAxisGrid           = ImVec4(Border.x, Border.y, Border.z, 0.55f);
        constexpr ImVec4 PlotAxisTick           = ImVec4(Text.x,   Text.y,   Text.z,   0.90f);
        constexpr ImVec4 PlotSelection          = ImVec4(0.490f, 0.812f, 1.000f, 0.40f);  // cyan @40%
        constexpr ImVec4 PlotCrosshairs         = AccentCyan;

        // Tables
        constexpr ImVec4 TableHeaderBg          = TitleBgActive;
        constexpr ImVec4 TableBorderStrong      = ImVec4(Border.x, Border.y, Border.z, 1.00f);
        constexpr ImVec4 TableBorderLight       = ImVec4(Border.x, Border.y, Border.z, 0.50f);
        constexpr ImVec4 TableRowBg             = ImVec4(1.000f, 1.000f, 1.000f, 0.00f);
        constexpr ImVec4 TableRowBgAlt          = ImVec4(1.000f, 1.000f, 1.000f, 0.04f);

        // Misc / Navigation
        constexpr ImVec4 TextSelectedBg         = ImVec4(0.231f, 0.259f, 0.380f, 0.85f);
        constexpr ImVec4 DragDropTarget         = AccentCyan;
        constexpr ImVec4 NavHighlight           = HeaderHovered;
        constexpr ImVec4 NavWindowingHighlight  = ImVec4(0.733f, 0.604f, 0.969f, 0.700f); // purple glow
        constexpr ImVec4 NavWindowingDimBg      = ImVec4(0.000f, 0.000f, 0.000f, 0.30f);
        constexpr ImVec4 ModalWindowDimBg       = ImVec4(0.000f, 0.000f, 0.000f, 0.45f);
    } // namespace TokyoNightConstants

    /// \class TokyoNightTheme
    /// \brief Tokyo Night (Storm) theme with blue/cyan primaries and balanced contrast.
    class TokyoNightTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace TokyoNightConstants;
            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                  = Text;
            colors[ImGuiCol_InputTextCursor]            = AccentCyan;
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

            colors[ImGuiCol_TableHeaderBg]         = TableHeaderBg;
            colors[ImGuiCol_TableBorderStrong]     = TableBorderStrong;
            colors[ImGuiCol_TableBorderLight]      = TableBorderLight;

            colors[ImGuiCol_TableRowBg]            = TableRowBg;
            colors[ImGuiCol_TableRowBgAlt]         = TableRowBgAlt;

            colors[ImGuiCol_NavWindowingDimBg]     = NavWindowingDimBg;
            colors[ImGuiCol_ModalWindowDimBg]      = ModalWindowDimBg;

#ifdef IMGUIX_HAS_DOCKING
            colors[ImGuiCol_DockingPreview] = AccentCyan;
            colors[ImGuiCol_DockingEmptyBg] = ImVec4(WindowBg.x, WindowBg.y, WindowBg.z, 1.0f);
#endif
            applyDefaultImGuiStyle(style);
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            using namespace TokyoNightConstants;

            ImPlot::StyleColorsDark(&style);

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

            applyDefaultImPlotStyle(style);
        }
#endif

#ifdef IMGUI_ENABLE_IMPLOT3D
        void apply(ImPlot3DStyle& style) const override {
            using namespace TokyoNightConstants;

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

            style.Colors[ImPlot3DCol_AxisGrid]     = PlotAxisGrid;
            style.Colors[ImPlot3DCol_AxisTick]     = PlotAxisTick;

            applyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Registers the Tokyo Night theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "tokyo-night").
    inline void registerTokyoNightTheme(ThemeManager& tm, std::string id = "tokyo-night") {
        tm.registerTheme(std::move(id), std::make_unique<TokyoNightTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_TOKYO_NIGHT_THEME_HPP_INCLUDED
