#pragma once
#ifndef _IMGUIX_WIDGETS_TIME_PICKER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_TIME_PICKER_HPP_INCLUDED

/// \file time_picker.hpp
/// \brief Time pickers: absolute time-of-day (HH:MM:SS) and signed offset (+/-HH:MM:SS) with TZ list.
/// \note Requires arrow_stepper.hpp

#include <imgui.h>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "arrow_stepper.hpp"

namespace ImGuiX::Widgets {

    // ---------- helpers ----------------------------------------------------------

    inline int clamp_time_sec(int sec) {
        if (sec < 0) return 0;
        if (sec > 86399) return 86399;
        return sec;
    }

    inline void seconds_to_hms(int sec, int& h, int& m, int& s) {
        sec = clamp_time_sec(sec);
        h = sec / 3600;
        m = (sec % 3600) / 60;
        s = sec % 60;
    }

    inline int hms_to_seconds(int h, int m, int s) {
        h = std::clamp(h, 0, 23);
        m = std::clamp(m, 0, 59);
        s = std::clamp(s, 0, 59);
        return (h * 3600 + m * 60 + s);
    }

    inline std::string format_hms(uint32_t sec) {
        int h, m, s; seconds_to_hms(static_cast<int>(sec), h, m, s);
        char buf[16]; std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, m, s);
        return std::string(buf);
    }

    inline std::string format_signed_hms(int64_t off_sec) {
        bool pos = off_sec >= 0;
        uint32_t a = static_cast<uint32_t>(pos ? off_sec : -off_sec);
        char buf[20];
        int h, m, s; seconds_to_hms(static_cast<int>(a), h, m, s);
        std::snprintf(buf, sizeof(buf), "%c%02d:%02d:%02d", pos ? '+' : '-', h, m, s);
        return std::string(buf);
    }
    
    // Parse strings like "+03:00", "-05:30:15", "3:00", "0300", with optional sign.
    inline bool parse_signed_hms(const char* txt, int64_t& out_sec) {
        if (!txt) return false;
        // Trim spaces
        while (*txt == ' ' || *txt == '\t') ++txt;
        int sign = +1;
        if (*txt == '+') { sign = +1; ++txt; }
        else if (*txt == '-') { sign = -1; ++txt; }

        int h = 0, m = 0, s = 0;
        int n = 0;

        // Try HH:MM:SS
        if (std::sscanf(txt, "%d:%d:%d%n", &h, &m, &s, &n) >= 2 && n > 0) {
            // ok
        } else if (std::sscanf(txt, "%d:%d%n", &h, &m, &n) == 2) {
            s = 0;
        } else {
            // Try compact HHMM or HMM
            int packed = 0;
            if (std::sscanf(txt, "%d%n", &packed, &n) == 1) {
                if (packed < 0) packed = -packed;
                if (packed >= 0 && packed <= 235959) {
                    if (packed <= 959) { // HMM
                        h = packed / 100;
                        m = packed % 100;
                        s = 0;
                    } else if (packed <= 2359) { // HHMM
                        h = packed / 100;
                        m = packed % 100;
                        s = 0;
                    } else { // HHMMSS
                        h = packed / 10000;
                        m = (packed / 100) % 100;
                        s = packed % 100;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        if (h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59) return false;
        out_sec = static_cast<int64_t>(sign) * static_cast<int64_t>(hms_to_seconds(h, m, s));
        return true;
    }

    // ---------- configs & data ---------------------------------------------------

    struct TimePickerConfig {
        const char* label       = "Time";
        const char* desc        = "HH:MM:SS";
        float       combo_width = 160.0f;
        bool        show_desc   = true;
        float       field_width = 48.0f;
    };

    struct TimeZoneInfo {
        const char* key;
        const char* label;
        int         offset_sec;
        bool        has_dst;
    };

    inline const std::vector<TimeZoneInfo>& DefaultTimeZones() {
        static const std::vector<TimeZoneInfo> tz = {
            {"_custom",           "Custom (manual)",             0,     false},
            {"Europe/London",     "London (UTC+0)",              0,     true },
            {"Europe/Berlin",     "Frankfurt/Berlin (UTC+1)",    3600,  true },
            {"Europe/Zurich",     "Zurich (UTC+1)",              3600,  true },
            {"Europe/Moscow",     "Moscow (UTC+3)",              10800, false},

            {"America/New_York",  "New York (UTC-5)",           -18000, true },
            {"America/Chicago",   "Chicago (UTC-6)",            -21600, true },
            {"America/Denver",    "Denver (UTC-7)",             -25200, true },
            {"America/Los_Angeles","Los Angeles (UTC-8)",       -28800, true },

            {"Asia/Shanghai",     "Shanghai (UTC+8)",            28800, false},
            {"Asia/Hong_Kong",    "Hong Kong (UTC+8)",           28800, false},
            {"Asia/Singapore",    "Singapore (UTC+8)",           28800, false},
            {"Asia/Tokyo",        "Tokyo (UTC+9)",               32400, false},
            {"Asia/Seoul",        "Seoul (UTC+9)",               32400, false},

            {"Australia/Sydney",  "Sydney (UTC+10)",             36000, true },

            {"America/Sao_Paulo", "Sao Paulo (UTC-3)",          -10800, true },
            {"America/Mexico_City","Mexico City (UTC-6)",       -21600, true },

            {"Asia/Dubai",        "Dubai (UTC+4)",               14400, false},
            {"Asia/Jerusalem",    "Tel Aviv (UTC+2)",            7200,  true }
        };
        return tz;
    }

    struct TimeOffsetPickerConfig {
        const char* label        = "Offset";
        const char* desc         = "±HH:MM:SS";
        float       combo_width  = 210.0f;
        bool        show_desc    = true;
        bool        show_gmt     = true;
        bool        show_tz_list = true;
        float       field_width  = 48.0f;
        const std::vector<TimeZoneInfo>* time_zones = nullptr;
    };

    // ---------- widgets ----------------------------------------------------------

    /// \brief Time-of-day picker (0..86399 seconds).
    inline bool TimePicker(const char* id, int& seconds, const TimePickerConfig& cfg = {}) {
        bool changed = false;
        std::string preview = format_hms(static_cast<uint32_t>(seconds));

        ImGui::PushID(id);
        ImGui::SetNextItemWidth(cfg.combo_width);
        if (ImGui::BeginCombo(cfg.label ? cfg.label : "Time", preview.c_str())) {
            if (cfg.show_desc && cfg.desc) ImGui::TextUnformatted(cfg.desc);

            int h, m, s; seconds_to_hms(seconds, h, m, s);

            ArrowStepperConfig sc_h{0,23,1,true,cfg.field_width,"%02d h"};
            ArrowStepperConfig sc_m{0,59,1,true,cfg.field_width,"%02d m"};
            ArrowStepperConfig sc_s{0,59,1,true,cfg.field_width,"%02d s"};

            bool any = false;
            any |= ArrowStepper("h", h, sc_h);
            any |= ArrowStepper("m", m, sc_m);
            any |= ArrowStepper("s", s, sc_s);

            int prev = seconds;
            seconds = hms_to_seconds(h, m, s);
            changed |= (seconds != prev) || any;

            ImGui::EndCombo();
        }
        ImGui::PopID();
        return changed;
    }

    /// \brief Signed offset picker with timezone list (sign inside value; no separate checkbox).
    /// \param offset_sec   in/out signed seconds (range not enforced here; UI edits magnitude 0..23:59:59)
    /// \param has_dst_out  out, true if chosen TZ observes DST (false in Custom)
    /// \param tz_index_io  in/out, index in tz list (0 = Custom/manual)
    inline bool TimeOffsetPicker(
            const char* id,
            int64_t& offset_sec,
            bool&    has_dst_out,
            int&     tz_index_io,
            const TimeOffsetPickerConfig& cfg = {}
        ) {
        const auto& tzlist = cfg.time_zones ? *cfg.time_zones : DefaultTimeZones();
        if (tz_index_io < 0 || tz_index_io >= static_cast<int>(tzlist.size()))
            tz_index_io = 0;

        bool changed = false;
        std::string preview = format_signed_hms(offset_sec);

        ImGui::PushID(id);
        ImGui::SetNextItemWidth(cfg.combo_width);
        if (ImGui::BeginCombo(cfg.label ? cfg.label : "Offset", preview.c_str())) {
            if (cfg.show_desc && cfg.desc) ImGui::TextUnformatted(cfg.desc);

            // Timezone combo
            if (cfg.show_tz_list && !tzlist.empty()) {
                const char* cur = tzlist[tz_index_io].label;
                if (ImGui::BeginCombo("Timezone", cur)) {
                    for (int i = 0; i < (int)tzlist.size(); ++i) {
                        bool sel = (i == tz_index_io);
                        if (ImGui::Selectable(tzlist[i].label, sel)) { tz_index_io = i; changed = true; }
                        if (sel) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }

            const bool custom = (tz_index_io == 0);
            if (!custom) {
                const auto& tz = tzlist[tz_index_io];
                offset_sec = tz.offset_sec;
                has_dst_out = tz.has_dst;
            } else {
                has_dst_out = false;
            }

            // Manual controls (only in Custom)
            if (!custom) ImGui::BeginDisabled(true);

            // 1) Direct string edit with sign parsing
            {
                char buf[32];
                std::snprintf(buf, sizeof(buf), "%s", format_signed_hms(offset_sec).c_str());
                ImGui::SetNextItemWidth(140.0f);
                if (ImGui::InputText("##offset_str", buf, sizeof(buf))) {
                    int64_t v = 0;
                    if (parse_signed_hms(buf, v)) {
                        if (v != offset_sec) { offset_sec = v; changed = true; }
                    }
                }
                ImGui::SameLine();
                ImGui::TextUnformatted("±HH:MM[:SS]");
            }

            // 2) H/M/S steppers edit magnitude + allow crossing zero to change sign
            {
                bool positive = offset_sec >= 0;
                uint32_t abs_u = static_cast<uint32_t>(positive ? offset_sec : -offset_sec);
                int h, m, s; seconds_to_hms(static_cast<int>(abs_u), h, m, s);

                // remember old magnitude to detect "at zero" state
                const int old_abs = hms_to_seconds(h, m, s);
                const bool was_positive = positive;

                // steppers: NO wrap here (we control borders ourselves)
                ArrowStepperConfig sc_h{0,23,1,false,cfg.field_width,"%02d h"};
                ArrowStepperConfig sc_m{0,59,1,false,cfg.field_width,"%02d m"};
                ArrowStepperConfig sc_s{0,59,1,false,cfg.field_width,"%02d s"};
				
				sc_h.disable_at_edges = false;
				sc_m.disable_at_edges = false;
				sc_s.disable_at_edges = false;

                int dh = 0, dm = 0, ds = 0; // last deltas from arrows/wheel; 0 if edited via text
                bool any = false;
                any |= ArrowStepper("oh", h, sc_h, &dh);
                any |= ArrowStepper("om", m, sc_m, &dm);
                any |= ArrowStepper("os", s, sc_s, &ds);

                int new_abs = hms_to_seconds(h, m, s);

                // Crossing zero logic:
                // if we were exactly 00:00:00 and user pressed DOWN on any field -> go to -00:00:01
                if (old_abs == 0 && (dh < 0 || dm < 0 || ds < 0)) {
                    positive = false;
                    new_abs = 1; // make minimal 1 second
                    h = 0; m = 0; s = 1;
                    any = true;
                }
                // if we were negative and at -00:00:00 (i.e., abs==0) and user pressed UP -> +00:00:01
                else if (!was_positive && old_abs == 0 && (dh > 0 || dm > 0 || ds > 0)) {
                    positive = true;
                    new_abs = 1;
                    h = 0; m = 0; s = 1;
                    any = true;
                }

                if (any) {
                    uint32_t abs_final = static_cast<uint32_t>(new_abs);
                    int64_t  new_val   = positive ? (int64_t)abs_final : -(int64_t)abs_final;
                    if (new_val != offset_sec) { offset_sec = new_val; changed = true; }
                }
            }

            if (!custom) ImGui::EndDisabled();

            if (cfg.show_gmt) {
                std::string g = format_signed_hms(offset_sec);
                ImGui::Text("GMT %s%s", g.c_str(),
                            (!custom && has_dst_out) ? " (DST observed)" : "");
            }

            ImGui::EndCombo();
        }
        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_TIME_PICKER_HPP_INCLUDED

