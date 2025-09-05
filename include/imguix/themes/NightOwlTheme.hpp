#pragma once
#ifndef _IMGUIX_THEMES_NIGHT_OWL_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_NIGHT_OWL_THEME_HPP_INCLUDED

/// \file NightOwlTheme.hpp
/// \brief Night Owl–inspired dark theme for ImGui/ImPlot, integrated with the ImGuiX theme system.
///
/// Aesthetics:
///  - deep navy background (#011627)
///  - soft light text (#D6DEEB)
///  - cyan/blue primaries (#82AAFF, #7FDBCA) + purple/yellow accents
///
/// Notes:
///  - repeated colors are grouped in NightOwlConstants
///  - layout/rounding is unified via applyDefaultImGuiStyle
///  - includes optional ImPlot/ImPlot3D styling

#include <imguix/core/themes/Theme.hpp>

namespace ImGuiX::Themes {

    /// \namespace NightOwlConstants
    /// \brief Color constants for the Night Owl theme.
    namespace NightOwlConstants {
        // Text
        constexpr ImVec4 Text                   = ImVec4(0.839f, 0.871f, 0.922f, 1.000f); // #D6DEEB
        constexpr ImVec4 TextDisabled           = ImVec4(0.373f, 0.494f, 0.592f, 1.000f); // #5F7E97

        // Backgrounds (deep navy)
        constexpr ImVec4 WindowBg               = ImVec4(0.004f, 0.086f, 0.153f, 1.000f); // #011627
        constexpr ImVec4 ChildBg                = ImVec4(0.043f, 0.145f, 0.227f, 0.980f); // #0B253A @98%
        constexpr ImVec4 PopupBg                = ImVec4(0.055f, 0.161f, 0.247f, 0.980f); // #0E293F @98%

        // Borders / separators (dim steel-blue)
        constexpr ImVec4 Border                 = ImVec4(0.118f, 0.227f, 0.341f, 0.90f);  // #1E3A57
        constexpr ImVec4 BorderShadow           = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);
        constexpr ImVec4 Separator              = ImVec4(0.118f, 0.227f, 0.341f, 1.00f);
        constexpr ImVec4 SeparatorHovered       = ImVec4(0.510f, 0.667f, 1.000f, 0.55f);  // #82AAFF @55%
        constexpr ImVec4 SeparatorActive        = ImVec4(0.498f, 0.859f, 0.792f, 0.70f);  // #7FDBCA @70%

        // Titles / Menu
        constexpr ImVec4 TitleBg                = ImVec4(0.043f, 0.145f, 0.227f, 1.000f); // #0B253A
        constexpr ImVec4 TitleBgCollapsed       = ImVec4(0.043f, 0.145f, 0.227f, 1.000f);
        constexpr ImVec4 TitleBgActive          = ImVec4(0.043f, 0.161f, 0.259f, 1.000f); // #0B2942
        constexpr ImVec4 MenuBarBg              = ImVec4(0.043f, 0.145f, 0.227f, 1.000f);

        // Accents
        constexpr ImVec4 AccentPrimary          = ImVec4(0.510f, 0.667f, 1.000f, 1.000f); // #82AAFF
        constexpr ImVec4 AccentPrimaryHover     = ImVec4(0.600f, 0.740f, 1.000f, 1.000f);
        constexpr ImVec4 AccentPrimaryActive    = ImVec4(0.380f, 0.560f, 0.980f, 1.000f);
        constexpr ImVec4 AccentSecondary        = ImVec4(0.498f, 0.859f, 0.792f, 1.000f); // #7FDBCA
        constexpr ImVec4 AccentPurple           = ImVec4(0.780f, 0.573f, 0.918f, 1.000f); // #C792EA
        constexpr ImVec4 AccentYellow           = ImVec4(0.925f, 0.769f, 0.553f, 1.000f); // #ECC48D
        constexpr ImVec4 AccentRed              = ImVec4(0.937f, 0.325f, 0.314f, 1.000f); // #EF5350

        // Frames (inputs/combo)
        constexpr ImVec4 FrameBg                = ImVec4(0.043f, 0.161f, 0.259f, 0.95f);  // #0B2942
        constexpr ImVec4 FrameBgHovered         = ImVec4(0.510f, 0.667f, 1.000f, 0.20f);
        constexpr ImVec4 FrameBgActive          = ImVec4(0.498f, 0.859f, 0.792f, 0.30f);

        // Buttons
        constexpr ImVec4 Button                 = ImVec4(0.510f, 0.667f, 1.000f, 0.30f);
        constexpr ImVec4 ButtonHovered          = ImVec4(0.510f, 0.667f, 1.000f, 0.45f);
        constexpr ImVec4 ButtonActive           = ImVec4(0.498f, 0.859f, 0.792f, 0.55f);

        // Sliders / checks
        constexpr ImVec4 CheckMark              = AccentSecondary;
        constexpr ImVec4 SliderGrab             = AccentPrimary;
        constexpr ImVec4 SliderGrabActive       = AccentPrimaryActive;

        // Headers / Tabs
        constexpr ImVec4 Header                 = ImVec4(0.114f, 0.231f, 0.325f, 0.60f);  // #1D3B53 @60%
        constexpr ImVec4 HeaderHovered          = ImVec4(0.114f, 0.231f, 0.325f, 0.85f);
        constexpr ImVec4 HeaderActive           = ImVec4(0.055f, 0.161f, 0.247f, 1.000f); // #0E293F

        constexpr ImVec4 Tab                    = ImVec4(0.114f, 0.231f, 0.325f, 0.85f);
        constexpr ImVec4 TabHovered             = AccentPrimaryHover;
        constexpr ImVec4 TabActive              = ImVec4(0.510f, 0.667f, 1.000f, 0.65f);
        constexpr ImVec4 TabUnfocused           = ImVec4(0.114f, 0.231f, 0.325f, 0.70f);
        constexpr ImVec4 TabUnfocusedActive     = ImVec4(0.510f, 0.667f, 1.000f, 0.50f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg            = ImVec4(0.004f, 0.086f, 0.153f, 0.85f);
        constexpr ImVec4 ScrollbarGrab          = ImVec4(0.118f, 0.227f, 0.341f, 0.75f);
        constexpr ImVec4 ScrollbarGrabHovered   = ImVec4(0.118f, 0.227f, 0.341f, 1.00f);
        constexpr ImVec4 ScrollbarGrabActive    = ImVec4(0.380f, 0.560f, 0.980f, 1.00f);

        // Resize grips
        constexpr ImVec4 ResizeGrip             = ImVec4(0.510f, 0.667f, 1.000f, 0.25f);
        constexpr ImVec4 ResizeGripHovered      = ImVec4(0.510f, 0.667f, 1.000f, 0.45f);
        constexpr ImVec4 ResizeGripActive       = ImVec4(0.498f, 0.859f, 0.792f, 0.70f);

        // Plots (ImPlot)
        constexpr ImVec4 PlotLines              = AccentPrimary;
        constexpr ImVec4 PlotLinesHovered       = AccentSecondary;
        constexpr ImVec4 PlotHistogram          = AccentYellow;
        constexpr ImVec4 PlotHistogramHovered   = AccentPurple;
        constexpr ImVec4 PlotAxisGrid           = ImVec4(Border.x, Border.y, Border.z, 0.55f);
        constexpr ImVec4 PlotAxisTick           = ImVec4(Text.x,   Text.y,   Text.z,   0.90f);
        constexpr ImVec4 PlotSelection          = ImVec4(0.055f, 0.161f, 0.247f, 0.70f);  // #0E293F @70%
        constexpr ImVec4 PlotCrosshairs         = ImVec4(0.510f, 0.667f, 1.000f, 1.00f);

        // Tables
        constexpr ImVec4 TableHeaderBg          = TitleBgActive;
        constexpr ImVec4 TableBorderStrong      = ImVec4(Border.x, Border.y, Border.z, 1.00f);
        constexpr ImVec4 TableBorderLight       = ImVec4(Border.x, Border.y, Border.z, 0.50f);
        constexpr ImVec4 TableRowBg             = ImVec4(1.000f, 1.000f, 1.000f, 0.00f);
        constexpr ImVec4 TableRowBgAlt          = ImVec4(1.000f, 1.000f, 1.000f, 0.04f);

        // Misc / Navigation
        constexpr ImVec4 TextSelectedBg         = ImVec4(0.055f, 0.161f, 0.247f, 0.85f);
        constexpr ImVec4 DragDropTarget         = ImVec4(0.498f, 0.859f, 0.792f, 0.95f);
        constexpr ImVec4 NavHighlight           = HeaderHovered;
        constexpr ImVec4 NavWindowingHighlight  = ImVec4(0.780f, 0.573f, 0.918f, 0.700f);
        constexpr ImVec4 NavWindowingDimBg      = ImVec4(0.000f, 0.000f, 0.000f, 0.30f);
        constexpr ImVec4 ModalWindowDimBg       = ImVec4(0.000f, 0.000f, 0.000f, 0.45f);
    } // namespace NightOwlConstants

    /// \class NightOwlTheme
    /// \brief Dark Night Owl theme with cyan/blue primaries and readable contrast.
    class NightOwlTheme final : public Theme {
    public:
        /// \brief Apply theme colors to ImGui style.
        /// \param style Target style.
        void apply(ImGuiStyle& style) const override {
            using namespace NightOwlConstants;
            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                  = Text;
            colors[ImGuiCol_InputTextCursor]            = AccentPrimary;
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
            colors[ImGuiCol_HeaderHovered]          = HeaderHovered;
            colors[ImGuiCol_HeaderActive]           = HeaderActive;

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
            colors[ImGuiCol_DockingPreview] = AccentSecondary;
            colors[ImGuiCol_DockingEmptyBg] = ImVec4(WindowBg.x, WindowBg.y, WindowBg.z, 1.0f);
#endif

            // Unify sizes/roundings/paddings/borders from config
            applyDefaultImGuiStyle(style);
        }

#ifdef IMGUI_ENABLE_IMPLOT
        /// \brief Apply theme colors to ImPlot style.
        /// \param style Target style.
        void apply(ImPlotStyle& style) const override {
            using namespace NightOwlConstants;

            ImPlot::StyleColorsDark(&style);

            // Frames & plot panels
            ImVec4 frame = ChildBg; frame.w = 1.0f;
            style.Colors[ImPlotCol_FrameBg]        = frame;
            style.Colors[ImPlotCol_PlotBg]         = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]     = Border;

            // Legend
            style.Colors[ImPlotCol_LegendBg]       = PopupBg;
            style.Colors[ImPlotCol_LegendBorder]   = Border;
            style.Colors[ImPlotCol_LegendText]     = Text;

            // Texts
            style.Colors[ImPlotCol_TitleText]      = Text;
            style.Colors[ImPlotCol_InlayText]      = Text;
            style.Colors[ImPlotCol_AxisText]       = Text;

            // Axes / grid
            style.Colors[ImPlotCol_AxisGrid]       = PlotAxisGrid;
            style.Colors[ImPlotCol_AxisTick]       = PlotAxisTick;
            style.Colors[ImPlotCol_AxisBgHovered]  = ButtonHovered;
            style.Colors[ImPlotCol_AxisBgActive]   = ButtonActive;

            // Selection / crosshair
            style.Colors[ImPlotCol_Selection]      = PlotSelection;
            style.Colors[ImPlotCol_Crosshairs]     = PlotCrosshairs;

            applyDefaultImPlotStyle(style);
        }
#endif

#ifdef IMGUI_ENABLE_IMPLOT3D
        /// \brief Apply theme colors to ImPlot3D style.
        /// \param style Target style.
        void apply(ImPlot3DStyle& style) const override {
            using namespace NightOwlConstants;

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

    /// \brief Registers the Night Owl theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "night-owl").
    inline void registerNightOwlTheme(ThemeManager& tm, std::string id = "night-owl") {
        tm.registerTheme(std::move(id), std::make_unique<NightOwlTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_NIGHT_OWL_THEME_HPP_INCLUDED
