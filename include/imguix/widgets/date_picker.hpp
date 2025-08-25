#pragma once
#ifndef _IMGUIX_WIDGETS_DATE_PICKER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_DATE_PICKER_HPP_INCLUDED

/// \file date_picker.hpp
/// \brief UTC date/time picker with optional steppers, quick actions, seconds toggle and bounds.

#include <imgui.h>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>
#include <chrono>
#include <limits>

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

        inline void clamp_ymdhms(int64_t& y, int& m, int& d, int& hh, int& mm, int& ss,
                                 int min_year, int max_year) {
            y = std::max<int64_t>(min_year, std::min<int64_t>(max_year, y));
            m = std::clamp(m, 1, 12);
            d = std::clamp(d, 1, num_days_in_month(y, m));
            hh = std::clamp(hh, 0, 23);
            mm = std::clamp(mm, 0, 59);
            ss = std::clamp(ss, 0, 59);
        }

    } // namespace detail

    struct DatePickerConfig {
        const char* label         = u8"Date";
        float       combo_width   = 200.0f;
        float       field_width   = 48.0f;
        bool        show_desc     = true;        ///< show row captions (DD/MM/YYYY and HH:MM[:SS])
        const char* desc_date     = u8"DD/MM/YYYY";
        const char* desc_time     = u8"HH:MM:SS";

        // UX options
        bool        show_seconds  = true;        ///< show seconds row/field
        bool        show_steppers = true;        ///< show -/+ buttons near date row
        bool        show_quick    = true;        ///< show Today/Now buttons

        // Bounds & clamping (optional)
        bool        clamp_to_years = true;       ///< clamp Y/M/D/.. into [min_year..max_year]
        int         min_year       = 1970;
        int         max_year       = 2099;

        // Preview format (seconds omitted automatically if show_seconds=false)
        const char* preview_fmt_with_sec = u8"%02d/%02d/%04lld %02d:%02d:%02d";
        const char* preview_fmt_no_sec   = u8"%02d/%02d/%04lld %02d:%02d";
    };

    inline std::string format_preview(detail::ts_t ts, const DatePickerConfig& cfg) {
        int64_t y; int m, d, hh, mm, ss;
        detail::timestamp_to_ymdhms(ts, y, m, d, hh, mm, ss);
        char buf[32];
        if (cfg.show_seconds) {
            std::snprintf(buf, sizeof(buf), cfg.preview_fmt_with_sec, d, m, (long long)y, hh, mm, ss);
        } else {
            std::snprintf(buf, sizeof(buf), cfg.preview_fmt_no_sec,   d, m, (long long)y, hh, mm);
        }
        return std::string(buf);
    }

    inline bool DatePicker(const char* id, detail::ts_t& ts, const DatePickerConfig& cfg = {}) {
        bool changed = false;
        std::string preview = format_preview(ts, cfg);

        ImGui::PushID(id);
        ImGui::SetNextItemWidth(cfg.combo_width);
        if (ImGui::BeginCombo(cfg.label ? cfg.label : id, preview.c_str())) {

            int64_t year; int month; int day; int hour; int minute; int second;
            ImGuiX::Widgets::detail::timestamp_to_ymdhms(ts, year, month, day, hour, minute, second);

            // --- Date row
            if (cfg.show_desc && cfg.desc_date) ImGui::TextUnformatted(cfg.desc_date);

            // steppers for date (optional)
            if (cfg.show_steppers) {
                ImGui::PushButtonRepeat(true);
                // Day
                ImGui::SetNextItemWidth(cfg.field_width);
                if (ImGui::SmallButton(u8"-##d")) { day = std::max(1, day - 1); changed = true; }
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 0.5f);
                ImGui::SetNextItemWidth(cfg.field_width);
                if (ImGui::InputInt(u8"##day", &day, 0, 0)) { changed = true; }
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 0.5f);
                if (ImGui::SmallButton(u8"+##d")) { day = std::min(detail::num_days_in_month(year, month), day + 1); changed = true; }

                // Month
                ImGui::SameLine();
                if (ImGui::SmallButton(u8"-##m")) { month = std::max(1, month - 1); day = std::min(day, detail::num_days_in_month(year, month)); changed = true; }
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 0.5f);
                ImGui::SetNextItemWidth(cfg.field_width);
                if (ImGui::InputInt(u8"##month", &month, 0, 0)) { changed = true; }
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 0.5f);
                if (ImGui::SmallButton(u8"+##m")) { month = std::min(12, month + 1); day = std::min(day, detail::num_days_in_month(year, month)); changed = true; }

                // Year
                ImGui::SameLine();
                if (ImGui::SmallButton(u8"-##y")) { year -= 1; changed = true; }
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 0.5f);
                ImGui::SetNextItemWidth(cfg.field_width * 2.0f);
                int year_i = static_cast<int>(year);
                if (ImGui::InputInt(u8"##year", &year_i, 0, 0)) { changed = true; }
                year = year_i;
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 0.5f);
                if (ImGui::SmallButton(u8"+##y")) { year += 1; changed = true; }

                ImGui::PopButtonRepeat();
            } else {
                // simple inputs without steppers
                ImGui::SetNextItemWidth(cfg.field_width);
                if (ImGui::InputInt(u8"##day", &day, 1, 5)) changed = true;
                ImGui::SameLine();
                ImGui::SetNextItemWidth(cfg.field_width);
                if (ImGui::InputInt(u8"##month", &month, 1, 3)) changed = true;
                ImGui::SameLine();
                ImGui::SetNextItemWidth(cfg.field_width * 2.0f);
                if (ImGui::InputInt(u8"##year", (int*)&year, 1, 10)) changed = true;
            }

            // Clamp date after edits
            if (cfg.clamp_to_years) detail::clamp_ymdhms(year, month, day, hour, minute, second, cfg.min_year, cfg.max_year);
            else {
                month = std::clamp(month, 1, 12);
                day   = std::clamp(day,   1, detail::num_days_in_month(year, month));
            }

            // --- Time row
            if (cfg.show_desc && cfg.desc_time) ImGui::TextUnformatted(cfg.desc_time);

            int time_vals[3] = {hour, minute, second};
            if (!cfg.show_seconds) time_vals[2] = 0;

            ImGui::SetNextItemWidth(cfg.field_width * (cfg.show_seconds ? 3.0f : 2.0f)
                                    + ImGui::GetStyle().ItemSpacing.x * (cfg.show_seconds ? 2.0f : 1.0f));
            if (cfg.show_seconds) {
                bool ch = false;
                ImGui::SetNextItemWidth(cfg.field_width);
                ch |= ImGui::InputInt(u8"##hh", &time_vals[0], 1, 5);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(cfg.field_width);
                ch |= ImGui::InputInt(u8"##mm", &time_vals[1], 1, 5);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(cfg.field_width);
                ch |= ImGui::InputInt(u8"##ss", &time_vals[2], 1, 5);
                if (ch) changed = true;
            } else {
                // два поля (часы и минуты)
                bool ch = false;
                ImGui::SetNextItemWidth(cfg.field_width);
                ch |= ImGui::InputInt(u8"##hh", &time_vals[0], 1, 5);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(cfg.field_width);
                ch |= ImGui::InputInt(u8"##mm", &time_vals[1], 1, 5);
                if (ch) changed = true;
            }

            hour   = std::clamp(time_vals[0], 0, 23);
            minute = std::clamp(time_vals[1], 0, 59);
            second = cfg.show_seconds ? std::clamp(time_vals[2], 0, 59) : 0;

            // --- Quick actions
            if (cfg.show_quick) {
                if (ImGui::SmallButton(u8"Today")) {
                    // midnight UTC today
                    using namespace std::chrono;
                    auto now  = system_clock::now();
                    auto s    = duration_cast<seconds>(now.time_since_epoch()).count();
                    int64_t Y; int M, D, H, MIN, S;
                    ImGuiX::Widgets::detail::timestamp_to_ymdhms((detail::ts_t)s, Y, M, D, H, MIN, S);
                    hour = minute = second = 0;
                    year = Y; month = M; day = D;
                    changed = true;
                }
                ImGui::SameLine();
                if (ImGui::SmallButton(u8"Now")) {
                    using namespace std::chrono;
                    auto now  = system_clock::now();
                    auto s    = duration_cast<seconds>(now.time_since_epoch()).count();
                    int64_t Y; int M, D, H, MIN, S;
                    ImGuiX::Widgets::detail::timestamp_to_ymdhms((detail::ts_t)s, Y, M, D, H, MIN, S);
                    year = Y; month = M; day = D; hour = H; minute = MIN; second = cfg.show_seconds ? S : 0;
                    changed = true;
                }
            }

            // Rebuild timestamp
            detail::ts_t new_ts = detail::ymdhms_to_timestamp(year, month, day, hour, minute, second);
            if (new_ts != ts) { ts = new_ts; changed = true; }

            ImGui::EndCombo();
        }
        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_DATE_PICKER_HPP_INCLUDED
