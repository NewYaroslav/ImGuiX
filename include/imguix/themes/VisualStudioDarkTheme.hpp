#pragma once
#ifndef _IMGUIX_THEMES_VISUAL_STUDIO_DARK_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_VISUAL_STUDIO_DARK_THEME_HPP_INCLUDED

/// \file VisualStudioDarkTheme.hpp
/// \brief Visual Studio-like dark theme for ImGui/ImPlot, ported to the ImGuiX theme system.
///
/// Based on palette ideas from: https://github.com/ocornut/imgui/issues/707
/// Adaptations:
///  - repeated colors moved to named constants
///  - unified layout via ApplyDefaultImGuiStyle
///  - added matching ImPlot styling

#include <imguix/core/themes/Theme.hpp> // Theme + ApplyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace VisualStudioDarkConstants
    /// \brief Color constants for the Visual Studio-like dark theme.
    namespace VisualStudioDarkConstants {
        // Helpers: byte->float (precomputed below for constexpr-like constants)
        // 37/255=0.145, 38/255=0.149, 82/255=0.322, 85/255=0.333, 90/255=0.353, 95/255=0.373
        // 51/255=0.200, 55/255=0.216, 29/255=0.114, 151/255=0.592, 78/255=0.306
        // 0/255=0.000, 119/255=0.467, 200/255=0.784

        // Base backgrounds
        constexpr ImVec4 Bg                 = ImVec4(0.145f, 0.145f, 0.149f, 1.00f); // (37,37,38)
        constexpr ImVec4 LightBg            = ImVec4(0.322f, 0.322f, 0.333f, 1.00f); // (82,82,85)
        constexpr ImVec4 VeryLightBg        = ImVec4(0.353f, 0.353f, 0.373f, 1.00f); // (90,90,95)

        // Panels
        constexpr ImVec4 Panel              = ImVec4(0.200f, 0.200f, 0.216f, 1.00f); // (51,51,55)
        constexpr ImVec4 PanelHover         = ImVec4(0.114f, 0.592f, 0.925f, 1.00f); // (29,151,236)
        constexpr ImVec4 PanelActive        = ImVec4(0.000f, 0.467f, 0.784f, 1.00f); // (0,119,200)

        // Text / border
        constexpr ImVec4 Text               = ImVec4(1.000f, 1.000f, 1.000f, 1.00f); // (255,255,255)
        constexpr ImVec4 TextDisabled       = ImVec4(0.592f, 0.592f, 0.592f, 1.00f); // (151,151,151)
        constexpr ImVec4 Border             = ImVec4(0.306f, 0.306f, 0.306f, 1.00f); // (78,78,78)

        // Derivatives / reuse
        constexpr ImVec4 BorderShadow       = Border;
        constexpr ImVec4 MenuBarBg          = Panel;
        constexpr ImVec4 ScrollbarBg        = Panel;
        constexpr ImVec4 ScrollbarGrab      = LightBg;
        constexpr ImVec4 ScrollbarGrabHv    = VeryLightBg;
        constexpr ImVec4 ScrollbarGrabAct   = VeryLightBg;

        // Buttons / headers / separators
        constexpr ImVec4 Button             = Panel;
        constexpr ImVec4 ButtonHovered      = PanelHover;
        constexpr ImVec4 ButtonActive       = PanelActive;

        constexpr ImVec4 Header             = Panel;
        constexpr ImVec4 HeaderHovered      = PanelHover;
        constexpr ImVec4 HeaderActive       = PanelActive;

        constexpr ImVec4 Separator          = Border;
        constexpr ImVec4 SeparatorHovered   = Border;
        constexpr ImVec4 SeparatorActive    = Border;

        // Tabs
        constexpr ImVec4 Tab                = Bg;
        constexpr ImVec4 TabHovered         = PanelHover;
        constexpr ImVec4 TabActive          = PanelActive;
        constexpr ImVec4 TabUnfocused       = Bg;
        constexpr ImVec4 TabUnfocusedActive = PanelActive;

        // Misc
        constexpr ImVec4 ResizeGrip         = Bg;
        constexpr ImVec4 ResizeGripHovered  = Panel;
        constexpr ImVec4 ResizeGripActive   = LightBg;

        constexpr ImVec4 TextSelectedBg     = PanelActive;
        constexpr ImVec4 DragDropTarget     = PanelActive;
        constexpr ImVec4 NavHighlight       = Bg;
        constexpr ImVec4 NavWindowingHighlight = ImVec4(PanelActive.x, PanelActive.y, PanelActive.z, 0.70f);
        constexpr ImVec4 NavWindowingDimBg  = ImVec4(0.000f, 0.000f, 0.000f, 0.30f);
        constexpr ImVec4 ModalWindowDimBg   = ImVec4(0.000f, 0.000f, 0.000f, 0.45f);

        // Plots
        constexpr ImVec4 PlotLines          = PanelActive;
        constexpr ImVec4 PlotLinesHovered   = PanelHover;
        constexpr ImVec4 PlotHistogram      = PanelActive;
        constexpr ImVec4 PlotHistogramHv    = PanelHover;
    }

    /// \class VisualStudioDarkTheme
    /// \brief Visual Studio-like dark theme, unified with default layout config.
    class VisualStudioDarkTheme final : public Theme {
    public:
        /// \brief Apply theme colors to ImGui style.
        /// \param style Target style.
        void apply(ImGuiStyle& style) const override {
            using namespace VisualStudioDarkConstants;
            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                  = Text;
            colors[ImGuiCol_InputTextCursor]            = PanelActive;
            colors[ImGuiCol_TextDisabled]          = TextDisabled;
            colors[ImGuiCol_TextSelectedBg]        = TextSelectedBg;

            colors[ImGuiCol_WindowBg]              = Bg;
            colors[ImGuiCol_ChildBg]               = Bg;
            colors[ImGuiCol_PopupBg]               = Bg;

            colors[ImGuiCol_Border]                = Border;
            colors[ImGuiCol_BorderShadow]          = BorderShadow;

            colors[ImGuiCol_FrameBg]               = Panel;
            colors[ImGuiCol_FrameBgHovered]        = PanelHover;
            colors[ImGuiCol_FrameBgActive]         = PanelActive;

            colors[ImGuiCol_TitleBg]               = Bg;
            colors[ImGuiCol_TitleBgActive]         = Bg;
            colors[ImGuiCol_TitleBgCollapsed]      = Bg;

            colors[ImGuiCol_MenuBarBg]             = MenuBarBg;

            colors[ImGuiCol_ScrollbarBg]           = ScrollbarBg;
            colors[ImGuiCol_ScrollbarGrab]         = ScrollbarGrab;
            colors[ImGuiCol_ScrollbarGrabHovered]  = ScrollbarGrabHv;
            colors[ImGuiCol_ScrollbarGrabActive]   = ScrollbarGrabAct;

            colors[ImGuiCol_CheckMark]             = PanelActive;
            colors[ImGuiCol_SliderGrab]            = PanelHover;
            colors[ImGuiCol_SliderGrabActive]      = PanelActive;

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

            colors[ImGuiCol_PlotLines]             = PlotLines;
            colors[ImGuiCol_PlotLinesHovered]      = PlotLinesHovered;
            colors[ImGuiCol_PlotHistogram]         = PlotHistogram;
            colors[ImGuiCol_PlotHistogramHovered]  = PlotHistogramHv;

            colors[ImGuiCol_DragDropTarget]        = DragDropTarget;
            colors[ImGuiCol_NavHighlight]          = NavHighlight;
            colors[ImGuiCol_NavWindowingHighlight] = NavWindowingHighlight;
            colors[ImGuiCol_NavWindowingDimBg]     = NavWindowingDimBg;
            colors[ImGuiCol_ModalWindowDimBg]      = ModalWindowDimBg;

            colors[ImGuiCol_Tab]                   = Tab;
            colors[ImGuiCol_TabHovered]            = TabHovered;
            colors[ImGuiCol_TabActive]             = TabActive;
            colors[ImGuiCol_TabUnfocused]          = TabUnfocused;
            colors[ImGuiCol_TabUnfocusedActive]    = TabUnfocusedActive;

            // Unified roundings/paddings/borders from config
            ApplyDefaultImGuiStyle(style);

#ifdef IMGUIX_HAS_DOCKING
            colors[ImGuiCol_DockingEmptyBg]        = Bg;
            colors[ImGuiCol_DockingPreview]        = ImVec4(PanelActive.x, PanelActive.y, PanelActive.z, 0.70f);

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
#endif
        }

#ifdef IMGUIX_ENABLE_IMPLOT
        /// \brief Apply theme colors to ImPlot style.
        /// \param style Target style.
        void apply(ImPlotStyle& style) const override {
            using namespace VisualStudioDarkConstants;

            ImPlot::StyleColorsDark(&style);

            ImVec4 frame = Panel; frame.w = 1.0f;
            style.Colors[ImPlotCol_FrameBg]      = frame;
            style.Colors[ImPlotCol_PlotBg]       = Bg;
            style.Colors[ImPlotCol_PlotBorder]    = Border;
            style.Colors[ImPlotCol_LegendBg]      = Panel;
            style.Colors[ImPlotCol_LegendBorder]  = Border;
            style.Colors[ImPlotCol_LegendText]    = Text;

            style.Colors[ImPlotCol_TitleText]     = Text;
            style.Colors[ImPlotCol_InlayText]     = Text;
            style.Colors[ImPlotCol_AxisText]      = Text;

            // Subtle neutral grid/ticks for VS dark
            style.Colors[ImPlotCol_AxisGrid]      = ImVec4(0.32f, 0.32f, 0.34f, 0.55f);
            style.Colors[ImPlotCol_AxisTick]      = ImVec4(0.60f, 0.60f, 0.62f, 0.90f);
            style.Colors[ImPlotCol_AxisBgHovered] = ButtonHovered;
            style.Colors[ImPlotCol_AxisBgActive]  = ButtonActive;

            style.Colors[ImPlotCol_Selection]     = ImVec4(PanelActive.x, PanelActive.y, PanelActive.z, 0.65f);
            style.Colors[ImPlotCol_Crosshairs]    = PanelActive;

            ApplyDefaultImPlotStyle(style);
        }
#endif
#ifdef IMGUIX_ENABLE_IMPLOT3D
        /// \brief Apply theme colors to ImPlot3D style.
        /// \param style Target style.
        void apply(ImPlot3DStyle& style) const override {
            using namespace VisualStudioDarkConstants;

            ImPlot3D::StyleColorsDark(&style);

            style.Colors[ImPlot3DCol_FrameBg]      = Panel;
            style.Colors[ImPlot3DCol_PlotBg]       = Bg;
            style.Colors[ImPlot3DCol_PlotBorder]   = Border;
            style.Colors[ImPlot3DCol_LegendBg]     = Panel;
            style.Colors[ImPlot3DCol_LegendBorder] = Border;
            style.Colors[ImPlot3DCol_LegendText]   = Text;
            style.Colors[ImPlot3DCol_TitleText]    = Text;
            style.Colors[ImPlot3DCol_InlayText]    = Text;
            style.Colors[ImPlot3DCol_AxisText]     = Text;

            style.Colors[ImPlot3DCol_AxisGrid]     = ImVec4(0.32f, 0.32f, 0.34f, 0.55f);
            style.Colors[ImPlot3DCol_AxisTick]     = ImVec4(0.60f, 0.60f, 0.62f, 0.90f);

            ApplyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Registers the Visual Studio-like dark theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "vs-dark").
    inline void registerVisualStudioDarkTheme(ThemeManager& tm, std::string id = "vs-dark") {
        tm.registerTheme(std::move(id), std::make_unique<VisualStudioDarkTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_VISUAL_STUDIO_DARK_THEME_HPP_INCLUDED
