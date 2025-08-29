#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_BARADAPTER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_BARADAPTER_HPP_INCLUDED

/// \file BarAdapter.hpp
/// \brief Generic adapters for OHLC bar data access.

#include <functional>
#include <utility>
#include <ratio>
#include <type_traits>
#include <implot.h>

namespace ImGuiX::Widgets {

    /// \brief Adapter accessing bar fields through member pointers or callables.
    /// \tparam T Bar type.
    /// \tparam Open   Member pointer or callable returning open price.
    /// \tparam High   Member pointer or callable returning high price.
    /// \tparam Low    Member pointer or callable returning low price.
    /// \tparam Close  Member pointer or callable returning close price.
    /// \tparam Time   Member pointer or callable returning time value.
    /// \tparam Volume Member pointer or callable returning volume.
    /// \tparam TimeScale Divisor applied to time to convert to seconds.
    template<typename T,
             auto Open,
             auto High,
             auto Low,
             auto Close,
             auto Time,
             auto Volume = nullptr,
             typename TimeScaleRatio = std::ratio<1>>
    struct BarAdapter {
        using TimeRet   = std::invoke_result_t<decltype(Time), const T&>;
        using TimeValue = std::remove_cv_t<std::remove_reference_t<TimeRet>>;
    
        static_assert(std::is_integral_v<TimeValue>,
                  "Time must return an integral type");

        static constexpr std::int64_t scale_num = TimeScaleRatio::num;
        static constexpr std::int64_t scale_den = TimeScaleRatio::den;

        static_assert(scale_num <= scale_den,
                      "TimeScaleRatio must be <= 1 second (e.g. sec/ms/us).");
        
        static double getOpen(const T& bar) {
            return static_cast<double>(std::invoke(Open, bar));
        }
        
        static double getHigh(const T& bar) {
            return static_cast<double>(std::invoke(High, bar));
        }
        
        static double getLow(const T& bar) {
            return static_cast<double>(std::invoke(Low, bar));
        }
        
        static double getClose(const T& bar) {
            return static_cast<double>(std::invoke(Close, bar));
        }
        
        static double getTime(const T& bar) {
            constexpr double scale = static_cast<double>(scale_num) / static_cast<double>(scale_den);
            return static_cast<double>(std::invoke(Time, bar)) * scale;
        }

        static ImPlotTime getTimeExact(const T& bar) {
            const auto raw = std::invoke(Time, bar);
            if constexpr (scale_num == 1 && scale_den == 1) {
                return ImPlotTime(static_cast<time_t>(raw), 0);
            } else if constexpr (scale_num == 1 && scale_den == 1000) {
                const time_t s = static_cast<time_t>(raw / 1000);
                const int    us = static_cast<int>((raw % 1000) * 1000);
                return ImPlotTime(s, us);
            } else if constexpr (scale_num == 1 && scale_den == 1'000'000) {
                const time_t s = static_cast<time_t>(raw / 1'000'000);
                const int    us = static_cast<int>(raw % 1'000'000);
                return ImPlotTime(s, us);
            } else {
                const std::int64_t A = static_cast<std::int64_t>(raw) * scale_num;
                const std::int64_t q = A / scale_den;
                const std::int64_t r = A % scale_den;
                const int us = static_cast<int>((r * 1'000'000) / scale_den);
                return ImPlotTime(static_cast<time_t>(q), us);
            }
        }

        static double getVolume(const T& bar) {
            if constexpr (Volume != nullptr) {
                return static_cast<double>(std::invoke(Volume, bar));
            }
            return 0.0;
        }
    };

    /// \brief Default adapter for bars with members: open, high, low, close, time (ms).
    template<typename T>
    using DefaultBarAdapter = BarAdapter<
        T,
        &T::open,
        &T::high,
        &T::low,
        &T::close,
        &T::time>;

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PLOT_BARADAPTER_HPP_INCLUDED
