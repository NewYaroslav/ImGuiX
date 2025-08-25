#pragma once
#ifndef _IMGUIX_WIDGETS_CALENDAR_DATE_PICKER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_CALENDAR_DATE_PICKER_HPP_INCLUDED

/// \file calendar_date_picker.hpp
/// \brief Date picker with month/year combos, inline arrows, and calendar grid.

#include <imgui.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <ctime>      // for Today button
#include <string>
#include <imguix/utils/time_utils.hpp>  // ImGuiX::Utils::timestamp_to_ymdhms, etc.

namespace ImGuiX::Widgets {

    enum class MonthLabelMode : int { Numeric = 0, ShortName = 1 };
    enum class WeekStart      : int { Monday = 0, Sunday = 1 };

    /// \brief Config for CalendarDatePicker.
    struct CalendarDatePickerConfig {
        const char*  label            = u8"Date";
        float        combo_width      = 200.0f;
        /// \brief Preview format for combo button (DD.MM.YYYY); snprintf-compatible.
        const char*  preview_fmt      = u8"%02d.%02d.%04lld";

        // Header controls
        bool         show_month_mode_selector = true; ///< Allow user to switch month label mode.
        MonthLabelMode month_label_mode = MonthLabelMode::ShortName;
        int          dynamic_year_span  = 20;     ///< +/- span around current year for year-combo.
        bool         show_quick_today   = true;
        bool         show_week_header   = true;
        WeekStart    week_start         = WeekStart::Monday;

        // Bounds/clamping
        bool         clamp_to_years     = true;
        int          min_year           = 1970;
        int          max_year           = 2099;

        // Grid visuals
        ImVec2       cell_size          = ImVec2(28.0f, 28.0f);
        float        header_spacing     = 6.0f;   ///< Space between controls and calendar table.
    };

    inline void format_month_label(char* buf, size_t n, int m, MonthLabelMode mode) {
        if (mode == MonthLabelMode::Numeric) std::snprintf(buf, n, "%02d", m);
        else std::snprintf(buf, n, "%s", ImGuiX::Utils::month_short_name(m));
    }

    /// \brief Format preview "DD.MM.YYYY".
    inline std::string format_date_preview(int64_t ts, const CalendarDatePickerConfig& cfg) {
        int64_t y; int m, d, hh, mm, ss;
        ImGuiX::Utils::timestamp_to_ymdhms(ts, y, m, d, hh, mm, ss);
        char buf[32]; std::snprintf(buf, sizeof(buf), cfg.preview_fmt, d, m, (long long)y);
        return std::string(buf);
    }

    /// \brief Date picker that edits only the date part, preserving time-of-day.
    /// \param id Unique widget id.
    /// \param ts In/Out POSIX seconds (local semantics are up to the caller).
    /// \param cfg UI configuration.
    /// \return true if value changed.
    inline bool CalendarDatePicker(const char* id,
                                   ImGuiX::Utils::ts_t& ts,
                                   const CalendarDatePickerConfig& cfg = {}) {
        bool changed = false;

        // Decode current timestamp (preserve time-of-day on date changes)
        int64_t y; int m, d, hh, mi, ss;
        ImGuiX::Utils::timestamp_to_ymdhms(ts, y, m, d, hh, mi, ss);
        if (cfg.clamp_to_years) {
            ImGuiX::Utils::clamp_ymdhms(y, m, d, hh, mi, ss, cfg.min_year, cfg.max_year);
        }

        std::string preview = format_date_preview(ts, cfg);

        ImGui::PushID(id);
        ImGui::SetNextItemWidth(cfg.combo_width);
        if (ImGui::BeginCombo(cfg.label ? cfg.label : id, preview.c_str())) {
            // --- Header: Month, Year, Mode, Arrows, Today
            ImGui::BeginGroup();
            // Month combo
            {
                char cur_m[16]; format_month_label(cur_m, sizeof(cur_m), m, cfg.month_label_mode);
                if (ImGui::BeginCombo(u8"##month", cur_m)) {
                    for (int mm = 1; mm <= 12; ++mm) {
                        char lab[16]; format_month_label(lab, sizeof(lab), mm, cfg.month_label_mode);
                        bool sel = (mm == m);
                        if (ImGui::Selectable(lab, sel)) { m = mm; changed = true; }
                        if (sel) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
            }
            // Year combo (dynamic window)
            {
                const int span = std::max(1, cfg.dynamic_year_span);
                const int ymin = cfg.clamp_to_years ? std::max(cfg.min_year, (int)y - span) : (int)y - span;
                const int ymax = cfg.clamp_to_years ? std::min(cfg.max_year, (int)y + span) : (int)y + span;
                char ylab[12]; std::snprintf(ylab, sizeof(ylab), "%lld", (long long)y);
                if (ImGui::BeginCombo(u8"##year", ylab)) {
                    for (int yy = ymin; yy <= ymax; ++yy) {
                        bool sel = (yy == (int)y);
                        char s[12]; std::snprintf(s, sizeof(s), "%d", yy);
                        if (ImGui::Selectable(s, sel)) { y = yy; changed = true; }
                        if (sel) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
            }
            // Month label mode selector (optional)
            MonthLabelMode mode = cfg.month_label_mode;
            if (cfg.show_month_mode_selector) {
                const char* cur = (mode == MonthLabelMode::Numeric ? "MM" : "Mon");
                if (ImGui::BeginCombo(u8"##mlabel", cur)) {
                    if (ImGui::Selectable("MM", (mode == MonthLabelMode::Numeric))) {
                        mode = MonthLabelMode::Numeric; changed = true;
                    }
                    if (ImGui::Selectable("Mon", (mode == MonthLabelMode::ShortName))) {
                        mode = MonthLabelMode::ShortName; changed = true;
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
            }

            // Arrows: prev/next (month; with Shift = year)
            {
                ImGui::PushButtonRepeat(true);
                if (ImGui::ArrowButton(u8"##prev", ImGuiDir_Left)) {
                    const bool shift = ImGui::GetIO().KeyShift;
                    if (shift) { y -= 1; }
                    else { m -= 1; if (m < 1) { m = 12; --y; } }
                    changed = true;
                }
                ImGui::SameLine(0.0f, 4.0f);
                if (ImGui::ArrowButton(u8"##next", ImGuiDir_Right)) {
                    const bool shift = ImGui::GetIO().KeyShift;
                    if (shift) { y += 1; }
                    else { m += 1; if (m > 12) { m = 1; ++y; } }
                    changed = true;
                }
                ImGui::PopButtonRepeat();
                ImGui::SameLine();
            }

            if (cfg.show_quick_today) {
                if (ImGui::SmallButton(u8"Today")) {
                    // Set current local date; keep current time-of-day (hh:mi:ss)
                    std::time_t now_t = std::time(nullptr);
                    std::tm lt{};
                #if defined(_WIN32)
                    localtime_s(&lt, &now_t);
                #else
                    localtime_r(&now_t, &lt);
                #endif
                    y = (int64_t)lt.tm_year + 1900;
                    m = lt.tm_mon + 1;
                    d = lt.tm_mday;
                    changed = true;
                }
            }
            ImGui::EndGroup();

            // Clamp day to month length after header edits
            int nd = ImGuiX::Utils::num_days_in_month(y, m);
            if (d > nd) { d = nd; }

            // Re-apply year clamps
            if (cfg.clamp_to_years) {
                if (y < cfg.min_year) y = cfg.min_year;
                if (y > cfg.max_year) y = cfg.max_year;
            }

            // --- Space before grid
            ImGui::Dummy(ImVec2(1.0f, cfg.header_spacing));

            // --- Week header
            const char* kWeekMoSu[7] = {"Mo","Tu","We","Th","Fr","Sa","Su"};
            const char* kWeekSuSa[7] = {"Su","Mo","Tu","We","Th","Fr","Sa"};
            const bool monday0 = (cfg.week_start == WeekStart::Monday);

            if (cfg.show_week_header) {
                ImGui::BeginGroup();
                for (int i = 0; i < 7; ++i) {
                    if (i) ImGui::SameLine();
                    ImGui::TextUnformatted(monday0 ? kWeekMoSu[i] : kWeekSuSa[i]);
                }
                ImGui::EndGroup();
            }

            // --- Calendar table (7 columns x 6 rows)
            const int first_w_sun0 = ImGuiX::Utils::weekday_sun0_from_ymd(y, m, 1);
            const int start_col = monday0 ? ImGuiX::Utils::to_monday0(first_w_sun0) : first_w_sun0;

            // Prev month day count and this month length
            const int pm = (m == 1 ? 12 : m - 1);
            const int py = (m == 1 ? (int)y - 1 : (int)y);
            const int pm_days = ImGuiX::Utils::num_days_in_month(py, pm);
            const int this_days = nd;

            // Selected day highlight
            const bool clamp_sel = (d >= 1 && d <= this_days);
            int selected_day = clamp_sel ? d : std::min(1, this_days);

            // Draw 6 rows
            int day = 1;
            for (int row = 0; row < 6; ++row) {
                for (int col = 0; col < 7; ++col) {
                    if (col) ImGui::SameLine();
                    ImGui::BeginGroup();
                    bool in_this_month = false;
                    int label_day = 0;

                    if (row == 0 && col < start_col) {
                        // Leading days from previous month (disabled)
                        label_day = pm_days - (start_col - col - 1);
                    } else if (day <= this_days) {
                        in_this_month = true;
                        label_day = day++;
                    } else {
                        // Trailing days from next month (disabled)
                        int trailing = (row * 7 + col) - (start_col + this_days);
                        label_day = trailing + 1;
                    }

                    char lab[8]; std::snprintf(lab, sizeof(lab), "%d", label_day);

                    if (!in_this_month) {
                        ImGui::BeginDisabled(true);
                        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
                        ImGui::Button(lab, cfg.cell_size);  // visual only
                        ImGui::PopStyleColor();
                        ImGui::EndDisabled();
                    } else {
                        bool is_sel = (label_day == selected_day);
                        ImGuiSelectableFlags f = ImGuiSelectableFlags_DontClosePopups;
                        if (ImGui::Selectable(lab, is_sel, f, cfg.cell_size)) {
                            d = label_day; changed = true;
                        }
                    }
                    ImGui::EndGroup();
                }
            }

            // Compose Y/M/D back into timestamp (keep time-of-day)
            int64_t y2 = y; int m2 = m, d2 = std::clamp(d, 1, ImGuiX::Utils::num_days_in_month(y, m));
            ImGuiX::Utils::clamp_ymdhms(y2, m2, d2, hh, mi, ss, cfg.min_year, cfg.max_year);
            int64_t new_ts = ImGuiX::Utils::ymdhms_to_timestamp(y2, m2, d2, hh, mi, ss);
            if (new_ts != ts) { ts = new_ts; changed = true; }

            ImGui::EndCombo();

            (void)mode; // if you persist month mode in cfg elsewhere.
        }
        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_CALENDAR_DATE_PICKER_HPP_INCLUDED
