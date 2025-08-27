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
    bool DatePicker(
            const char* id,
            int64_t& year,
            int& month,
            int& day,
            const DatePickerConfig& cfg = {});
    
    /// \brief Date picker (YYYY-MM-DD) operating on a timestamp.
    /// \param id Unique widget identifier.
    /// \param ts In/out timestamp.
    /// \param cfg Picker configuration.
    /// \return True if value changed.
    bool DatePicker(const char* id,
                    int64_t& ts,
                    const DatePickerConfig& cfg = {});

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "date_picker.ipp"
#endif

#endif // _IMGUIX_WIDGETS_DATE_PICKER_HPP_INCLUDED

