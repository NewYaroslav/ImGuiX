#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_SET_DATA_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_SET_DATA_HPP_INCLUDED

/// \file MetricsPlotSetData.hpp
/// \brief Data container for a collection of MetricsPlotData.

#include <string>
#include <vector>

#include <imguix/widgets/plot/MetricsPlotData.hpp>

namespace ImGuiX::Widgets {

    /// \brief Data sets with display labels for MetricsPlotSet.
    struct MetricsPlotSetData {
        std::vector<MetricsPlotData> sets; ///< Collection of data sets.
        std::vector<std::string> labels;   ///< Labels for each data set.
    };

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_SET_DATA_HPP_INCLUDED
