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
#include <imguix/widgets/input/arrow_stepper.hpp>
#include <imguix/utils/time_utils.hpp>  // ImGuiX::Utils::format_hms, etc.
#include <imguix/extensions/sizing.hpp> // ImGuiX::Extensions::CalcTimeComboWidth(), etc.

namespace ImGuiX::Widgets {

    // ---------- configs & data ---------------------------------------------------

    struct TimePickerConfig {
        const char* label       = u8"Time";
        const char* desc        = u8"HH:MM:SS";
        float       combo_width = 0.0f;
        bool        show_desc   = true;
        float       field_width = 0.0f;
    };

    struct TimeZoneInfo {
        const char* key;
        const char* label;
        int         offset_sec;
        bool        has_dst;
    };

    const std::vector<TimeZoneInfo>& DefaultTimeZones();

    struct TimeOffsetPickerConfig {
        const char* label        = u8"Offset";
        const char* desc         = u8"±HH:MM:SS";
        float       combo_width  = 0.0f;
        bool        show_desc    = true;
        bool        show_gmt     = true;
        bool        show_tz_list = true;
        float       tz_field_width  = 0.0f;
        float       hms_field_width = 0.0f;
        const std::vector<TimeZoneInfo>* time_zones = nullptr;
    };

    // ---------- widgets ----------------------------------------------------------

    /// \brief Time-of-day picker editing (hour, minute, second) separately.
    /// \param id     Unique widget identifier.
    /// \param hour   In/out [0..23].
    /// \param minute In/out [0..59].
    /// \param second In/out [0..59].
    /// \param cfg    Picker configuration.
    /// \return True if any component changed.
    bool TimePicker(const char* id,
                    int& hour,
                    int& minute,
                    int& second,
                    const TimePickerConfig& cfg = {});

    /// \brief Time-of-day picker (0..86399 seconds).
    /// \param id Unique widget identifier.
    /// \param seconds In/out: selected time in seconds since midnight.
    /// \param cfg Picker configuration.
    /// \return True if value changed.
    bool TimePicker(const char* id, int& seconds, const TimePickerConfig& cfg = {});

    /// \brief Signed offset picker with timezone list (sign inside value; no separate checkbox).
    /// \param id Unique widget identifier.
    /// \param offset_sec In/out signed seconds (range not enforced; UI edits magnitude 0..23:59:59).
    /// \param has_dst_out Output flag set when chosen TZ observes DST (false in Custom).
    /// \param tz_index_io In/out index in timezone list (0 = Custom/manual).
    /// \param cfg Picker configuration.
    /// \return True if value changed.
    bool TimeOffsetPicker(
            const char* id,
            int64_t& offset_sec,
            bool&    has_dst_out,
            int&     tz_index_io,
            const TimeOffsetPickerConfig& cfg = {});

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "time_picker.ipp"
#endif

#endif // _IMGUIX_WIDGETS_TIME_PICKER_HPP_INCLUDED
