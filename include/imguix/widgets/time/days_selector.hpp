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
#include <imguix/config/icons.hpp>

namespace ImGuiX::Widgets {

    /// \brief Configuration for days-of-week selector.
    struct DaysSelectorConfig {
        const char* label       = u8"Days";                ///< Combo label.
        const char* empty_hint  = u8"none";                ///< Preview when no days selected.

        // Toolbar labels
        const char* label_all      = u8"All";              ///< Select all button label.
        const char* label_none     = u8"None";             ///< Clear selection label.
        const char* label_workdays = u8"Workdays";         ///< Workdays button label.
        const char* label_weekend  = u8"Weekend";          ///< Weekend button label.

        float       combo_width = 0.0f;                    ///< SetNextItemWidth for combo preview.
        bool        use_icon_combo = true;                 ///< Use BeginIconCombo for the combo.
        const char* icon_text    = IMGUIX_ICON_CALENDAR_WEEK; ///< Combo icon glyph.
        ImVec2      popup_size  = ImVec2(0, 0);            ///< Scroll area size.
        ImVec2      cell_size   = ImVec2(24, 20);          ///< Clickable cell size.
        int         rows        = 1;                       ///< Grid rows.
        int         cols        = 7;                       ///< Grid cols (rows*cols should cover 7).
        bool        use_header_color_for_selected = true;  ///< Use ImGuiCol_TextSelectedBg for selected bg.

        // Quick actions toolbar
        bool        show_toolbar_all_none     = true;      ///< Show All/None buttons.
        bool        show_toolbar_work_vs_week = true;      ///< Workdays/Weekend buttons.

        // Cell borders (like HoursSelector)
        bool    show_cell_borders        = true;           ///< Draw cell borders.
        float   cell_border_thickness    = 1.0f;           ///< Border thickness.
        ImU32   cell_border_color        = ImGui::GetColorU32(ImGuiCol_Separator); ///< Cell border color (default ImGuiCol_Separator).
        ImU32   cell_border_color_hovered = 0;             ///< 0 -> ImGuiCol_HeaderHovered.
        ImU32   cell_border_color_selected =0;             ///< 0 -> ImGuiCol_Header.
        float   cell_rounding            = 0.0f;           ///< <0 -> use style.FrameRounding.

        /// \brief Short day labels indexed 0..6 (Mon..Sun by default).
        std::array<const char*,7> short_names = {
            u8"Mon", u8"Tue", u8"Wed", u8"Thu", u8"Fri", u8"Sat", u8"Sun"
        };
    };

    /// \brief Renders days-of-week picker as combo with grid.
    /// \param id Unique ID.
    /// \param selected_days In/out list of selected days (0..6). Output normalized ascending unique.
    /// \param cfg Widget configuration.
    /// \return True if selection changed this frame.
    bool DaysOfWeekSelector(
            const char* id,
            std::vector<int>& selected_days,
            const DaysSelectorConfig& cfg = {});

#ifdef IMGUIX_DEMO
    /// \brief Render demo for DaysOfWeekSelector widget.
    inline void DemoDaysOfWeekSelector() {
        static std::vector<int> selected_days;
        static DaysSelectorConfig cfg;
        cfg.label       = u8"Working days";
        cfg.short_names = { u8"Пн", u8"Вт", u8"Ср", u8"Чт", u8"Пт", u8"Сб", u8"Вс" };
        if (DaysOfWeekSelector("##days_selector", selected_days, cfg)) {
            // handle selected days
        }
    }
#endif

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "days_selector.ipp"
#endif

#endif // _IMGUIX_WIDGETS_DAYS_SELECTOR_HPP_INCLUDED
