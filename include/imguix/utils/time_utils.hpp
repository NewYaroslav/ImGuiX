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

    // ---------- civil date algorithms (Howard Hinnant) ----------

    /// \brief Check if the year is leap.
    /// \param y Civil year.
    /// \return True if \p y is leap.
    inline bool is_leap_year_date(int64_t y) noexcept {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    /// \brief Compute days since 1970-01-01 for civil date.
    /// \param y Civil year.
    /// \param m Month [1,12].
    /// \param d Day [1,31].
    /// \return Days since Unix epoch.
    int64_t days_from_civil(int64_t y, unsigned m, unsigned d) noexcept;

    /// \brief Convert days since 1970-01-01 to civil date.
    /// \param z Days since Unix epoch.
    /// \param y Output civil year.
    /// \param m Output month.
    /// \param d Output day.
    void civil_from_days(int64_t z, int64_t& y, unsigned& m, unsigned& d) noexcept;

    // ---------- timestamp <-> ymdhms ----------

    /// \brief Number of days in month for civil year and month.
    /// \param year Civil year.
    /// \param month Month [1,12].
    /// \return Days in month.
    inline int num_days_in_month(int64_t year, int month) {
        static const int k_days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
        int d = k_days[month - 1];
        if (month == 2 && is_leap_year_date(year)) ++d;
        return d;
    }

    /// \brief Clamp Y/M/D/h/m/s into ranges with year bounds.
    /// \param y Year value to clamp.
    /// \param m Month value to clamp.
    /// \param d Day value to clamp.
    /// \param hh Hour value to clamp.
    /// \param mm Minute value to clamp.
    /// \param ss Second value to clamp.
    /// \param min_year Minimum year inclusive.
    /// \param max_year Maximum year inclusive.
    void clamp_ymdhms(
            int64_t& y,
            int& m,
            int& d,
            int& hh,
            int& mm,
            int& ss,
            int min_year,
            int max_year);

    /// \brief Clamp year, month, and day using clamp_ymdhms.
    /// \param y Year value to clamp.
    /// \param m Month value to clamp.
    /// \param d Day value to clamp.
    /// \param min_y Minimum year inclusive.
    /// \param max_y Maximum year inclusive.
    inline void clamp_ymd(int64_t& y, int& m, int& d, int min_y, int max_y) {
        int hh = 0, mm = 0, ss = 0;
        clamp_ymdhms(y, m, d, hh, mm, ss, min_y, max_y);
    }

    /// \brief Split POSIX seconds into date-time components.
    /// \param ts Seconds since Unix epoch.
    /// \param year Output year.
    /// \param month Output month.
    /// \param day Output day.
    /// \param hour Output hour.
    /// \param minute Output minute.
    /// \param second Output second.
    void timestamp_to_ymdhms(
            int64_t ts,
            int64_t& year,
            int& month,
            int& day,
            int& hour,
            int& minute,
            int& second);

    /// \brief Compose POSIX seconds from date-time components.
    /// \param year Civil year.
    /// \param month Month.
    /// \param day Day.
    /// \param hour Hour.
    /// \param minute Minute.
    /// \param second Second.
    /// \return Seconds since Unix epoch.
    int64_t ymdhms_to_timestamp(
            int64_t year,
            int month,
            int day,
            int hour,
            int minute,
            int second);

    // ---------- H:M:S helpers ----------

    /// \brief Clamp seconds to the range [0, 86399].
    /// \param sec Seconds to clamp.
    /// \return Clamped seconds.
    inline int clamp_time_sec(int sec) {
        if (sec < 0)   return 0;
        if (sec > 86399) return 86399;
        return sec;
    }

    /// \brief Convert seconds to hours, minutes, and seconds.
    /// \param sec Seconds to convert.
    /// \param h Output hours.
    /// \param m Output minutes.
    /// \param s Output seconds.
    inline void seconds_to_hms(int sec, int& h, int& m, int& s) {
        sec = clamp_time_sec(sec);
        h = sec / 3600;
        m = (sec % 3600) / 60;
        s = sec % 60;
    }

    /// \brief Compose seconds from hours, minutes, and seconds.
    /// \param h Hours component.
    /// \param m Minutes component.
    /// \param s Seconds component.
    /// \return Seconds in range [0, 86399].
    inline int hms_to_seconds(int h, int m, int s) {
        h = std::clamp(h, 0, 23);
        m = std::clamp(m, 0, 59);
        s = std::clamp(s, 0, 59);
        return (h * 3600 + m * 60 + s);
    }
    
    // ---------- weekdays ----------

    /// \brief Weekday index where 0=Sun and 6=Sat.
    /// \param y Civil year.
    /// \param m Month [1,12].
    /// \param d Day [1,31].
    /// \return Weekday index.
    int weekday_sun0_from_ymd(int64_t y, int m, int d);

    /// \brief Convert Sunday-based index (Sun=0..Sat=6) to Monday-based.
    /// \param sun0 Sunday-based weekday index.
    /// \return Monday-based weekday index.
    inline int to_monday0(int sun0) { return (sun0 + 6) % 7; }

    /// \brief Short English month name.
    /// \param m Month [1,12].
    /// \return Pointer to static month name or "???".
    inline const char* month_short_name(int m) {
        static const char* k[12] = {"Jan","Feb","Mar","Apr","May","Jun",
                                    "Jul","Aug","Sep","Oct","Nov","Dec"};
        return (m>=1 && m<=12) ? k[m-1] : "???";
    }

    // ---------- format ----------

    /// \brief Format seconds as HH:MM:SS (no day component).
    /// \param sec Seconds to format.
    /// \return Formatted string.
    std::string format_hms_u32(int sec);

    /// \brief Format seconds as "HH:MM:SS".
    /// \param sec Seconds to format.
    /// \return Formatted string.
    std::string format_hms(int sec);

    /// \brief Format signed offset seconds as "+HH:MM:SS".
    /// \param off_sec Offset seconds.
    /// \return Formatted string.
    std::string format_signed_hms(int64_t off_sec);

    /// \brief Format date as "YYYY-MM-DD".
    /// \param y Civil year.
    /// \param m Month.
    /// \param d Day.
    /// \return Formatted string.
    std::string format_ymd(int64_t y, int m, int d);

    /// \brief Format date with weekday prefix like "Mon 2025-08-25".
    /// \param y Civil year.
    /// \param m Month.
    /// \param d Day.
    /// \return Formatted string.
    std::string format_with_weekday(int64_t y, int m, int d);

    /// \brief Parse signed offset like "+03:00" or "-05:30:15".
    /// \param txt Input text.
    /// \param out_sec Output seconds.
    /// \return True on success.
    bool parse_signed_hms(const char* txt, int64_t& out_sec);

    /// \brief Parse "YYYY-MM-DD", "YYYY/MM/DD", "YYYY.MM.DD", or "YYYYMMDD".
    /// \param txt Input text.
    /// \param y Output year.
    /// \param m Output month.
    /// \param d Output day.
    /// \return True on success.
    /// Accepts optional spaces; rejects invalid ranges.
    bool parse_ymd(const char* txt, int64_t& y, int& m, int& d);

} // namespace ImGuiX::Utils

#ifdef IMGUIX_HEADER_ONLY
#   include "time_utils.ipp"
#endif

#endif // _IMGUIX_UTILS_TIME_UTILS_HPP_INCLUDED
