#pragma once
#ifndef _IMGUIX_WIDGETS_DAYS_SELECTOR_HPP_INCLUDED
#define _IMGUIX_WIDGETS_DAYS_SELECTOR_HPP_INCLUDED

/// \file days_selector.hpp
/// \brief Combo widget to pick days of week (0..6) via a grid (default 1x7) with a compact preview.
/// \note Index mapping by default: 0=Mon,1=Tue,2=Wed,3=Thu,4=Fri,5=Sat,6=Sun.

#include <imgui.h>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <imguix/extensions/sizing.hpp> // ImGuiX::Extensions::CalcWeekdayComboWidth(), etc.

namespace ImGuiX::Widgets {

    /// \brief Configuration for DaysOfWeekSelector.
    struct DaysSelectorConfig {
        const char* label       = u8"Days";                ///< Combo label (left of preview)
        const char* empty_hint  = u8"none";                ///< Preview when no days selected
        
        // Toolbar labels
        const char* label_all      = u8"All";
        const char* label_none     = u8"None";
        const char* label_workdays = u8"Workdays";
        const char* label_weekend  = u8"Weekend";
    
        float       combo_width = 0.0f;                    ///< SetNextItemWidth for combo preview
        ImVec2      popup_size  = ImVec2(0, 0);            ///< Scroll area size (height governs scroll)
        ImVec2      cell_size   = ImVec2(24, 20);          ///< Clickable cell size
        int         rows        = 1;                       ///< Grid rows
        int         cols        = 7;                       ///< Grid cols (rows*cols should cover 7)
        bool        use_header_color_for_selected = true;  ///< Use theme selection colors

        // Quick actions toolbar
        bool        show_toolbar_all_none     = true;
        bool        show_toolbar_work_vs_week = true;      ///< Workdays/Weekend buttons

        // Cell borders (like HoursSelector)
        bool    show_cell_borders        = true;
        float   cell_border_thickness    = 1.0f;
        ImU32   cell_border_color        = 0;  // 0 -> ImGuiCol_Border
        ImU32   cell_border_color_hovered= 0;  // 0 -> ImGuiCol_HeaderHovered
        ImU32   cell_border_color_selected=0;  // 0 -> ImGuiCol_Header
        float   cell_rounding            = -1.0f; ///< <0 -> use style.FrameRounding

        // Day labels (short). Mapping: 0..6 (Mon..Sun by default).
        std::array<const char*,7> short_names = {
            u8"Mon", u8"Tue", u8"Wed", u8"Thu", u8"Fri", u8"Sat", u8"Sun"
        };
    };

    /// \brief Renders days-of-week picker as combo with grid.
    /// \param id Unique ID.
    /// \param selected_days In/out: list of selected days (0..6). Output normalized ascending unique.
    /// \param cfg Config.
    /// \return true if selection changed this frame.
    bool DaysOfWeekSelector(
            const char* id,
            std::vector<int>& selected_days,
            const DaysSelectorConfig& cfg = {});

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "days_selector.ipp"
#endif

#endif // _IMGUIX_WIDGETS_DAYS_SELECTOR_HPP_INCLUDED
