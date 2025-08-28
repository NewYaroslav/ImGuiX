#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_BARADAPTER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_BARADAPTER_HPP_INCLUDED

/// \file BarAdapter.hpp
/// \brief Generic adapters for OHLC bar data access.

#include <functional>
#include <utility>

namespace ImGuiX::Widgets {

    inline constexpr double kMillisecondsInSecond = 1000.0; ///< Milliseconds in one second.

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
             double TimeScale = kMillisecondsInSecond>
    struct MemberBarAdapter {
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
            return static_cast<double>(std::invoke(Time, bar)) / TimeScale;
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
    using DefaultBarAdapter = MemberBarAdapter<T,
                                              &T::open,
                                              &T::high,
                                              &T::low,
                                              &T::close,
                                              &T::time>;

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PLOT_BARADAPTER_HPP_INCLUDED
