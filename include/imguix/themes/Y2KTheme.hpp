#pragma once
#ifndef _IMGUIX_THEMES_Y2K_THEME_HPP_INCLUDED
#define _IMGUIX_THEMES_Y2K_THEME_HPP_INCLUDED

/// \file Y2KTheme.hpp
/// \brief Y2K theme for ImGui/ImPlot, integrated with the ImGuiX theme system.
///
/// Aesthetics:
///  - light chrome/silver surfaces (XP-era vibe)
///  - cobalt primary, tech-purple secondary
///  - occasional magenta/lime accents on active states
///
/// Notes:
///  - repeated colors are grouped in Y2KConstants
///  - layout/rounding is unified via applyDefaultImGuiStyle
///  - includes optional ImPlot/ImPlot3D styling

#include <imguix/core/themes/Theme.hpp> // Theme + applyDefaultImGuiStyle

namespace ImGuiX::Themes {

    /// \namespace Y2KConstants
    /// \brief Color constants for the Y2K theme.
    namespace Y2KConstants {
        // Text
        constexpr ImVec4 Text                   = ImVec4(0.051f, 0.075f, 0.129f, 1.000f); // #0D1321
        constexpr ImVec4 TextDisabled           = ImVec4(0.350f, 0.400f, 0.470f, 1.000f);

        // Backgrounds (chrome/silver)
        constexpr ImVec4 WindowBg               = ImVec4(0.914f, 0.925f, 0.945f, 1.000f); // #E9ECF1
        constexpr ImVec4 ChildBg                = ImVec4(0.827f, 0.855f, 0.890f, 0.650f); // #D3DAE3, glassy
        constexpr ImVec4 PopupBg                = ImVec4(0.827f, 0.855f, 0.890f, 0.980f); // #D3DAE3

        // Borders
        constexpr ImVec4 Border                 = ImVec4(0.592f, 0.651f, 0.694f, 1.000f); // #97A6B1
        constexpr ImVec4 BorderShadow           = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);

        // Frames
        constexpr ImVec4 FrameBg                = ImVec4(0.827f, 0.855f, 0.890f, 0.95f); // #D3DAE3
        constexpr ImVec4 FrameBgHovered         = ImVec4(0.000f, 0.500f, 1.000f, 0.35f);  // ↑ альфа, чистый cobal
        constexpr ImVec4 FrameBgActive          = ImVec4(0.000f, 0.350f, 0.900f, 0.55f);  // чуть темнее/насыщеннее

        // Titles / Menu
        /*
        constexpr ImVec4 TitleBg                = ImVec4(0.827f, 0.855f, 0.890f, 0.950f);
        constexpr ImVec4 TitleBgCollapsed       = ImVec4(0.827f, 0.855f, 0.890f, 0.750f);
        constexpr ImVec4 TitleBgActive          = ImVec4(0.000f, 0.400f, 1.000f, 0.250f);
        constexpr ImVec4 MenuBarBg              = ImVec4(0.827f, 0.855f, 0.890f, 1.000f);
        */
        constexpr ImVec4 TitleBg                = ImVec4(0.914f, 0.925f, 0.945f, 1.000f); // #E9ECF1
        constexpr ImVec4 TitleBgCollapsed       = ImVec4(0.914f, 0.925f, 0.945f, 1.000f); // opaque
        constexpr ImVec4 TitleBgActive          = ImVec4(0.820f, 0.880f, 1.000f, 1.000f); // #D1E0FF (light cobalt tint)
        constexpr ImVec4 MenuBarBg              = ImVec4(0.827f, 0.855f, 0.890f, 1.000f);

        // Scrollbar
        constexpr ImVec4 ScrollbarBg            = ImVec4(0.827f, 0.855f, 0.890f, 0.850f);
        constexpr ImVec4 ScrollbarGrab          = ImVec4(0.000f, 0.400f, 1.000f, 0.400f);
        constexpr ImVec4 ScrollbarGrabHovered   = ImVec4(0.000f, 0.400f, 1.000f, 0.600f);
        constexpr ImVec4 ScrollbarGrabActive    = ImVec4(0.000f, 0.400f, 1.000f, 0.800f);

        // Accents
        constexpr ImVec4 CheckMark              = ImVec4(0.000f, 0.400f, 1.000f, 1.000f); // cobalt
        constexpr ImVec4 AccentPurple           = ImVec4(0.478f, 0.000f, 1.000f, 1.000f); // #7A00FF
        constexpr ImVec4 AccentMagenta          = ImVec4(1.000f, 0.000f, 0.800f, 1.000f); // #FF00CC
        constexpr ImVec4 AccentLime             = ImVec4(0.725f, 1.000f, 0.000f, 1.000f); // #B9FF00

        // Buttons / Headers
		/*
        constexpr ImVec4 Button                 = ImVec4(0.000f, 0.400f, 1.000f, 0.750f);
        constexpr ImVec4 ButtonHovered          = ImVec4(0.240f, 0.620f, 1.000f, 0.85f);  // lighter cobalt
        constexpr ImVec4 ButtonActive           = ImVec4(0.000f, 0.400f, 1.000f, 0.95f);  // cobalt strong
		*/
		
		constexpr ImVec4 Button          = ImVec4(0.000f, 0.400f, 1.000f, 0.60f);
		constexpr ImVec4 ButtonHovered   = ImVec4(0.000f, 0.500f, 1.000f, 0.90f);  // ярче, больше альфа
		constexpr ImVec4 ButtonActive    = ImVec4(0.000f, 0.350f, 0.900f, 1.00f);  // нажатие — темнее/насыщеннее

        //constexpr ImVec4 Header                 = ImVec4(0.000f, 0.400f, 1.000f, 0.500f);
        constexpr ImVec4 Header                 = ImVec4(0.000f, 0.400f, 1.000f, 0.20f);
        constexpr ImVec4 HeaderHovered          = ImVec4(0.000f, 0.400f, 1.000f, 0.32f);  // 32%
        constexpr ImVec4 HeaderActive           = ImVec4(0.000f, 0.400f, 1.000f, 0.42f);  // 42

        // Separators / grips
        constexpr ImVec4 Separator              = Border;
        constexpr ImVec4 SeparatorHovered       = ImVec4(0.000f, 0.400f, 1.000f, 0.780f);
        constexpr ImVec4 SeparatorActive        = ImVec4(0.478f, 0.000f, 1.000f, 1.000f);

        constexpr ImVec4 ResizeGrip             = ImVec4(0.000f, 0.400f, 1.000f, 0.400f);
        constexpr ImVec4 ResizeGripHovered      = ImVec4(0.000f, 0.400f, 1.000f, 0.700f);
        constexpr ImVec4 ResizeGripActive       = ImVec4(0.478f, 0.000f, 1.000f, 0.900f);

        // Tabs
		/*
        constexpr ImVec4 Tab                   = ImVec4(0.000f, 0.400f, 1.000f, 0.55f);
        constexpr ImVec4 TabHovered            = ImVec4(0.240f, 0.620f, 1.000f, 0.85f);
        constexpr ImVec4 TabActive             = ImVec4(0.000f, 0.400f, 1.000f, 0.85f);
		*/
		constexpr ImVec4 Tab             		= ImVec4(0.000f, 0.400f, 1.000f, 0.55f);
		constexpr ImVec4 TabHovered      		= ImVec4(0.000f, 0.500f, 1.000f, 0.85f);
		constexpr ImVec4 TabActive       		= ImVec4(0.000f, 0.400f, 1.000f, 0.85f);
        //constexpr ImVec4 Tab                    = ImVec4(0.000f, 0.400f, 1.000f, 0.550f);
        //constexpr ImVec4 TabHovered             = ImVec4(0.478f, 0.000f, 1.000f, 0.850f);
        //constexpr ImVec4 TabActive              = ImVec4(0.000f, 0.400f, 1.000f, 0.850f);
        constexpr ImVec4 TabUnfocused           = ImVec4(0.827f, 0.855f, 0.890f, 0.900f);
        constexpr ImVec4 TabUnfocusedActive     = ImVec4(0.000f, 0.400f, 1.000f, 0.700f);

        // Plots
        constexpr ImVec4 PlotLines              = ImVec4(0.000f, 0.400f, 1.000f, 1.000f); // cobalt
        constexpr ImVec4 PlotLinesHovered       = ImVec4(0.478f, 0.000f, 1.000f, 1.000f); // purple
        constexpr ImVec4 PlotHistogram          = AccentLime;
        constexpr ImVec4 PlotHistogramHovered   = AccentMagenta;

        // Misc / Navigation
        constexpr ImVec4 TextSelectedBg         = ImVec4(0.000f, 0.400f, 1.000f, 0.350f);
        constexpr ImVec4 DragDropTarget         = ImVec4(0.000f, 0.400f, 1.000f, 0.950f);
        constexpr ImVec4 NavHighlight           = HeaderHovered;
        constexpr ImVec4 NavWindowingHighlight  = ImVec4(0.700f, 0.700f, 0.700f, 0.700f);
    } // namespace Y2KConstants

    /// \class Y2KTheme
    /// \brief Light Y2K theme with cobalt/purple accents and glassy panels.
    class Y2KTheme final : public Theme {
    public:
        void apply(ImGuiStyle& style) const override {
            using namespace Y2KConstants;
            ImVec4* colors = style.Colors;

            colors[ImGuiCol_Text]                  = Text;
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
            colors[ImGuiCol_SliderGrab]            = CheckMark;
            colors[ImGuiCol_SliderGrabActive]      = Y2KConstants::AccentPurple;

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
            // Note: NavWindowingDimBg / ModalWindowDimBg left as defaults.

            // Unify sizes/roundings/paddings/borders from config
            applyDefaultImGuiStyle(style);
        }

#ifdef IMGUI_ENABLE_IMPLOT
        void apply(ImPlotStyle& style) const override {
            using namespace Y2KConstants;

            ImPlot::StyleColorsLight(&style);

            style.Colors[ImPlotCol_PlotBg]         = WindowBg;
            style.Colors[ImPlotCol_PlotBorder]     = Border;
            style.Colors[ImPlotCol_LegendBg]       = PopupBg;
            style.Colors[ImPlotCol_LegendBorder]   = Border;
            style.Colors[ImPlotCol_LegendText]     = Text;

            style.Colors[ImPlotCol_TitleText]      = Text;
            style.Colors[ImPlotCol_InlayText]      = Text;
            style.Colors[ImPlotCol_AxisText]       = Text;

            // Light grid/ticks over bright bg
            style.Colors[ImPlotCol_AxisGrid]       = ImVec4(0.75f, 0.75f, 0.75f, 0.60f);
            style.Colors[ImPlotCol_AxisTick]       = ImVec4(0.40f, 0.40f, 0.40f, 0.90f);

            // Use cobalt/purple accents
            style.Colors[ImPlotCol_Selection]      = ImVec4(0.000f, 0.400f, 1.000f, 0.55f);
            style.Colors[ImPlotCol_Crosshairs]     = ImVec4(0.000f, 0.400f, 1.000f, 1.00f);

            applyDefaultImPlotStyle(style);
        }
#endif

#ifdef IMGUI_ENABLE_IMPLOT3D
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

            style.Colors[ImPlot3DCol_AxisGrid]     = ImVec4(0.75f, 0.75f, 0.75f, 0.60f);
            style.Colors[ImPlot3DCol_AxisTick]     = ImVec4(0.40f, 0.40f, 0.40f, 0.90f);

            applyDefaultImPlot3DStyle(style);
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
