#include <cstdio>

namespace ImGuiX::Utils {

    int64_t days_from_civil(int64_t y, unsigned m, unsigned d) noexcept {
        y -= m <= 2;
        const int64_t era = (y >= 0 ? y : y - 399) / 400;
        const unsigned yoe = static_cast<unsigned>(y - era * 400);                 // [0, 399]
        const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;       // [0, 365]
        const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + yoe / 400 + doy;    // [0, 146096]
        return era * 146097 + static_cast<int64_t>(doe) - 719468;
    }

    void civil_from_days(int64_t z, int64_t& y, unsigned& m, unsigned& d) noexcept {
        z += 719468;
        const int64_t era = (z >= 0 ? z : z - 146096) / 146097;
        const unsigned doe = static_cast<unsigned>(z - era * 146097);              // [0, 146096]
        const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;// [0, 399]
        y = static_cast<int64_t>(yoe) + era * 400;
        const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
        const unsigned mp = (5 * doy + 2) / 153;
        d = doy - (153 * mp + 2) / 5 + 1;
        m = mp + (mp < 10 ? 3 : -9);
        y += (m <= 2);
    }

    void clamp_ymdhms(
            int64_t& y,
            int& m,
            int& d,
            int& hh,
            int& mm,
            int& ss,
            int min_year,
            int max_year) {
        y = std::max<int64_t>(min_year, std::min<int64_t>(max_year, y));
        m = std::clamp(m, 1, 12);
        d = std::clamp(d, 1, num_days_in_month(y, m));
        hh = std::clamp(hh, 0, 23);
        mm = std::clamp(mm, 0, 59);
        ss = std::clamp(ss, 0, 59);
    }

    void timestamp_to_ymdhms(
            ts_t ts,
            int64_t& year,
            int& month,
            int& day,
            int& hour,
            int& minute,
            int& second) {
        int64_t days = ts / 86400;
        int64_t rem  = ts % 86400;
        if (rem < 0) { rem += 86400; --days; }
        unsigned m, d;
        civil_from_days(days, year, m, d);
        month = static_cast<int>(m);
        day   = static_cast<int>(d);
        hour  = static_cast<int>(rem / 3600); rem %= 3600;
        minute= static_cast<int>(rem / 60);
        second= static_cast<int>(rem % 60);
    }

    int64_t ymdhms_to_timestamp(
            int64_t year,
            int month,
            int day,
            int hour,
            int minute,
            int second) {
        int64_t days = days_from_civil(year, static_cast<unsigned>(month), static_cast<unsigned>(day));
        return days * 86400 + static_cast<int64_t>(hour) * 3600 + minute * 60 + second;
    }

    int weekday_sun0_from_ymd(int64_t y, int m, int d) {
        int64_t z = days_from_civil(y, static_cast<unsigned>(m), static_cast<unsigned>(d)); // days since 1970-01-01
        int w = int((z + 4) % 7); if (w < 0) w += 7;              // 1970-01-01 is Thu
        return w;                                                 // 0=Sun..6=Sat
    }

    std::string format_hms(int sec) {
        int h, m, s; ImGuiX::Utils::seconds_to_hms(sec, h, m, s);
        char buf[16]; std::snprintf(buf, sizeof(buf), u8"%02d:%02d:%02d", h, m, s);
        return std::string(buf);
    }

    std::string format_signed_hms(int64_t off_sec) {
        bool pos = off_sec >= 0;
        uint32_t a = static_cast<uint32_t>(pos ? off_sec : -off_sec);
        char buf[20];
        int h, m, s; ImGuiX::Utils::seconds_to_hms(static_cast<int>(a), h, m, s);
        std::snprintf(buf, sizeof(buf), u8"%c%02d:%02d:%02d", pos ? '+' : '-', h, m, s);
        return std::string(buf);
    }

    std::string format_ymd(int64_t y, int m, int d) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%04lld-%02d-%02d",
                      static_cast<long long>(y), m, d);
        return std::string(buf);
    }

    std::string format_with_weekday(int64_t y, int m, int d) {
        static const char* WD[7] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
        int w_sun0 = ImGuiX::Utils::weekday_sun0_from_ymd(y, m, d);
        int w_mon0 = ImGuiX::Utils::to_monday0(w_sun0);
        (void)w_mon0; // keep for possible future Mon-based
        char buf[48];
        std::snprintf(buf, sizeof(buf), "%s %s",
            WD[w_sun0],
            ImGuiX::Utils::format_ymd(y, m, d).c_str()
        );
        return std::string(buf);
    }

    bool parse_signed_hms(const char* txt, int64_t& out_sec) {
        if (!txt) return false;
        while (*txt == ' ' || *txt == '\t') ++txt;
        int sign = +1;
        if (*txt == '+') { sign = +1; ++txt; }
        else if (*txt == '-') { sign = -1; ++txt; }

        int h = 0, m = 0, s = 0, n = 0;

        if (std::sscanf(txt, u8"%d:%d:%d%n", &h, &m, &s, &n) >= 2 && n > 0) {
            // ok
        } else if (std::sscanf(txt, u8"%d:%d%n", &h, &m, &n) == 2) {
            s = 0;
        } else {
            int packed = 0;
            if (std::sscanf(txt, u8"%d%n", &packed, &n) == 1) {
                if (packed < 0) packed = -packed;
                if (packed >= 0 && packed <= 235959) {
                    if (packed <= 959) {       // HMM
                        h = packed / 100; m = packed % 100; s = 0;
                    } else if (packed <= 2359) { // HHMM
                        h = packed / 100; m = packed % 100; s = 0;
                    } else {                    // HHMMSS
                        h = packed / 10000; m = (packed / 100) % 100; s = packed % 100;
                    }
                } else return false;
            } else return false;
        }

        if (h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59) return false;
        out_sec = static_cast<int64_t>(sign) * static_cast<int64_t>(hms_to_seconds(h, m, s));
        return true;
    }

    bool parse_ymd(const char* txt, int64_t& y, int& m, int& d) {
        if (!txt) return false;
        while (*txt==' ' || *txt=='\t') ++txt;

        int yy=0, mm=0, dd=0, n=0;
        // Try dashed/slashed/dotted
        if (std::sscanf(txt, "%d-%d-%d%n", &yy, &mm, &dd, &n) == 3 && n>0) {
            // ok
        } else if (std::sscanf(txt, "%d/%d/%d%n", &yy, &mm, &dd, &n) == 3 && n>0) {
            // ok
        } else if (std::sscanf(txt, "%d.%d.%d%n", &yy, &mm, &dd, &n) == 3 && n>0) {
            // ok
        } else {
            // Packed YYYYMMDD
            long long packed = 0;
            if (std::sscanf(txt, "%lld%n", &packed, &n) == 1) {
                if (packed < 0) packed = -packed;
                if (packed >= 10101 && packed <= 99991231) {
                    yy = static_cast<int>(packed / 10000);
                    mm = static_cast<int>((packed / 100) % 100);
                    dd = static_cast<int>(packed % 100);
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        // Basic range check (month/day will be clamped later if needed)
        if (mm < 1 || mm > 12) return false;
        if (dd < 1 || dd > 31) return false;

        y = yy; m = mm; d = dd;
        return true;
    }

} // namespace ImGuiX::Utils

