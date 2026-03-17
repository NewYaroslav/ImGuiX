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
    inline void DemoMetricsPlotSet() {
        ImGui::TextUnformatted("MetricsPlotSet / Data sets demo");
        static MetricsPlotSetData data;
        static MetricsPlotSetState state;
        static bool init = false;
        if (!init) {
            MetricsPlotData bars;
            bars.labels = {"A", "B", "C", "D"};
            bars.values = {1.0, 2.0, 3.0, 2.5};
            data.sets.push_back(bars);
            data.labels.emplace_back("Bars sample");

            MetricsPlotData lines;
            lines.labels = {"L1", "L2"};
            const int N = 50;
            lines.line_x.resize(2);
            lines.line_y.resize(2);
            for (int s = 0; s < 2; ++s) {
                lines.line_x[s].resize(N);
                lines.line_y[s].resize(N);
                for (int i = 0; i < N; ++i) {
                    lines.line_x[s][i] = i;
                    lines.line_y[s][i] = (s + 1) * 0.5 + (i % 10) * 0.1;
                }
            }
            data.sets.push_back(lines);
            data.labels.emplace_back("Lines sample");

            init = true;
        }
        MetricsPlotConfig cfg;
        cfg.id = "MetricsPlotSet_Demo";
        cfg.left_list_header = "Data sets";
        MetricsPlotSet(data, state, cfg);
    }
#   endif

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#include "MetricsPlotSet.ipp"
#endif

#endif // _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_SET_HPP_INCLUDED
