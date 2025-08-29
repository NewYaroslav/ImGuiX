#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_BARS_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_BARS_HPP_INCLUDED

/// \file bars.hpp
/// \brief Simple OHLC/OHLCV bar data structures.

#include <cstdint>

namespace ImGuiX::Widgets {

    /// \brief Bar with OHLC prices and time.
    struct OhlcBar {
        double open   = 0; ///< Open price.
        double high   = 0; ///< High price.
        double low    = 0; ///< Low price.
        double close  = 0; ///< Close price.
        uint64_t time = 0; ///< Time.
    };

    /// \brief Bar with OHLC prices, volume and time.
    struct OhlcvBar {
        double open   = 0; ///< Open price.
        double high   = 0; ///< High price.
        double low    = 0; ///< Low price.
        double close  = 0; ///< Close price.
        double volume = 0; ///< Traded volume.
        uint64_t time = 0; ///< Time.
    };

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PLOT_BARS_HPP_INCLUDED
