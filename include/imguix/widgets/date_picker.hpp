#pragma once
#ifndef _IMGUIX_WIDGETS_DATE_PICKER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_DATE_PICKER_HPP_INCLUDED

/// \file date_picker.hpp
/// \brief Simple UTC date/time picker without local-time dependencies.

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>

#include <imgui.h>

namespace ImGuiX::Widgets {

    namespace detail {

        using ts_t = int64_t;

        constexpr bool is_leap_year_date(int64_t y) noexcept {
            return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        }

        // Howard Hinnant's civil <-> days algorithms
        constexpr int64_t days_from_civil(int64_t y, unsigned m, unsigned d) noexcept {
            y -= m <= 2;
            const int64_t era = (y >= 0 ? y : y - 399) / 400;
            const unsigned yoe = static_cast<unsigned>(y - era * 400);        // [0, 399]
            const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1; // [0, 365]
            const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + yoe / 400 + doy; // [0, 146096]
            return era * 146097 + static_cast<int64_t>(doe) - 719468;
        }

        constexpr void civil_from_days(int64_t z, int64_t& y, unsigned& m, unsigned& d) noexcept {
            z += 719468;
            const int64_t era = (z >= 0 ? z : z - 146096) / 146097;
            const unsigned doe = static_cast<unsigned>(z - era * 146097);
            const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
            y = static_cast<int64_t>(yoe) + era * 400;
            const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
            const unsigned mp = (5 * doy + 2) / 153;
            d = doy - (153 * mp + 2) / 5 + 1;
            m = mp + (mp < 10 ? 3 : -9);
            y += (m <= 2);
        }

        inline void timestamp_to_ymdhms(
                ts_t ts,
                int64_t& year,
                int& month,
                int& day,
                int& hour,
                int& minute,
                int& second) {
            int64_t days = ts / 86400;
            int64_t rem = ts % 86400;
            if (rem < 0) { rem += 86400; --days; }
            unsigned m, d;
            civil_from_days(days, year, m, d);
            month = static_cast<int>(m);
            day = static_cast<int>(d);
            hour = static_cast<int>(rem / 3600); rem %= 3600;
            minute = static_cast<int>(rem / 60);
            second = static_cast<int>(rem % 60);
        }

        inline ts_t ymdhms_to_timestamp(
                int64_t year,
                int month,
                int day,
                int hour,
                int minute,
                int second) {
            int64_t days = days_from_civil(year, static_cast<unsigned>(month), static_cast<unsigned>(day));
            return days * 86400 + static_cast<int64_t>(hour) * 3600 + minute * 60 + second;
        }

        inline int num_days_in_month(int64_t year, int month) {
            static const int k_days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
            int d = k_days[month - 1];
            if (month == 2 && is_leap_year_date(year)) ++d;
            return d;
        }

    } // namespace detail

    struct DatePickerConfig {
        const char* label       = u8"Date";
        const char* desc_date   = u8"DD/MM/YYYY";
        const char* desc_time   = u8"HH:MM:SS";
        float       combo_width = 180.0f;
        bool        show_desc   = true;
        float       field_width = 48.0f;
    };

    inline std::string format_ymdhms(detail::ts_t ts) {
        int64_t y; int m, d, h, min, s;
        detail::timestamp_to_ymdhms(ts, y, m, d, h, min, s);
        char buf[32];
        std::snprintf(buf, sizeof(buf), u8"%02d/%02d/%04lld %02d:%02d:%02d",
                      d, m, static_cast<long long>(y), h, min, s);
        return std::string(buf);
    }

    inline bool DatePicker(
            const char* id,
            detail::ts_t& ts,
            const DatePickerConfig& cfg = {}) {
        bool changed = false;
        std::string preview = format_ymdhms(ts);

        ImGui::PushID(id);
        ImGui::SetNextItemWidth(cfg.combo_width);
        if (ImGui::BeginCombo(cfg.label ? cfg.label : id, preview.c_str())) {
            if (cfg.show_desc && cfg.desc_date) ImGui::TextUnformatted(cfg.desc_date);

            int64_t year; int month; int day; int hour; int minute; int second;
            detail::timestamp_to_ymdhms(ts, year, month, day, hour, minute, second);

            int day_idx0 = day - 1;
            int month_idx = month - 1;
            int year_i = static_cast<int>(year);

            const char* number_items[] = {
                "1","2","3","4","5","6","7","8","9","10",
                "11","12","13","14","15","16","17","18","19","20",
                "21","22","23","24","25","26","27","28","29","30",
                "31"
            };

            ImGui::SetNextItemWidth(cfg.field_width);
            int max_days = detail::num_days_in_month(year_i, month);
            if (ImGui::Combo("##day", &day_idx0, number_items, max_days)) {
                day = day_idx0 + 1;
                changed = true;
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(cfg.field_width);
            if (ImGui::Combo("##month", &month_idx, number_items, 12)) {
                month = month_idx + 1;
                day = std::min(day, detail::num_days_in_month(year_i, month));
                changed = true;
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(cfg.field_width * 2);
            if (ImGui::InputInt("##year", &year_i)) {
                if (year_i < 1970) year_i = 1970;
                day = std::min(day, detail::num_days_in_month(year_i, month));
                changed = true;
            }

            if (cfg.show_desc && cfg.desc_time) ImGui::TextUnformatted(cfg.desc_time);
            int time_vals[3] = {hour, minute, second};
            ImGui::SetNextItemWidth(cfg.field_width * 3 + ImGui::GetStyle().ItemSpacing.x * 2);
            if (ImGui::InputInt3("##time", time_vals)) {
                changed = true;
            }
            hour = std::clamp(time_vals[0], 0, 23);
            minute = std::clamp(time_vals[1], 0, 59);
            second = std::clamp(time_vals[2], 0, 59);

            detail::ts_t new_ts = detail::ymdhms_to_timestamp(year_i, month, day, hour, minute, second);
            if (new_ts != ts) {
                ts = new_ts;
                changed = true;
            }

            ImGui::EndCombo();
        }
        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_DATE_PICKER_HPP_INCLUDED
