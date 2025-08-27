#pragma once
#ifndef _IMGUIX_WIDGETS_HOURS_SELECTOR_HPP_INCLUDED
#define _IMGUIX_WIDGETS_HOURS_SELECTOR_HPP_INCLUDED

/// \file hours_selector.hpp
/// \brief Combo widget to pick hours (0..23) via a grid (default 3x8) with a compact summary preview.

#include <imgui.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <imguix/extensions/sizing.hpp> // ImGuiX::Extensions::CalcTimeComboWidth(), etc.

namespace ImGuiX::Widgets {

    /// \brief Configuration for HoursSelector.
    struct HoursSelectorConfig {
        const char* label        = u8"Hours";    ///< Combo label (left of preview)
        const char* empty_hint   = u8"none";     ///< Preview text when no hours selected

        // Toolbar labels
        const char* label_all      = u8"All";
        const char* label_none     = u8"None";

        ImVec2      cell_size    = ImVec2(0, 0); ///< Clickable cell size for each hour
        int         rows         = 3;            ///< Grid rows (rows*cols should cover 24)
        int         cols         = 8;            ///< Grid cols
        float       combo_width  = 0.0f;         ///< SetNextItemWidth for the combo
        bool        use_header_color_for_selected = true; ///< Use ImGuiCol_Header for selected bg
        bool    show_cell_borders   = true;
        float   cell_border_thickness = 1.0f;
        ImU32   cell_border_color   = 0; // 0 = возьмём ImGuiCol_Border
        ImU32   cell_border_color_hovered = 0;   // 0 = ImGuiCol_HeaderHovered
        ImU32   cell_border_color_selected = 0;  // 0 = ImGuiCol_Header
        float   cell_rounding = 0.0f;
    };

    /// \brief Renders hours picker (0..23) as a combo with a grid inside.
    /// \param id Unique ID.
    /// \param selected_hours In/out: list of selected hours (0..23). Order is normalized to ascending on exit.
    /// \param cfg Config.
    /// \return true if selection changed this frame.
    bool HoursSelector(const char* id, std::vector<int>& selected_hours, const HoursSelectorConfig& cfg = {});

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "hours_selector.ipp"
#endif

#endif // _IMGUIX_WIDGETS_HOURS_SELECTOR_HPP_INCLUDED
