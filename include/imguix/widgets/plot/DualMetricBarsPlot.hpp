#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_PLOT_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_PLOT_HPP_INCLUDED

/// \file DualMetricBarsPlot.hpp
/// \brief Render grouped bars with two metrics and two Y axes.

#include <string>

#include <imgui.h>
#include <implot.h>

#include <imguix/widgets/plot/DualMetricBarsData.hpp>

namespace ImGuiX::Widgets {

    /// \brief Configuration for DualMetricBarsPlot.
    struct DualMetricBarsPlotConfig {
        const char* id = "DualMetricBarsPlot"; ///< Plot identifier.
        bool force_y1_percent = false;          ///< Clamp left axis to [0,100].
        const char* metric1_fmt = nullptr;      ///< Format for left annotations.
        const char* metric2_fmt = "%.0f";      ///< Format for right annotations.
        float y_indent = 0.0f;                  ///< Top indent for Y limits.
        float bar_width = 0.4f;                 ///< Width of each bar.
        ImVec4 drag_line_color{1.0f,1.0f,1.0f,0.5f}; ///< Crosshair color.
    };

    /// \brief Runtime state for DualMetricBarsPlot.
    struct DualMetricBarsPlotState {
        bool show_annotations = false; ///< Show annotations above bars.
        bool show_legend = true;       ///< Show legend.
        int update_counter = 0;        ///< Auto-fit counter.
    };

    /// \brief Render dual-metric grouped bars.
    /// \param data Input data arrays.
    /// \param state Runtime state (modified).
    /// \param cfg Plot configuration.
    /// \thread_safety Not thread-safe.
    void DualMetricBarsPlot(
            const DualMetricBarsData& data,
            DualMetricBarsPlotState& state,
            const DualMetricBarsPlotConfig& cfg = {}
        );

#   ifdef IMGUIX_DEMO
    /// \brief Engineering demo for DualMetricBarsPlot.
    /// \thread_safety Not thread-safe.
    inline void DemoDualMetricBarsPlot();
#   endif

} // namespace ImGuiX::Widgets

#include "DualMetricBarsPlot.ipp"

#endif // _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_PLOT_HPP_INCLUDED
