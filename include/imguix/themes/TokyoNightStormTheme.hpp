#pragma once
#ifndef _IMGUIX_THEMES_TOKYO_NIGHT_STORM_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_TOKYO_NIGHT_STORM_THEME_HPP_INCLUDED

/// \file TokyoNightStormTheme.hpp
/// \brief Tokyo Night — Storm variant for ImGui/ImPlot.
///
/// Aesthetics:
///  - stormy navy bg (#24283B), panels #1F2335, popups #1A1B26
///  - soft light text (#C0CAF5), muted comments (#565F89)
///  - blue/cyan primaries (#7AA2F7/#7DCFFF), purple/orange/yellow accents
///
/// Notes:
///  - constants grouped in TokyoNightStormConstants
///  - sizing unified via applyDefaultImGuiStyle/ImPlot/ImPlot3D

#include <imguix/core/themes/Theme.hpp>

namespace ImGuiX::Themes {

namespace TokyoNightStormConstants {
    // Text
    constexpr ImVec4 Text                 = ImVec4(0.753f, 0.792f, 0.961f, 1.000f); // #C0CAF5
    constexpr ImVec4 TextDisabled         = ImVec4(0.337f, 0.373f, 0.537f, 1.000f); // #565F89

    // Backgrounds
    constexpr ImVec4 WindowBg             = ImVec4(0.141f, 0.157f, 0.231f, 1.000f); // #24283B
    constexpr ImVec4 ChildBg              = ImVec4(0.122f, 0.137f, 0.208f, 0.980f); // #1F2335 @98%
    constexpr ImVec4 PopupBg              = ImVec4(0.102f, 0.106f, 0.149f, 0.980f); // #1A1B26 @98%

    // Borders / separators
    constexpr ImVec4 Border               = ImVec4(0.231f, 0.259f, 0.380f, 0.90f);  // #3B4261
    constexpr ImVec4 BorderShadow         = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);
    constexpr ImVec4 Separator            = ImVec4(0.231f, 0.259f, 0.380f, 1.00f);
    constexpr ImVec4 SeparatorHovered     = ImVec4(0.478f, 0.635f, 0.969f, 0.55f);  // #7AA2F7
    constexpr ImVec4 SeparatorActive      = ImVec4(0.490f, 0.812f, 1.000f, 0.70f);  // #7DCFFF

    // Titles / Menu
    constexpr ImVec4 TitleBg              = ImVec4(0.122f, 0.137f, 0.208f, 1.000f); // #1F2335
    constexpr ImVec4 TitleBgCollapsed     = TitleBg;
    constexpr ImVec4 TitleBgActive        = ImVec4(0.231f, 0.259f, 0.380f, 1.000f); // #3B4261
    constexpr ImVec4 MenuBarBg            = TitleBg;

    // Accents
    constexpr ImVec4 AccentBlue           = ImVec4(0.478f, 0.635f, 0.969f, 1.000f); // #7AA2F7
    constexpr ImVec4 AccentCyan           = ImVec4(0.490f, 0.812f, 1.000f, 1.000f); // #7DCFFF
    constexpr ImVec4 AccentBlueActive     = ImVec4(0.380f, 0.560f, 0.980f, 1.000f);
    constexpr ImVec4 AccentPurple         = ImVec4(0.733f, 0.604f, 0.969f, 1.000f); // #BB9AF7
    constexpr ImVec4 AccentOrange         = ImVec4(1.000f, 0.620f, 0.392f, 1.000f); // #FF9E64
    constexpr ImVec4 AccentYellow         = ImVec4(0.878f, 0.686f, 0.408f, 1.000f); // #E0AF68
    constexpr ImVec4 AccentGreen          = ImVec4(0.620f, 0.808f, 0.416f, 1.000f); // #9ECE6A
    constexpr ImVec4 AccentRed            = ImVec4(0.969f, 0.463f, 0.557f, 1.000f); // #F7768E

    // Frames / inputs
    constexpr ImVec4 FrameBg              = ImVec4(0.231f, 0.259f, 0.380f, 0.80f);
    constexpr ImVec4 FrameBgHovered       = ImVec4(0.478f, 0.635f, 0.969f, 0.25f);
    constexpr ImVec4 FrameBgActive        = ImVec4(0.490f, 0.812f, 1.000f, 0.35f);

    // Buttons
    constexpr ImVec4 Button               = ImVec4(0.478f, 0.635f, 0.969f, 0.28f);
    constexpr ImVec4 ButtonHovered        = ImVec4(0.478f, 0.635f, 0.969f, 0.44f);
    constexpr ImVec4 ButtonActive         = ImVec4(0.490f, 0.812f, 1.000f, 0.55f);

    // Sliders / checks
    constexpr ImVec4 CheckMark            = AccentCyan;
    constexpr ImVec4 SliderGrab           = AccentBlue;
    constexpr ImVec4 SliderGrabActive     = AccentBlueActive;

    // Headers / Tabs
    constexpr ImVec4 Header               = ImVec4(0.114f, 0.160f, 0.267f, 0.75f);
    constexpr ImVec4 HeaderHovered        = ImVec4(0.114f, 0.160f, 0.267f, 0.95f);
    constexpr ImVec4 HeaderActive         = ImVec4(0.102f, 0.106f, 0.149f, 1.000f); // #1A1B26

    constexpr ImVec4 Tab                  = ImVec4(0.114f, 0.160f, 0.267f, 0.90f);
    constexpr ImVec4 TabHovered           = AccentBlue;
    constexpr ImVec4 TabActive            = ImVec4(0.478f, 0.635f, 0.969f, 0.60f);
    constexpr ImVec4 TabUnfocused         = ImVec4(0.122f, 0.137f, 0.208f, 0.80f);
    constexpr ImVec4 TabUnfocusedActive   = ImVec4(0.478f, 0.635f, 0.969f, 0.45f);

    // Scrollbar
    constexpr ImVec4 ScrollbarBg          = ImVec4(0.141f, 0.157f, 0.231f, 0.85f);
    constexpr ImVec4 ScrollbarGrab        = ImVec4(0.231f, 0.259f, 0.380f, 0.85f);
    constexpr ImVec4 ScrollbarGrabHovered = ImVec4(0.231f, 0.259f, 0.380f, 1.00f);
    constexpr ImVec4 ScrollbarGrabActive  = AccentBlueActive;

    // Resize grips
    constexpr ImVec4 ResizeGrip           = ImVec4(0.478f, 0.635f, 0.969f, 0.25f);
    constexpr ImVec4 ResizeGripHovered    = ImVec4(0.478f, 0.635f, 0.969f, 0.45f);
    constexpr ImVec4 ResizeGripActive     = ImVec4(0.490f, 0.812f, 1.000f, 0.70f);

    // Plots
    constexpr ImVec4 PlotLines            = AccentBlue;
    constexpr ImVec4 PlotLinesHovered     = AccentCyan;
    constexpr ImVec4 PlotHistogram        = AccentOrange;
    constexpr ImVec4 PlotHistogramHovered = AccentPurple;
    constexpr ImVec4 PlotAxisGrid         = ImVec4(Border.x, Border.y, Border.z, 0.55f);
    constexpr ImVec4 PlotAxisTick         = ImVec4(Text.x, Text.y, Text.z, 0.90f);
    constexpr ImVec4 PlotSelection        = ImVec4(0.490f, 0.812f, 1.000f, 0.40f);
    constexpr ImVec4 PlotCrosshairs       = AccentCyan;

    // Tables / misc
    constexpr ImVec4 TableHeaderBg        = TitleBgActive;
    constexpr ImVec4 TableBorderStrong    = ImVec4(Border.x, Border.y, Border.z, 1.00f);
    constexpr ImVec4 TableBorderLight     = ImVec4(Border.x, Border.y, Border.z, 0.50f);
    constexpr ImVec4 TableRowBg           = ImVec4(1.0f, 1.0f, 1.0f, 0.00f);
    constexpr ImVec4 TableRowBgAlt        = ImVec4(1.0f, 1.0f, 1.0f, 0.04f);

    constexpr ImVec4 TextSelectedBg       = ImVec4(0.231f, 0.259f, 0.380f, 0.85f);
    constexpr ImVec4 DragDropTarget       = AccentCyan;
    constexpr ImVec4 NavHighlight         = HeaderHovered;
    constexpr ImVec4 NavWindowingHighlight= ImVec4(0.733f, 0.604f, 0.969f, 0.700f);
    constexpr ImVec4 NavWindowingDimBg    = ImVec4(0.000f, 0.000f, 0.000f, 0.30f);
    constexpr ImVec4 ModalWindowDimBg     = ImVec4(0.000f, 0.000f, 0.000f, 0.45f);
} // namespace TokyoNightStormConstants

class TokyoNightStormTheme final : public Theme {
public:
    /// \brief Apply theme colors to ImGui style.
    /// \param style Target style.
    void apply(ImGuiStyle& style) const override {
        using namespace TokyoNightStormConstants;
        ImVec4* c = style.Colors;

        c[ImGuiCol_Text]                  = Text;
        c[ImGuiCol_InputTextCursor]            = AccentCyan;
        c[ImGuiCol_TextDisabled]          = TextDisabled;
        c[ImGuiCol_WindowBg]              = WindowBg;
        c[ImGuiCol_ChildBg]               = ChildBg;
        c[ImGuiCol_PopupBg]               = PopupBg;

        c[ImGuiCol_Border]                = Border;
        c[ImGuiCol_BorderShadow]          = BorderShadow;

        c[ImGuiCol_FrameBg]               = FrameBg;
        c[ImGuiCol_FrameBgHovered]        = FrameBgHovered;
        c[ImGuiCol_FrameBgActive]         = FrameBgActive;

        c[ImGuiCol_TitleBg]               = TitleBg;
        c[ImGuiCol_TitleBgCollapsed]      = TitleBgCollapsed;
        c[ImGuiCol_TitleBgActive]         = TitleBgActive;
        c[ImGuiCol_MenuBarBg]             = MenuBarBg;

        c[ImGuiCol_ScrollbarBg]           = ScrollbarBg;
        c[ImGuiCol_ScrollbarGrab]         = ScrollbarGrab;
        c[ImGuiCol_ScrollbarGrabHovered]  = ScrollbarGrabHovered;
        c[ImGuiCol_ScrollbarGrabActive]   = ScrollbarGrabActive;

        c[ImGuiCol_CheckMark]             = CheckMark;
        c[ImGuiCol_SliderGrab]            = SliderGrab;
        c[ImGuiCol_SliderGrabActive]      = SliderGrabActive;

        c[ImGuiCol_Button]                = Button;
        c[ImGuiCol_ButtonHovered]         = ButtonHovered;
        c[ImGuiCol_ButtonActive]          = ButtonActive;

        c[ImGuiCol_Header]                = Header;
        c[ImGuiCol_HeaderHovered]         = HeaderHovered;
        c[ImGuiCol_HeaderActive]          = HeaderActive;

        c[ImGuiCol_Separator]             = Separator;
        c[ImGuiCol_SeparatorHovered]      = SeparatorHovered;
        c[ImGuiCol_SeparatorActive]       = SeparatorActive;

        c[ImGuiCol_ResizeGrip]            = ResizeGrip;
        c[ImGuiCol_ResizeGripHovered]     = ResizeGripHovered;
        c[ImGuiCol_ResizeGripActive]      = ResizeGripActive;

        c[ImGuiCol_Tab]                   = Tab;
        c[ImGuiCol_TabHovered]            = TabHovered;
        c[ImGuiCol_TabActive]             = TabActive;
        c[ImGuiCol_TabUnfocused]          = TabUnfocused;
        c[ImGuiCol_TabUnfocusedActive]    = TabUnfocusedActive;

        c[ImGuiCol_PlotLines]             = PlotLines;
        c[ImGuiCol_PlotLinesHovered]      = PlotLinesHovered;
        c[ImGuiCol_PlotHistogram]         = PlotHistogram;
        c[ImGuiCol_PlotHistogramHovered]  = PlotHistogramHovered;

        c[ImGuiCol_TextSelectedBg]        = TextSelectedBg;
        c[ImGuiCol_DragDropTarget]        = DragDropTarget;

        c[ImGuiCol_NavHighlight]          = NavHighlight;
        c[ImGuiCol_NavWindowingHighlight] = NavWindowingHighlight;

        c[ImGuiCol_TableHeaderBg]         = TableHeaderBg;
        c[ImGuiCol_TableBorderStrong]     = TableBorderStrong;
        c[ImGuiCol_TableBorderLight]      = TableBorderLight;
        c[ImGuiCol_TableRowBg]            = TableRowBg;
        c[ImGuiCol_TableRowBgAlt]         = TableRowBgAlt;

        c[ImGuiCol_NavWindowingDimBg]     = NavWindowingDimBg;
        c[ImGuiCol_ModalWindowDimBg]      = ModalWindowDimBg;

#ifdef IMGUIX_HAS_DOCKING
        c[ImGuiCol_DockingPreview] = AccentCyan;
        c[ImGuiCol_DockingEmptyBg] = ImVec4(WindowBg.x, WindowBg.y, WindowBg.z, 1.0f);
#endif

        applyDefaultImGuiStyle(style);
    }

#ifdef IMGUI_ENABLE_IMPLOT
    /// \brief Apply theme colors to ImPlot style.
    /// \param style Target style.
    void apply(ImPlotStyle& style) const override {
        using namespace TokyoNightStormConstants;
        ImPlot::StyleColorsDark(&style);
        ImVec4 frame = ChildBg; frame.w = 1.0f;

        style.Colors[ImPlotCol_FrameBg]      = frame;
        style.Colors[ImPlotCol_PlotBg]       = WindowBg;
        style.Colors[ImPlotCol_PlotBorder]   = Border;

        style.Colors[ImPlotCol_LegendBg]     = PopupBg;
        style.Colors[ImPlotCol_LegendBorder] = Border;
        style.Colors[ImPlotCol_LegendText]   = Text;

        style.Colors[ImPlotCol_TitleText]    = Text;
        style.Colors[ImPlotCol_InlayText]    = Text;
        style.Colors[ImPlotCol_AxisText]     = Text;

        style.Colors[ImPlotCol_AxisGrid]     = PlotAxisGrid;
        style.Colors[ImPlotCol_AxisTick]     = PlotAxisTick;
        style.Colors[ImPlotCol_AxisBgHovered]= ButtonHovered;
        style.Colors[ImPlotCol_AxisBgActive] = ButtonActive;

        style.Colors[ImPlotCol_Selection]    = PlotSelection;
        style.Colors[ImPlotCol_Crosshairs]   = PlotCrosshairs;

        applyDefaultImPlotStyle(style);
    }
#endif

#ifdef IMGUI_ENABLE_IMPLOT3D
    /// \brief Apply theme colors to ImPlot3D style.
    /// \param style Target style.
    void apply(ImPlot3DStyle& style) const override {
        using namespace TokyoNightStormConstants;
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

/// \brief Register Tokyo Night Storm in ThemeManager.
/// \param tm Theme manager.
/// \param id Unique id (defaults to "tokyo-night-storm").
inline void registerTokyoNightStormTheme(ThemeManager& tm, std::string id = "tokyo-night-storm") {
    tm.registerTheme(std::move(id), std::make_unique<TokyoNightStormTheme>());
}

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_TOKYO_NIGHT_STORM_THEME_HPP_INCLUDED
