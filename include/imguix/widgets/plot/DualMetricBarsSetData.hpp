#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_SET_DATA_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_SET_DATA_HPP_INCLUDED

/// \file DualMetricBarsSetData.hpp
/// \brief Collection of DualMetricBarsData for DualMetricBarsPlotSet.

#include <vector>

#include <imguix/widgets/plot/DualMetricBarsData.hpp>

namespace ImGuiX::Widgets {

    /// \brief Data sets for DualMetricBarsPlotSet.
    struct DualMetricBarsSetData {
        std::vector<DualMetricBarsData> sets; ///< Data series collection.
    };

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_SET_DATA_HPP_INCLUDED
