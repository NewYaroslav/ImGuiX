#pragma once
#ifndef _IMGUIX_THEMES_DARK_CHARCOAL_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_DARK_CHARCOAL_THEME_HPP_INCLUDED

/// \file DarkCharcoalTheme.hpp
/// \brief Dark Charcoal theme for ImGui/ImPlot, ported to ImGuiX theme system.
///
/// Based on the issue discussion: https://github.com/ocornut/imgui/issues/707
/// (Dark Charcoal variant), adapted to:
///  - use named color constants
///  - unify paddings/roundings via applyDefaultImGuiStyle
///  - provide matching ImPlot styling

#include <imguix/core/themes/Theme.hpp> // Theme + applyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace DarkCharcoalConstants
    /// \brief Color constants for the Dark Charcoal theme.
    namespace DarkCharcoalConstants {
        // Text
        constexpr ImVec4 Text                 = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
        constexpr ImVec4 TextDisabled         = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);

        // Backgrounds
        constexpr ImVec4 WindowBg             = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
        constexpr ImVec4 ChildBg              = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
        constexpr ImVec4 PopupBg              = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);

        // Borders
        constexpr ImVec4 Border               = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
        constexpr ImVec4 BorderShadow         = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);

        // Frames
        constexpr ImVec4 FrameBg              = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
        constexpr ImVec4 FrameBgHovered       = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
        constexpr ImVec4 FrameBgActive        = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);

        // Titles / Menus
        constexpr ImVec4 TitleBg              = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
        constexpr ImVec4 TitleBgActive        = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
        constexpr ImVec4 TitleBgCollapsed     = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
        constexpr ImVec4 MenuBarBg            = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg          = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
        constexpr ImVec4 ScrollbarGrab        = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
        constexpr ImVec4 ScrollbarGrabHovered = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
        constexpr ImVec4 ScrollbarGrabActive  = ImVec4(1.000f, 0.391f, 0.000f, 1.000f); // orange accent

        // Marks / Sliders
        constexpr ImVec4 CheckMark            = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
        constexpr ImVec4 SliderGrab           = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
        constexpr ImVec4 SliderGrabActive     = ImVec4(1.000f, 0.391f, 0.000f, 1.000f); // orange accent

        // Cursor
        constexpr ImVec4 InputTextCursor      = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);

        // Buttons
        constexpr ImVec4 Button               = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
        constexpr ImVec4 ButtonHovered        = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
        constexpr ImVec4 ButtonActive         = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);

        // Headers / Tables
        constexpr ImVec4 Header               = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
        constexpr ImVec4 HeaderHovered        = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
        constexpr ImVec4 HeaderActive         = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);

        // Separators
        constexpr ImVec4 SeparatorHovered     = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
        constexpr ImVec4 SeparatorActive      = ImVec4(1.000f, 0.391f, 0.000f, 1.000f); // orange accent

        // Resize grip
        constexpr ImVec4 ResizeGrip           = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
        constexpr ImVec4 ResizeGripHovered    = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
        constexpr ImVec4 ResizeGripActive     = ImVec4(1.000f, 0.391f, 0.000f, 1.000f); // orange accent

        // Tabs
        constexpr ImVec4 Tab                  = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
        constexpr ImVec4 TabHovered           = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
        constexpr ImVec4 TabActive            = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
        constexpr ImVec4 TabUnfocused         = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
        constexpr ImVec4 TabUnfocusedActive   = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);

        // Plots
        constexpr ImVec4 PlotLines            = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
        constexpr ImVec4 PlotLinesHovered     = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        constexpr ImVec4 PlotHistogram        = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
        constexpr ImVec4 PlotHistogramHovered = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);

        // Misc / Navigation
        constexpr ImVec4 TextSelectedBg       = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
        constexpr ImVec4 DragDropTarget       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f); // orange accent
        constexpr ImVec4 NavHighlight         = ImVec4(1.000f, 0.391f, 0.000f, 1.000f); // orange accent
        constexpr ImVec4 NavWindowingHighlight= ImVec4(1.000f, 0.391f, 0.000f, 1.000f); // orange accent
        constexpr ImVec4 NavWindowingDimBg    = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
        constexpr ImVec4 ModalWindowDimBg     = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

        // Derivatives / mappings
        constexpr ImVec4 Separator            = Border; // equals to Border in the original snippet
    }

    /// \class DarkCharcoalTheme
    /// \brief Dark Charcoal theme (orange accent), consistent with ImGuiX default layout config.
    class DarkCharcoalTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace DarkCharcoalConstants;
            ImVec4* colors = style.Colors;

            // If you prefer to start from ImGui's preset:
            // ImGui::StyleColorsDark(&style);

            colors[ImGuiCol_Text]                  = Text;
            colors[ImGuiCol_InputTextCursor]            = InputTextCursor;
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

            // Unify sizes/roundings from config
            applyDefaultImGuiStyle(style);

#ifdef IMGUIX_HAS_DOCKING
            // In the original snippet, Docking colors were commented; we set reasonable matches.
            colors[ImGuiCol_DockingEmptyBg]        = WindowBg;
            colors[ImGuiCol_DockingPreview]        = ImVec4(1.000f, 0.391f, 0.000f, 0.781f); // orange, semi-opaque
            // Tabs already set above.

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
#endif
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            using namespace DarkCharcoalConstants;

            // Start from ImPlot's dark defaults, then apply our colors.
            ImPlot::StyleColorsDark(&style);

            ImVec4 frame = FrameBg; frame.w = 1.0f;
            style.Colors[ImPlotCol_FrameBg]       = frame;
            style.Colors[ImPlotCol_PlotBg]        = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]    = Border;
            style.Colors[ImPlotCol_LegendBg]      = PopupBg;
            style.Colors[ImPlotCol_LegendBorder]  = Border;
            style.Colors[ImPlotCol_LegendText]    = Text;
            style.Colors[ImPlotCol_TitleText]     = Text;
            style.Colors[ImPlotCol_InlayText]     = Text;
            style.Colors[ImPlotCol_AxisText]      = Text;

            // Subtle grid/ticks over dark bg
            style.Colors[ImPlotCol_AxisGrid]      = ImVec4(0.35f, 0.35f, 0.35f, 0.55f);
            style.Colors[ImPlotCol_AxisTick]      = ImVec4(0.60f, 0.60f, 0.60f, 0.80f);
            style.Colors[ImPlotCol_AxisBgHovered] = ButtonHovered;
            style.Colors[ImPlotCol_AxisBgActive]  = ButtonActive;

            // Selection/crosshair in signature orange
            style.Colors[ImPlotCol_Selection]     = ImVec4(1.000f, 0.391f, 0.000f, 0.65f);
            style.Colors[ImPlotCol_Crosshairs]    = ImVec4(1.000f, 0.391f, 0.000f, 1.00f);

            applyDefaultImPlotStyle(style);
        }
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
        void apply(ImPlot3DStyle& style) const override {
            using namespace DarkCharcoalConstants;

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

            style.Colors[ImPlot3DCol_AxisGrid]     = ImVec4(0.35f, 0.35f, 0.35f, 0.55f);
            style.Colors[ImPlot3DCol_AxisTick]     = ImVec4(0.60f, 0.60f, 0.60f, 0.80f);

            applyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Registers the Dark Charcoal theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "dark-charcoal").
    inline void registerDarkCharcoalTheme(ThemeManager& tm, std::string id = "dark-charcoal") {
        tm.registerTheme(std::move(id), std::make_unique<DarkCharcoalTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_DARK_CHARCOAL_THEME_HPP_INCLUDED
