#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_SET_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_SET_HPP_INCLUDED

/// \file MetricsPlotSet.hpp
/// \brief Render MetricsPlotData sets with single-selection list.

#include <imguix/widgets/plot/MetricsPlotSetData.hpp>
#include <imguix/widgets/plot/MetricsPlot.hpp>

namespace ImGuiX::Widgets {

    /// \brief Runtime state for MetricsPlotSet.
    /// \invariant selected >= 0 && selected < data.sets.size() when not empty.
    struct MetricsPlotSetState {
        int selected = 0;              ///< Index of currently selected data set.
        MetricsPlotState plot_state;   ///< State passed to inner MetricsPlot.
    };

    /// \brief Render selectable MetricsPlotData sets.
    /// \param data Data sets with labels.
    /// \param state Runtime state (modified in-place).
    /// \param cfg Plot configuration.
    /// \thread_safety Not thread-safe.
    void MetricsPlotSet(
            const MetricsPlotSetData& data,
            MetricsPlotSetState& state,
            const MetricsPlotConfig& cfg = {}
        );

#   ifdef IMGUIX_DEMO
    /// \brief Engineering demo for MetricsPlotSet.
    /// \thread_safety Not thread-safe.
    inline void DemoMetricsPlotSet();
#   endif

} // namespace ImGuiX::Widgets

#include "MetricsPlotSet.ipp"

#endif // _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_SET_HPP_INCLUDED
