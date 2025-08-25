#pragma once
#ifndef _IMGUIX_UTILS_TIME_UTILS_HPP_INCLUDED
#define _IMGUIX_UTILS_TIME_UTILS_HPP_INCLUDED

/// \file time_utils.hpp
/// \brief Time/date utilities: civil<->days, timestamp<->ymdhms, H:M:S helpers, weekdays.
/// \note Supports optional header-only mode via IMGUIX_HEADER_ONLY.

#include <algorithm>
#include <cstdint>
#include <string>

namespace ImGuiX::Utils {

    // ---------- basic types ----------

    using ts_t = int64_t; ///< POSIX seconds

    // ---------- civil date algorithms (Howard Hinnant) ----------

    /// @brief Leap year check for civil year.
    inline bool is_leap_year_date(int64_t y) noexcept {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    /// @brief Days since 1970-01-01 for civil y-m-d.
    int64_t days_from_civil(int64_t y, unsigned m, unsigned d) noexcept;

    /// @brief Civil y-m-d from days since 1970-01-01.
    void civil_from_days(int64_t z, int64_t& y, unsigned& m, unsigned& d) noexcept;

    // ---------- timestamp <-> ymdhms ----------

    /// @brief num days in month for civil y-m.
    inline int num_days_in_month(int64_t year, int month) {
        static const int k_days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
        int d = k_days[month - 1];
        if (month == 2 && is_leap_year_date(year)) ++d;
        return d;
    }

    /// @brief Clamp Y/M/D/h/m/s into sane ranges (with year bounds).
    void clamp_ymdhms(
            int64_t& y,
            int& m,
            int& d,
            int& hh,
            int& mm,
            int& ss,
            int min_year,
            int max_year);
    
    /// \brief Clamp Y/M/D to sane ranges using Utils::clamp_ymdhms.
    inline void clamp_ymd(int64_t& y, int& m, int& d, int min_y, int max_y) {
        int hh=0, mm=0, ss=0;
        clamp_ymdhms(y, m, d, hh, mm, ss, min_y, max_y);
    }

    /// @brief Split POSIX seconds into Y/M/D/h/m/s.
    void timestamp_to_ymdhms(
            ts_t ts,
            int64_t& year,
            int& month,
            int& day,
            int& hour,
            int& minute,
            int& second);

    /// @brief Compose POSIX seconds from Y/M/D/h/m/s.
    int64_t ymdhms_to_timestamp(
            int64_t year,
            int month,
            int day,
            int hour,
            int minute,
            int second);

    // ---------- H:M:S helpers ----------

    inline int clamp_time_sec(int sec) {
        if (sec < 0)   return 0;
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
    
    // ---------- weekdays ----------

    /// @brief Weekday 0=Sun..6=Sat for civil date.
    int weekday_sun0_from_ymd(int64_t y, int m, int d);

    /// @brief Convert Sunday-based index (Sun=0..Sat=6) to Monday-based (Mon=0..Sun=6).
    inline int to_monday0(int sun0) { return (sun0 + 6) % 7; }

    /// \brief Format month by mode.
    inline const char* month_short_name(int m) {
        static const char* k[12] = {"Jan","Feb","Mar","Apr","May","Jun",
                                    "Jul","Aug","Sep","Oct","Nov","Dec"};
        return (m>=1 && m<=12) ? k[m-1] : "???";
    }
    
    // ---------- format ----------

    std::string format_hms(int sec);

    std::string format_signed_hms(int64_t off_sec);

    /// \brief Format YYYY-MM-DD.
    std::string format_ymd(int64_t y, int m, int d);

    /// \brief Optional human hint like "Mon 2025-08-25" based on Utils weekday.
    std::string format_with_weekday(int64_t y, int m, int d);

    /// @brief Parse "+03:00", "-05:30:15", "3:00", "0300" into signed seconds.
    bool parse_signed_hms(const char* txt, int64_t& out_sec);

    /// \brief Parse "YYYY-MM-DD", "YYYY/MM/DD", "YYYY.MM.DD", or "YYYYMMDD".
    /// Accepts optional spaces; rejects invalid ranges.
    bool parse_ymd(const char* txt, int64_t& y, int& m, int& d);

} // namespace ImGuiX::Utils

#ifdef IMGUIX_HEADER_ONLY
#   include "time_utils.ipp"
#endif

#endif // _IMGUIX_UTILS_TIME_UTILS_HPP_INCLUDED
