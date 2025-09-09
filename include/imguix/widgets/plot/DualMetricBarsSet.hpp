#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_SET_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_SET_HPP_INCLUDED

/// \file DualMetricBarsSet.hpp
/// \brief Render selectable DualMetricBarsData sets.

#include <imguix/widgets/plot/DualMetricBarsSetData.hpp>
#include <imguix/widgets/plot/DualMetricBarsPlot.hpp>
#include <imguix/widgets/plot/MetricsPlot.hpp> // for CalcMetricsDndWidth

namespace ImGuiX::Widgets {

    /// \brief Runtime state for DualMetricBarsPlotSet.
    /// \invariant selected.size() == data.sets.size().
    struct DualMetricBarsSetState {
        std::vector<bool> selected;           ///< Selection flags per set.
        DualMetricBarsPlotState plot_state;   ///< Inner plot state.
    };

    /// \brief Render selectable collection of DualMetricBarsData.
    /// \param data Input data sets.
    /// \param state Runtime state (modified).
    /// \param cfg Plot configuration.
    /// \thread_safety Not thread-safe.
    void DualMetricBarsPlotSet(
            const DualMetricBarsSetData& data,
            DualMetricBarsSetState& state,
            const DualMetricBarsPlotConfig& cfg = {}
        );

#   ifdef IMGUIX_DEMO
    /// \brief Engineering demo for DualMetricBarsPlotSet.
    /// \thread_safety Not thread-safe.
    inline void DemoDualMetricBarsPlotSet();
#   endif

} // namespace ImGuiX::Widgets

#include "DualMetricBarsSet.ipp"

#endif // _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_SET_HPP_INCLUDED
