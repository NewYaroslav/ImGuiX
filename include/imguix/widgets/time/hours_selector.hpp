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
#include <imguix/config/icons.hpp>

namespace ImGuiX::Widgets {

    /// \brief Configuration for hours selector.
    struct HoursSelectorConfig {
        const char* label        = u8"Hours";    ///< Combo label.
        const char* empty_hint   = u8"none";     ///< Preview when no hours selected.

        // Toolbar labels
        const char* label_all      = u8"All";    ///< Select all button label.
        const char* label_none     = u8"None";   ///< Clear selection label.

        ImVec2      cell_size    = ImVec2(0, 0); ///< Clickable cell size for each hour.
        int         rows         = 3;            ///< Grid rows (rows*cols should cover 24).
        int         cols         = 8;            ///< Grid cols.
        float       combo_width  = 0.0f;         ///< SetNextItemWidth for the combo.
        bool        use_icon_combo = true;       ///< Use BeginIconCombo instead of ImGui combo.
        const char* icon_text   = IMGUIX_ICON_CLOCK; ///< Combo icon glyph.
        bool        use_header_color_for_selected = true; ///< Use ImGuiCol_Header for selected bg.
        bool    show_cell_borders   = true;      ///< Draw cell borders.
        float   cell_border_thickness = 1.0f;   ///< Border thickness.
        ImU32   cell_border_color   = 0;           ///< 0 -> ImGuiCol_Separator.
        ImU32   cell_border_color_hovered = 0;   ///< 0 -> ImGuiCol_HeaderHovered.
        ImU32   cell_border_color_selected = 0;  ///< 0 -> ImGuiCol_Header.
        float   cell_rounding = 0.0f;            ///< Cell rounding radius.
    };

    /// \brief Renders hours picker (0..23) as a combo with a grid inside.
    /// \param id Unique ID.
    /// \param selected_hours In/out list of selected hours (0..23). Order normalized ascending on exit.
    /// \param cfg Widget configuration.
    /// \return True if selection changed this frame.
    bool HoursSelector(const char* id, std::vector<int>& selected_hours, const HoursSelectorConfig& cfg = {});

#ifdef IMGUIX_DEMO
    /// \brief Render demo for HoursSelector widget.
    inline void DemoHoursSelector() {
        static std::vector<int> hours;
        static HoursSelectorConfig cfg;
        if (ImGui::SmallButton("Work 9–18")) {
            hours.clear();
            for (int h = 9; h <= 18; ++h) hours.push_back(h);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Night 0–6")) {
            hours.clear();
            for (int h = 0; h <= 6; ++h) hours.push_back(h);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Even hours")) {
            hours.clear();
            for (int h = 0; h < 24; h += 2) hours.push_back(h);
        }

        bool changed = HoursSelector("hours", hours, cfg);
        ImGui::SameLine();
        ImGui::TextDisabled("(%d selected)", (int)hours.size());
        if (changed) {
            // handle hour changes
        }
    }
#endif

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "hours_selector.ipp"
#endif

#endif // _IMGUIX_WIDGETS_HOURS_SELECTOR_HPP_INCLUDED
