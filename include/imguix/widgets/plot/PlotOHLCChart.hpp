#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_PLOTOHLCCHART_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_PLOTOHLCCHART_HPP_INCLUDED

/// \file PlotOHLCChart.hpp
/// \brief OHLC price chart widget built on ImPlot.

#include <imgui.h>
#include <implot.h>
#include <string>
#include <vector>
#include <functional>

#include <imguix/config/colors.hpp>
#include <imguix/widgets/plot/BarAdapter.hpp>

namespace ImGuiX::Widgets {

    inline constexpr int kSecondsPerMinute = 60;                ///< Seconds in one minute.
    inline constexpr int kSecondsPerHour   = 60 * kSecondsPerMinute; ///< Seconds in one hour.
    inline constexpr int kSecondsPerDay    = 24 * kSecondsPerHour;   ///< Seconds in one day.

    /// \brief Configuration for PlotOHLCChart.
    struct OHLCChartConfig {
        ImVec4 bull_color = IMGUIX_COLOR_OHLC_BULL; ///< Color for bullish candles.
        ImVec4 bear_color = IMGUIX_COLOR_OHLC_BEAR; ///< Color for bearish candles.
        ImVec2 plot_size  = ImVec2(-1, 0);          ///< Plot size; (-1,0) uses full width.

        bool enable_timeframes = true; ///< Show timeframe combo.
        std::vector<int> timeframes_seconds = {
            kSecondsPerMinute,
            5 * kSecondsPerMinute,
            15 * kSecondsPerMinute,
            kSecondsPerHour,
            kSecondsPerDay
        }; ///< Available timeframe options in seconds.
        int initial_timeframe = kSecondsPerMinute; ///< Default timeframe value.
        std::function<void(int)> on_timeframe_changed; ///< Callback on timeframe change.

        std::vector<std::string> symbols{}; ///< Available trading symbols.
        std::string initial_symbol{"AAPL"}; ///< Initially selected symbol.
        std::function<void(const std::string&)> on_symbol_changed; ///< Callback on symbol change.

        bool enable_lines = true; ///< Render user-defined lines.
        struct Line {
            double x1; ///< Line start X (time).
            double y1; ///< Line start Y (price).
            double x2; ///< Line end X (time).
            double y2; ///< Line end Y (price).
            ImVec4 color; ///< Line color.
        };
        std::vector<Line> user_lines; ///< User lines to draw over chart.

        bool show_tooltip = true; ///< Display tooltip on bar hover.
        const char* no_data_text = "No data available"; ///< Text shown when no bars.
    };

    /// \brief Render an OHLC chart.
    /// \tparam T Bar data type.
    /// \tparam Adapter Adapter providing accessors for bar fields.
    /// \param id Unique plot identifier.
    /// \param bars Bars ordered by ascending time.
    /// \param config Chart rendering configuration.
    template<typename T, typename Adapter = DefaultBarAdapter<T>>
    void PlotOHLCChart(
            const char* id,
            const std::vector<T>& bars,
            OHLCChartConfig& config
    );

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "PlotOHLCChart.ipp"
#endif

#endif // _IMGUIX_WIDGETS_PLOT_PLOTOHLCCHART_HPP_INCLUDED
