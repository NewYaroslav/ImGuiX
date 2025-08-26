#pragma once
#ifndef _IMGUIX_WIDGETS_DATE_PICKER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_DATE_PICKER_HPP_INCLUDED

/// \file date_picker.hpp
/// \brief Date picker (YYYY-MM-DD) with ArrowStepper controls and direct input.
/// \note Requires arrow_stepper.hpp and time_utils.hpp (Hinnant civil date).

#include <imgui.h>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>
#include <imguix/widgets/input/arrow_stepper.hpp>
#include <imguix/utils/time_utils.hpp>  // days_from_civil, civil_from_days, num_days_in_month, clamp_ymdhms, month_short_name
#include <imguix/extensions/sizing.hpp> // ImGuiX::Extensions::CalcDateComboWidth(), etc.

namespace ImGuiX::Widgets {

    /// \brief Month label mode for UI hints (steppers remain numeric).
    enum class MonthLabelMode : int { Numeric = 0, ShortName = 1 };

    /// \brief Config for DatePicker.
    struct DatePickerConfig {
        const char*     label           = u8"Date";
        const char*     desc            = u8"YYYY-MM-DD";
        float           combo_width     = 0.0f;
        bool            show_desc       = true;
        float           field_width     = 0.0f;      ///< width per ArrowStepper field
        int             min_year        = 1970;      ///< inclusive
        int             max_year        = 2099;      ///< inclusive
        MonthLabelMode  month_label     = MonthLabelMode::Numeric;
        bool            show_weekday    = false;     ///< show weekday hint
        bool            preserve_time_of_day = true;///< keep hh:mm:ss from original ts
        // Optional hard TS bounds (disabled if min_ts > max_ts).
        int64_t         min_ts          = 0;
        int64_t         max_ts          = std::numeric_limits<int64_t>::max();
    };

    // --- widget -----------------------------------------------------------------

    /// \brief Date picker (YYYY-MM-DD) using ArrowStepper; allows direct text input.
    /// \param id  Unique widget identifier.
    /// \param year In/out civil year.
    /// \param month In/out 1..12.
    /// \param day In/out 1..31 (clamped to month length).
    /// \param cfg  Picker configuration.
    /// \return true if any component changed.
    inline bool DatePicker(
            const char* id,
            int64_t& year,
            int&     month,
            int&     day,
            const DatePickerConfig& cfg = {}
        ) {
        // Clamp initial Y/M/D into configured bounds
        ImGuiX::Utils::clamp_ymd(year, month, day, cfg.min_year, cfg.max_year);
        
        const float combo_width = cfg.combo_width > 0.0f ? 
            cfg.combo_width : 
            ImGuiX::Extensions::CalcDateComboWidth();

        const float year_field_width = cfg.field_width > 0.0f ? 
            cfg.field_width :
            ImGuiX::Extensions::CalcYearFieldWidth();   

        const float hms_field_width = cfg.field_width > 0.0f ? 
            cfg.field_width : 
            ImGuiX::Extensions::CalcHMSFieldWidth();

        bool changed = false;
        const std::string preview = cfg.show_weekday
            ? ImGuiX::Utils::format_with_weekday(year, month, day)
            : ImGuiX::Utils::format_ymd(year, month, day);

        ImGui::PushID(id);
        ImGui::SetNextItemWidth(combo_width);
        if (ImGui::BeginCombo(cfg.label ? cfg.label : u8"Date", preview.c_str())) {
            if (cfg.show_desc && cfg.desc) ImGui::TextUnformatted(cfg.desc);

            // --- direct string edit ------------------------------------------------
            {
                char buf[32];
                std::snprintf(buf, sizeof(buf), "%s", ImGuiX::Utils::format_ymd(year, month, day).c_str());
                ImGui::SetNextItemWidth(combo_width);
                if (ImGui::InputText(u8"##date_str", buf, sizeof(buf))) {
                    int64_t y = year; int m = month; int d = day;
                    if (ImGuiX::Utils::parse_ymd(buf, y, m, d)) {
                        // Clamp to bounds and month length
                        ImGuiX::Utils::clamp_ymd(y, m, d, cfg.min_year, cfg.max_year);
                        const int mdays = ImGuiX::Utils::num_days_in_month(y, m);
                        if (d > mdays) d = mdays;
                        if (y != year || m != month || d != day) {
                            year = y; month = m; day = d; changed = true;
                        }
                    }
                }
                ImGui::SameLine();
                ImGui::TextUnformatted(u8"YYYY-MM-DD");
            }

            // --- steppers (Y/M/D) -------------------------------------------------
            {
                // Year: no wrap; Month/Day: no wrap; clamp day after Y/M change.
                ArrowStepperConfig scY{cfg.min_year, cfg.max_year, 1, false, year_field_width, u8"%04d y"};
                ArrowStepperConfig scM{1, 12, 1, false, hms_field_width,  u8"%02d m"};
                // Day upper bound depends on (year,month) -> set after Y/M edits.
                int mdays = ImGuiX::Utils::num_days_in_month(year, month);
                ArrowStepperConfig scD{1, mdays, 1, false, hms_field_width, u8"%02d d"};

                int dy=0, dm=0, dd=0; // deltas from arrows/wheel
                bool any = false;

                int y_i = static_cast<int>(year);
                any |= ArrowStepper(u8"Y", y_i, scY, &dy);
                if (y_i != static_cast<int>(year)) { year = static_cast<int64_t>(y_i); any = true; }
                any |= ArrowStepper(u8"M", month, scM, &dm);

                // Month short label hint (optional)
                if (cfg.month_label == MonthLabelMode::ShortName) {
                    ImGui::SameLine();
                    ImGui::TextDisabled("%s", ImGuiX::Utils::month_short_name(month));
                }

                // After potential Y/M change, recompute days-in-month and clamp 'day'
                const int new_mdays = ImGuiX::Utils::num_days_in_month(year, month);
                if (day > new_mdays) { day = new_mdays; any = true; }

                // Rebuild scD upper bound when Y/M changed
                if (dm != 0 || dy != 0) {
                    scD.min_value = 1;
                    scD.max_value = new_mdays;
                }

                any |= ArrowStepper(u8"D", day, scD, &dd);

                // Normalize in-range just in case
                if (day < 1) { day = 1; any = true; }
                if (day > new_mdays) { day = new_mdays; any = true; }

                changed |= any;
            }

            // --- extra hint line ---------------------------------------------------
            if (cfg.show_weekday) {
                const std::string hint = ImGuiX::Utils::format_with_weekday(year, month, day);
                ImGui::TextDisabled("%s", hint.c_str());
            }

            ImGui::EndCombo();
        }
        ImGui::PopID();
        return changed;
    }
    
    inline bool DatePicker(const char* id,
                           int64_t& ts,
                           const DatePickerConfig& cfg = {}) {
        // 1) Decompose ts -> Y/M/D/h/m/s
        int64_t y = 1970;
        int m = 1, d = 1, hh = 0, mm = 0, ss = 0;
        ImGuiX::Utils::timestamp_to_ymdhms(ts, y, m, d, hh, mm, ss);

        // 2) Clamp Y/M/D to config year bounds (time part unaffected here)
        ImGuiX::Utils::clamp_ymd(y, m, d, cfg.min_year, cfg.max_year);

        // Preview string (date or "WD date")
        const std::string preview = cfg.show_weekday
            ? ImGuiX::Utils::format_with_weekday(y, m, d)
            : ImGuiX::Utils::format_ymd(y, m, d);
            
        const float combo_width = cfg.combo_width > 0.0f ? 
            cfg.combo_width : 
            ImGuiX::Extensions::CalcDateComboWidth();

        const float year_field_width = cfg.field_width > 0.0f ? 
            cfg.field_width :
            ImGuiX::Extensions::CalcYearFieldWidth();   

        const float hms_field_width = cfg.field_width > 0.0f ? 
            cfg.field_width : 
            ImGuiX::Extensions::CalcHMSFieldWidth();

        bool changed = false;

        ImGui::PushID(id);
        ImGui::SetNextItemWidth(combo_width);
        if (ImGui::BeginCombo(cfg.label ? cfg.label : u8"Date", preview.c_str())) {
            if (cfg.show_desc && cfg.desc)
                ImGui::TextUnformatted(cfg.desc);

            // --- direct string edit --------------------------------------------------
            {
                char buf[32];
                std::snprintf(buf, sizeof(buf), "%s", ImGuiX::Utils::format_ymd(y, m, d).c_str());
                ImGui::SetNextItemWidth(combo_width);
                if (ImGui::InputText(u8"##date_str", buf, sizeof(buf))) {
                    int64_t ny = y; int nm = m; int nd = d;
                    if (ImGuiX::Utils::parse_ymd(buf, ny, nm, nd)) {
                        ImGuiX::Utils::clamp_ymd(ny, nm, nd, cfg.min_year, cfg.max_year);
                        const int md = ImGuiX::Utils::num_days_in_month(ny, nm);
                        if (nd > md) nd = md;
                        if (ny != y || nm != m || nd != d) {
                            y = ny; m = nm; d = nd; changed = true;
                        }
                    }
                }
                ImGui::SameLine();
                ImGui::TextUnformatted(u8"YYYY-MM-DD");
            }

            // --- steppers Y/M/D ------------------------------------------------------
            {
                ArrowStepperConfig scY{cfg.min_year, cfg.max_year, 1, false, year_field_width, u8"%04d y"};
                ArrowStepperConfig scM{1, 12, 1, false, hms_field_width,  u8"%02d m"};

                int y_i = static_cast<int>(y);
                int dy=0, dm=0, dd=0;
                bool any = false;

                any |= ArrowStepper(u8"Y", y_i, scY, &dy);
                if (y_i != static_cast<int>(y)) { y = y_i; any = true; }

                any |= ArrowStepper(u8"M", m, scM, &dm);

                if (cfg.month_label == MonthLabelMode::ShortName) {
                    ImGui::SameLine();
                    ImGui::TextDisabled("%s", ImGuiX::Utils::month_short_name(m));
                }

                // Day depends on (y,m)
                int mdays = ImGuiX::Utils::num_days_in_month(y, m);
                if (d > mdays) { d = mdays; any = true; }
                ArrowStepperConfig scD{1, mdays, 1, false, hms_field_width, u8"%02d d"};
                any |= ArrowStepper(u8"D", d, scD, &dd);

                if (d < 1) { d = 1; any = true; }
                if (d > mdays) { d = mdays; any = true; }

                changed |= any;
            }

            // --- weekday hint ---------------------------------------------------------
            if (cfg.show_weekday) {
                ImGui::TextDisabled("%s", ImGuiX::Utils::format_with_weekday(y, m, d).c_str());
            }

            ImGui::EndCombo();
        }
        ImGui::PopID();

        if (changed) {
            // 3) Recompose timestamp (preserve hh:mm:ss or snap to midnight)
            const int nh = cfg.preserve_time_of_day ? hh : 0;
            const int nm = cfg.preserve_time_of_day ? mm : 0;
            const int ns = cfg.preserve_time_of_day ? ss : 0;
            int64_t nts = ImGuiX::Utils::ymdhms_to_timestamp(y, m, d, nh, nm, ns);

            // 4) Optional clamp to TS bounds
            if (cfg.min_ts <= cfg.max_ts) {
                if (nts < cfg.min_ts) nts = cfg.min_ts;
                if (nts > cfg.max_ts) nts = cfg.max_ts;
            }

            // 5) Write back
            ts = nts;
        }

        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_DATE_PICKER_HPP_INCLUDED

