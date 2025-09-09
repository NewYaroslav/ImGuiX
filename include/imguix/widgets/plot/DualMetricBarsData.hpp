#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_DATA_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_DATA_HPP_INCLUDED

/// \file DualMetricBarsData.hpp
/// \brief Data for DualMetricBarsPlot.

#include <string>
#include <vector>

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Data for a dual-metric grouped bar chart.
    /// \invariant labels.size() == metric1.size() == metric2.size().
    struct DualMetricBarsData {
        std::string title;           ///< Plot title.
        std::string label_x;         ///< X axis label.
        std::string label_y1;        ///< Left Y axis label.
        std::string label_y2;        ///< Right Y axis label.
        std::string metric1_name;    ///< Name for left metric.
        std::string metric2_name;    ///< Name for right metric.
        std::vector<std::string> labels;  ///< Category labels.
        std::vector<double> metric1;      ///< Left metric values.
        std::vector<double> metric2;      ///< Right metric values.
        ImVec4 metric1_color{0.16f, 0.66f, 0.13f, 1.0f}; ///< Left metric color.
        ImVec4 metric2_color{0.90f, 0.24f, 0.24f, 1.0f}; ///< Right metric color.
    };

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_DATA_HPP_INCLUDED
