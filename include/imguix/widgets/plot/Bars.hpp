#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_BARS_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_BARS_HPP_INCLUDED

/// \file Bars.hpp
/// \brief Simple OHLC bar data structures.

namespace ImGuiX::Widgets {

    /// \brief Bar with OHLC prices and time in milliseconds.
    struct Bar {
        double open{};  ///< Open price.
        double high{};  ///< High price.
        double low{};   ///< Low price.
        double close{}; ///< Close price.
        double time{};  ///< Time in milliseconds.
    };

    /// \brief Bar with OHLC prices, volume and time in milliseconds.
    struct BarWithVolume {
        double open{};   ///< Open price.
        double high{};   ///< High price.
        double low{};    ///< Low price.
        double close{};  ///< Close price.
        double volume{}; ///< Traded volume.
        double time{};   ///< Time in milliseconds.
    };

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PLOT_BARS_HPP_INCLUDED
