#pragma once
#ifndef _IMGUIX_THEMES_CORPORATE_GREY_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_CORPORATE_GREY_THEME_HPP_INCLUDED

/// \file CorporateGreyTheme.hpp
/// \brief Corporate gray theme for ImGui/ImPlot, tailored to modern UI style.
///
/// This theme offers a neat, restrained gray palette suited for business applications and utility
/// interfaces. It supports both ImGui and ImPlot.
///
/// Based on the discussion theme: https://github.com/ocornut/imgui/issues/707
/// (Corporate Grey Theme by Sam Hocevar), but reworked and improved:
/// - repeated colors moved to named constants
/// - integration with ImPlot added
/// - rounding, border, and spacing settings refined
///
/// \author NewYaroslav
/// \date 2025

#include <imguix/core/themes/Theme.hpp> // Theme base + applyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace CorporateGreyConstants
    /// \brief Common color constants used by the Corporate Grey theme.
    namespace CorporateGreyConstants {
        constexpr ImVec4 White             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        constexpr ImVec4 MediumGrey       = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
        constexpr ImVec4 HoverGrey        = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
        constexpr ImVec4 ActiveGrey       = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
        constexpr ImVec4 DarkGrey25       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        constexpr ImVec4 DarkGrey22       = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        constexpr ImVec4 DarkGrey19       = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
        constexpr ImVec4 DarkGrey17       = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
        constexpr ImVec4 ScrollbarHover   = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
        constexpr ImVec4 ScrollbarActive  = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
        constexpr ImVec4 ButtonBase       = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
        constexpr ImVec4 HeaderBase       = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        constexpr ImVec4 ResizeBase       = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        constexpr ImVec4 ResizeHover      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        constexpr ImVec4 ResizeActive     = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        constexpr ImVec4 Highlight        = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        constexpr ImVec4 PlotLine         = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        constexpr ImVec4 TextDisabled     = ImVec4(0.62f, 0.62f, 0.62f, 1.00f);
        constexpr ImVec4 Separator        = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
    }

    /// \class CorporateGreyTheme
    /// \brief Gray interface theme for ImGui and ImPlot.
    ///
    /// Theme implementation tailored for corporate interfaces.
    /// Configures both ImGui and ImPlot.
    class CorporateGreyTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace CorporateGreyConstants;
            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                   = White;
            colors[ImGuiCol_TextDisabled]           = TextDisabled;
            colors[ImGuiCol_WindowBg]               = DarkGrey25;
            colors[ImGuiCol_ChildBg]                = DarkGrey25;
            colors[ImGuiCol_PopupBg]                = DarkGrey25;
            colors[ImGuiCol_Border]                 = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
            colors[ImGuiCol_BorderShadow]           = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
            colors[ImGuiCol_FrameBg]                = MediumGrey;
            colors[ImGuiCol_FrameBgHovered]         = HoverGrey;
            colors[ImGuiCol_FrameBgActive]          = ActiveGrey;
            colors[ImGuiCol_TitleBg]                = DarkGrey19;
            colors[ImGuiCol_TitleBgActive]          = DarkGrey22;
            colors[ImGuiCol_TitleBgCollapsed]       = DarkGrey17;
            colors[ImGuiCol_MenuBarBg]              = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
            colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
            colors[ImGuiCol_ScrollbarGrab]          = MediumGrey;
            colors[ImGuiCol_ScrollbarGrabHovered]   = ScrollbarHover;
            colors[ImGuiCol_ScrollbarGrabActive]    = ScrollbarActive;
            colors[ImGuiCol_CheckMark]              = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
            colors[ImGuiCol_SliderGrab]             = ScrollbarHover;
            colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
            colors[ImGuiCol_Button]                 = ButtonBase;
            colors[ImGuiCol_ButtonHovered]          = ScrollbarHover;
            colors[ImGuiCol_ButtonActive]           = ScrollbarActive;
            colors[ImGuiCol_Header]                 = HeaderBase;
            colors[ImGuiCol_HeaderHovered]          = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
            colors[ImGuiCol_HeaderActive]           = ScrollbarActive;
            colors[ImGuiCol_Separator]              = Separator;
            colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
            colors[ImGuiCol_SeparatorActive]        = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
            colors[ImGuiCol_ResizeGrip]             = ResizeBase;
            colors[ImGuiCol_ResizeGripHovered]      = ResizeHover;
            colors[ImGuiCol_ResizeGripActive]       = ResizeActive;
            colors[ImGuiCol_PlotLines]              = PlotLine;
            colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
            colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
            colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
            colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
            colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
            colors[ImGuiCol_NavHighlight]           = Highlight;
            colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
            colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

            applyDefaultImGuiStyle(style);
            style.DisabledAlpha = 0.75f;

#ifdef IMGUI_HAS_DOCKING
            colors[ImGuiCol_DockingEmptyBg]     = HeaderBase;
            colors[ImGuiCol_Tab]                = DarkGrey25;
            colors[ImGuiCol_TabHovered]         = TextDisabled;
            colors[ImGuiCol_TabActive]          = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
            colors[ImGuiCol_TabUnfocused]       = DarkGrey25;
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
            colors[ImGuiCol_DockingPreview]     = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
#endif
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            ImPlot::StyleColorsDark(&style);

            using namespace CorporateGreyConstants;
            style.Colors[ImPlotCol_PlotBg]        = DarkGrey25;
            style.Colors[ImPlotCol_PlotBorder]    = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
            style.Colors[ImPlotCol_LegendBg]      = DarkGrey25;
            style.Colors[ImPlotCol_LegendBorder]  = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
            style.Colors[ImPlotCol_LegendText]    = White;
            style.Colors[ImPlotCol_TitleText]     = White;
            style.Colors[ImPlotCol_InlayText]     = White;
            style.Colors[ImPlotCol_AxisText]      = White;
            style.Colors[ImPlotCol_AxisGrid]      = ImVec4(0.25f, 0.25f, 0.25f, 0.45f);
            style.Colors[ImPlotCol_AxisTick]      = ImVec4(0.45f, 0.45f, 0.45f, 0.70f);
            style.Colors[ImPlotCol_Selection]     = Highlight;
            style.Colors[ImPlotCol_Crosshairs]    = Highlight;

            applyDefaultImPlotStyle(style);
        }
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
        void apply(ImPlot3DStyle& style) const override {
            ImPlot3D::StyleColorsDark(&style);

            using namespace CorporateGreyConstants;
            style.Colors[ImPlot3DCol_FrameBg]      = MediumGrey;
            style.Colors[ImPlot3DCol_PlotBg]       = DarkGrey25;
            style.Colors[ImPlot3DCol_PlotBorder]   = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
            style.Colors[ImPlot3DCol_LegendBg]     = DarkGrey25;
            style.Colors[ImPlot3DCol_LegendBorder] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
            style.Colors[ImPlot3DCol_LegendText]   = White;
            style.Colors[ImPlot3DCol_TitleText]    = White;
            style.Colors[ImPlot3DCol_InlayText]    = White;
            style.Colors[ImPlot3DCol_AxisText]     = White;
            style.Colors[ImPlot3DCol_AxisGrid]     = ImVec4(0.25f, 0.25f, 0.25f, 0.45f);
            style.Colors[ImPlot3DCol_AxisTick]     = ImVec4(0.45f, 0.45f, 0.45f, 0.70f);

            applyDefaultImPlot3DStyle(style);
        }
#endif
    };

    /// \brief Registers the Corporate Grey theme in ThemeManager.
    /// \param tm Theme manager where the theme will be registered.
    /// \param id Unique theme identifier (defaults to "corporate-grey").
    inline void registerCorporateGreyTheme(ThemeManager& tm, std::string id = "corporate-grey") {
        tm.registerTheme(std::move(id), std::make_unique<CorporateGreyTheme>());
    }

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_CORPORATE_GREY_THEME_HPP_INCLUDED

