#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_DATA_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_DATA_HPP_INCLUDED

/// \file MetricsPlotData.hpp
/// \brief Data container for MetricsPlot widget.

#include <string>
#include <vector>

namespace ImGuiX::Widgets {

    /// \brief Data for MetricsPlot.
    /// \invariant labels.size() equals values.size() when values not empty.
    /// \invariant line_x[k].size() equals line_y[k].size() for all k; mismatched series are skipped.
    struct MetricsPlotData {
        std::vector<std::string> labels;         ///< Category labels.
        std::vector<double> values;              ///< Bar values per category.
        std::vector<std::vector<double>> line_x; ///< Line X coordinates.
        std::vector<std::vector<double>> line_y; ///< Line Y coordinates.
        std::vector<std::string> tick_labels_x;  ///< X tick labels (optional).
    };

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_DATA_HPP_INCLUDED
