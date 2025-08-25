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
#include <imguix/utils/time_utils.hpp>  // ImGuiX::Utils::format_hms, etc.

namespace ImGuiX::Widgets {

    // ---------- configs & data ---------------------------------------------------

    struct TimePickerConfig {
        const char* label       = u8"Time";
        const char* desc        = u8"HH:MM:SS";
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
            {u8"_custom",           u8"Custom (manual)",             0,     false},
            {u8"Europe/London",     u8"London (UTC+0)",              0,     true },
            {u8"Europe/Berlin",     u8"Frankfurt/Berlin (UTC+1)",    3600,  true },
            {u8"Europe/Zurich",     u8"Zurich (UTC+1)",              3600,  true },
            {u8"Europe/Moscow",     u8"Moscow (UTC+3)",              10800, false},

            {u8"America/New_York",  u8"New York (UTC-5)",           -18000, true },
            {u8"America/Chicago",   u8"Chicago (UTC-6)",            -21600, true },
            {u8"America/Denver",    u8"Denver (UTC-7)",             -25200, true },
            {u8"America/Los_Angeles",u8"Los Angeles (UTC-8)",       -28800, true },

            {u8"Asia/Shanghai",     u8"Shanghai (UTC+8)",            28800, false},
            {u8"Asia/Hong_Kong",    u8"Hong Kong (UTC+8)",           28800, false},
            {u8"Asia/Singapore",    u8"Singapore (UTC+8)",           28800, false},
            {u8"Asia/Tokyo",        u8"Tokyo (UTC+9)",               32400, false},
            {u8"Asia/Seoul",        u8"Seoul (UTC+9)",               32400, false},

            {u8"Australia/Sydney",  u8"Sydney (UTC+10)",             36000, true },

            {u8"America/Sao_Paulo", u8"Sao Paulo (UTC-3)",          -10800, true },
            {u8"America/Mexico_City",u8"Mexico City (UTC-6)",       -21600, true },

            {u8"Asia/Dubai",        u8"Dubai (UTC+4)",               14400, false},
            {u8"Asia/Jerusalem",    u8"Tel Aviv (UTC+2)",            7200,  true }
        };
        return tz;
    }

    struct TimeOffsetPickerConfig {
        const char* label        = u8"Offset";
        const char* desc         = u8"±HH:MM:SS";
        float       combo_width  = 210.0f;
        bool        show_desc    = true;
        bool        show_gmt     = true;
        bool        show_tz_list = true;
        float       field_width  = 48.0f;
        const std::vector<TimeZoneInfo>* time_zones = nullptr;
    };

    // ---------- widgets ----------------------------------------------------------

    /// \brief Time-of-day picker (0..86399 seconds).
    /// \param id Unique widget identifier.
    /// \param seconds In/out: selected time in seconds since midnight.
    /// \param cfg Picker configuration.
    /// \return True if value changed.
    inline bool TimePicker(const char* id, int& seconds, const TimePickerConfig& cfg = {}) {
        bool changed = false;
        std::string preview = ImGuiX::Utils::format_hms(seconds);

        ImGui::PushID(id);
        ImGui::SetNextItemWidth(cfg.combo_width);
        if (ImGui::BeginCombo(cfg.label ? cfg.label : u8"Time", preview.c_str())) {
            if (cfg.show_desc && cfg.desc) ImGui::TextUnformatted(cfg.desc);

            int h, m, s; 
			ImGuiX::Utils::seconds_to_hms(seconds, h, m, s);

            ArrowStepperConfig sc_h{0,23,1,true,cfg.field_width,u8"%02d h"};
            ArrowStepperConfig sc_m{0,59,1,true,cfg.field_width,u8"%02d m"};
            ArrowStepperConfig sc_s{0,59,1,true,cfg.field_width,u8"%02d s"};

            bool any = false;
            any |= ArrowStepper(u8"h", h, sc_h);
            any |= ArrowStepper(u8"m", m, sc_m);
            any |= ArrowStepper(u8"s", s, sc_s);

            int prev = seconds;
            seconds = ImGuiX::Utils::hms_to_seconds(h, m, s);
            changed |= (seconds != prev) || any;

            ImGui::EndCombo();
        }
        ImGui::PopID();
        return changed;
    }

    /// \brief Signed offset picker with timezone list (sign inside value; no separate checkbox).
    /// \param id Unique widget identifier.
    /// \param offset_sec In/out signed seconds (range not enforced; UI edits magnitude 0..23:59:59).
    /// \param has_dst_out Output flag set when chosen TZ observes DST (false in Custom).
    /// \param tz_index_io In/out index in timezone list (0 = Custom/manual).
    /// \param cfg Picker configuration.
    /// \return True if value changed.
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
        std::string preview = ImGuiX::Utils::format_signed_hms(offset_sec);

        ImGui::PushID(id);
        ImGui::SetNextItemWidth(cfg.combo_width);
        if (ImGui::BeginCombo(cfg.label ? cfg.label : u8"Offset", preview.c_str())) {
            if (cfg.show_desc && cfg.desc) ImGui::TextUnformatted(cfg.desc);

            // Timezone combo
            if (cfg.show_tz_list && !tzlist.empty()) {
                const char* cur = tzlist[tz_index_io].label;
                if (ImGui::BeginCombo(u8"Timezone", cur)) {
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
                std::snprintf(buf, sizeof(buf), u8"%s", ImGuiX::Utils::format_signed_hms(offset_sec).c_str());
                ImGui::SetNextItemWidth(140.0f);
                if (ImGui::InputText(u8"##offset_str", buf, sizeof(buf))) {
                    int64_t v = 0;
                    if (ImGuiX::Utils::parse_signed_hms(buf, v)) {
                        if (v != offset_sec) { offset_sec = v; changed = true; }
                    }
                }
                ImGui::SameLine();
                ImGui::TextUnformatted(u8"±HH:MM[:SS]");
            }

            // 2) H/M/S steppers edit magnitude + allow crossing zero to change sign
            {
                bool positive = offset_sec >= 0;
                uint32_t abs_u = static_cast<uint32_t>(positive ? offset_sec : -offset_sec);
                int h, m, s; 
				ImGuiX::Utils::seconds_to_hms(static_cast<int>(abs_u), h, m, s);

                // remember old magnitude to detect "at zero" state
                const int old_abs = ImGuiX::Utils::hms_to_seconds(h, m, s);
                const bool was_positive = positive;

                // steppers: NO wrap here (we control borders ourselves)
                ArrowStepperConfig sc_h{0,23,1,false,cfg.field_width,u8"%02d h"};
                ArrowStepperConfig sc_m{0,59,1,false,cfg.field_width,u8"%02d m"};
                ArrowStepperConfig sc_s{0,59,1,false,cfg.field_width,u8"%02d s"};
				
				sc_h.disable_at_edges = false;
				sc_m.disable_at_edges = false;
				sc_s.disable_at_edges = false;

                int dh = 0, dm = 0, ds = 0; // last deltas from arrows/wheel; 0 if edited via text
                bool any = false;
                any |= ArrowStepper(u8"oh", h, sc_h, &dh);
                any |= ArrowStepper(u8"om", m, sc_m, &dm);
                any |= ArrowStepper(u8"os", s, sc_s, &ds);

                int new_abs = ImGuiX::Utils::hms_to_seconds(h, m, s);

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
                std::string g = ImGuiX::Utils::format_signed_hms(offset_sec);
                ImGui::Text(u8"GMT %s%s", g.c_str(),
                            (!custom && has_dst_out) ? u8" (DST observed)" : u8"");
            }

            ImGui::EndCombo();
        }
        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_TIME_PICKER_HPP_INCLUDED

