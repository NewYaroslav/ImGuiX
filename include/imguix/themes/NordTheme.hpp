#pragma once
#ifndef _IMGUIX_THEMES_NORD_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_NORD_THEME_HPP_INCLUDED

/// @file NordTheme.hpp
/// @brief Nord palette (Arctic Ice Studio) for ImGui/ImPlot.
///
/// Palette:
///  - Polar Night:  nord0 #2E3440, nord1 #3B4252, nord2 #434C5E, nord3 #4C566A
///  - Snow Storm:  nord4 #D8DEE9, nord5 #E5E9F0, nord6 #ECEFF4
///  - Frost:       nord7 #8FBCBB, nord8 #88C0D0, nord9 #81A1C1, nord10 #5E81AC
///  - Aurora:      nord11 #BF616A, nord12 #D08770, nord13 #EBCB8B, nord14 #A3BE8C, nord15 #B48EAD
///
/// Notes:
///  - calm, low-contrast dark with cool blues and soft neutrals
///  - constants grouped in NordConstants

#include <imguix/core/themes/Theme.hpp>

namespace ImGuiX::Themes {

namespace NordConstants {
    // Text
    constexpr ImVec4 Text                 = ImVec4(0.847f, 0.871f, 0.914f, 1.000f); // #D8DEE9
    constexpr ImVec4 TextDisabled         = ImVec4(0.298f, 0.337f, 0.416f, 1.000f); // #4C566A

    // Backgrounds (Polar Night)
    constexpr ImVec4 WindowBg             = ImVec4(0.180f, 0.204f, 0.251f, 1.000f); // #2E3440
    constexpr ImVec4 ChildBg              = ImVec4(0.231f, 0.259f, 0.322f, 0.980f); // #3B4252 @98%
    constexpr ImVec4 PopupBg              = ImVec4(0.263f, 0.298f, 0.369f, 0.980f); // #434C5E @98%

    // Borders / separators
    constexpr ImVec4 Border               = ImVec4(0.298f, 0.337f, 0.416f, 0.90f);  // #4C566A
    constexpr ImVec4 BorderShadow         = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);
    constexpr ImVec4 Separator            = ImVec4(0.298f, 0.337f, 0.416f, 1.00f);
    constexpr ImVec4 SeparatorHovered     = ImVec4(0.506f, 0.753f, 0.800f, 0.55f);  // #81A1C1 @55%
    constexpr ImVec4 SeparatorActive      = ImVec4(0.345f, 0.506f, 0.675f, 0.70f);  // #5E81AC @70%

    // Titles / menu
    constexpr ImVec4 TitleBg              = ImVec4(0.231f, 0.259f, 0.322f, 1.000f); // #3B4252
    constexpr ImVec4 TitleBgCollapsed     = TitleBg;
    constexpr ImVec4 TitleBgActive        = ImVec4(0.263f, 0.298f, 0.369f, 1.000f); // #434C5E
    constexpr ImVec4 MenuBarBg            = TitleBg;

    // Accents (Frost)
    constexpr ImVec4 AccentPrimary        = ImVec4(0.506f, 0.753f, 0.800f, 1.000f); // #81A1C1
    constexpr ImVec4 AccentPrimaryHover   = ImVec4(0.533f, 0.800f, 0.816f, 1.000f); // ~mix
    constexpr ImVec4 AccentPrimaryActive  = ImVec4(0.369f, 0.506f, 0.675f, 1.000f); // #5E81AC
    constexpr ImVec4 AccentCyan           = ImVec4(0.533f, 0.753f, 0.816f, 1.000f); // #88C0D0
    constexpr ImVec4 AccentTeal           = ImVec4(0.561f, 0.737f, 0.733f, 1.000f); // #8FBCBB

    // Aurora accents
    constexpr ImVec4 AccentRed            = ImVec4(0.749f, 0.380f, 0.416f, 1.000f); // #BF616A
    constexpr ImVec4 AccentOrange         = ImVec4(0.816f, 0.529f, 0.439f, 1.000f); // #D08770
    constexpr ImVec4 AccentYellow         = ImVec4(0.922f, 0.796f, 0.545f, 1.000f); // #EBCB8B
    constexpr ImVec4 AccentGreen          = ImVec4(0.639f, 0.745f, 0.549f, 1.000f); // #A3BE8C
    constexpr ImVec4 AccentPurple         = ImVec4(0.706f, 0.557f, 0.678f, 1.000f); // #B48EAD

    // Frames / inputs
    constexpr ImVec4 FrameBg              = ImVec4(0.263f, 0.298f, 0.369f, 0.85f);  // #434C5E
    constexpr ImVec4 FrameBgHovered       = ImVec4(0.506f, 0.753f, 0.800f, 0.22f); // #81A1C1
    constexpr ImVec4 FrameBgActive        = ImVec4(0.369f, 0.506f, 0.675f, 0.32f); // #5E81AC

    // Buttons
    constexpr ImVec4 Button               = ImVec4(0.506f, 0.753f, 0.800f, 0.26f);
    constexpr ImVec4 ButtonHovered        = ImVec4(0.506f, 0.753f, 0.800f, 0.42f);
    constexpr ImVec4 ButtonActive         = ImVec4(0.369f, 0.506f, 0.675f, 0.55f);

    // Sliders / checks
    constexpr ImVec4 CheckMark            = AccentCyan;
    constexpr ImVec4 SliderGrab           = AccentPrimary;
    constexpr ImVec4 SliderGrabActive     = AccentPrimaryActive;

    // Headers / Tabs
    constexpr ImVec4 Header               = ImVec4(0.263f, 0.298f, 0.369f, 0.70f);
    constexpr ImVec4 HeaderHovered        = ImVec4(0.263f, 0.298f, 0.369f, 0.95f);
    constexpr ImVec4 HeaderActive         = ImVec4(0.231f, 0.259f, 0.322f, 1.000f);

    constexpr ImVec4 Tab                  = ImVec4(0.263f, 0.298f, 0.369f, 0.90f);
    constexpr ImVec4 TabHovered           = AccentPrimaryHover;
    constexpr ImVec4 TabActive            = ImVec4(0.506f, 0.753f, 0.800f, 0.60f);
    constexpr ImVec4 TabUnfocused         = ImVec4(0.231f, 0.259f, 0.322f, 0.80f);
    constexpr ImVec4 TabUnfocusedActive   = ImVec4(0.506f, 0.753f, 0.800f, 0.45f);

    // Scrollbar
    constexpr ImVec4 ScrollbarBg          = ImVec4(0.180f, 0.204f, 0.251f, 0.85f); // #2E3440
    constexpr ImVec4 ScrollbarGrab        = ImVec4(0.298f, 0.337f, 0.416f, 0.85f);
    constexpr ImVec4 ScrollbarGrabHovered = ImVec4(0.298f, 0.337f, 0.416f, 1.00f);
    constexpr ImVec4 ScrollbarGrabActive  = AccentPrimaryActive;

    // Resize grips
    constexpr ImVec4 ResizeGrip           = ImVec4(0.506f, 0.753f, 0.800f, 0.25f);
    constexpr ImVec4 ResizeGripHovered    = ImVec4(0.506f, 0.753f, 0.800f, 0.45f);
    constexpr ImVec4 ResizeGripActive     = ImVec4(0.369f, 0.506f, 0.675f, 0.70f);

    // Plots
    constexpr ImVec4 PlotLines            = AccentPrimary;     // blue
    constexpr ImVec4 PlotLinesHovered     = AccentCyan;
    constexpr ImVec4 PlotHistogram        = AccentYellow;
    constexpr ImVec4 PlotHistogramHovered = AccentPurple;
    constexpr ImVec4 PlotAxisGrid         = ImVec4(Border.x, Border.y, Border.z, 0.55f);
    constexpr ImVec4 PlotAxisTick         = ImVec4(Text.x, Text.y, Text.z, 0.90f);
    constexpr ImVec4 PlotSelection        = ImVec4(0.533f, 0.753f, 0.816f, 0.35f);
    constexpr ImVec4 PlotCrosshairs       = AccentCyan;

    // Tables / misc
    constexpr ImVec4 TableHeaderBg        = TitleBgActive;
    constexpr ImVec4 TableBorderStrong    = ImVec4(Border.x, Border.y, Border.z, 1.00f);
    constexpr ImVec4 TableBorderLight     = ImVec4(Border.x, Border.y, Border.z, 0.50f);
    constexpr ImVec4 TableRowBg           = ImVec4(1.0f, 1.0f, 1.0f, 0.00f);
    constexpr ImVec4 TableRowBgAlt        = ImVec4(1.0f, 1.0f, 1.0f, 0.04f);

    constexpr ImVec4 TextSelectedBg       = ImVec4(0.231f, 0.259f, 0.322f, 0.85f);
    constexpr ImVec4 DragDropTarget       = AccentCyan;
    constexpr ImVec4 NavHighlight         = HeaderHovered;
    constexpr ImVec4 NavWindowingHighlight= ImVec4(0.706f, 0.557f, 0.678f, 0.700f); // purple glow
    constexpr ImVec4 NavWindowingDimBg    = ImVec4(0.000f, 0.000f, 0.000f, 0.30f);
    constexpr ImVec4 ModalWindowDimBg     = ImVec4(0.000f, 0.000f, 0.000f, 0.45f);
} // namespace NordConstants

class NordTheme final : public Theme {
public:
    void apply(ImGuiStyle& style) const override {
        using namespace NordConstants;
        ImVec4* c = style.Colors;

        c[ImGuiCol_Text]                  = Text;
        c[ImGuiCol_InputTextCursor]            = AccentPrimary;
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
    void apply(ImPlotStyle& style) const override {
        using namespace NordConstants;
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
    void apply(ImPlot3DStyle& style) const override {
        using namespace NordConstants;
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

/// @brief Register Nord in ThemeManager.
/// @param tm Theme manager.
/// @param id Unique id (defaults to "nord").
inline void registerNordTheme(ThemeManager& tm, std::string id = "nord") {
    tm.registerTheme(std::move(id), std::make_unique<NordTheme>());
}

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_NORD_THEME_HPP_INCLUDED
