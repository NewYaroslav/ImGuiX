#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_HPP_INCLUDED

/// \file MetricsPlot.hpp
/// \brief Plot categorical metrics as bars or lines with drag-and-drop.

#include <string_view>
#include <vector>

#include <imgui.h>
#include <implot.h>

namespace ImGuiX::Widgets {

    /// \brief Configuration for MetricsPlot.
    struct MetricsPlotConfig {
        const char *id = "MetricsPlot";             ///< Plot identifier.
        const char *x_label = u8"X";                ///< X axis label.
        const char *y_label = u8"Y";                ///< Y axis label.
        const char *dnd_payload = u8"METRICS_PLOT"; ///< Drag-n-drop payload ID.
        const char *annotation_checkbox = u8"Annotations"; ///< Annotation checkbox label.
        const char *legend_checkbox = u8"Legend";   ///< Legend checkbox label.
        const char *button_show_all = u8"Show all"; ///< Select all button label.
        const char *button_reset = u8"Reset";       ///< Reset selection button label.
        const char* left_list_header = nullptr;     ///< e.g. "Series"; nullptr shows plain line.
        const char* settings_header = u8"Chart settings"; ///< Settings section header.
        const char *value_fmt = "%+.2f";            ///< Format for values.
        const char *label_value_fmt = "%s = %+.2f"; ///< Annotation format.
        bool list_bordered       = false;           ///< Inner border for scroll list.
        bool y_axis_right        = true;            ///< Draw Y axis labels on the right.
        bool left_panel_bordered = true;            ///< Border or background for left checkbox panel.
        bool cap_by_avail_y      = false;           ///< Limit height by available space.
        bool show_tooltip        = true;            ///< Enable tooltips.
        float bar_tooltip_snap_factor = 0.55f;      ///< Bars: X tolerance as fraction of bar width (0..1). 0.5 equals half width.
        float line_tooltip_snap_px    = 18.0f;      ///< Lines: tolerance in pixels to nearest point.
        float plot_height     = 0.0f;               ///< Height of the plot area.
        float aspect_w_over_h = 2.0f;               ///< Width to height ratio (2:1 default).
        float auto_height_min = 0.0f;               ///< Lower bound for auto height.
        float auto_height_max = 0.0f;               ///< 0 disables upper bound.
        float dnd_width       = 0.0f;               ///< Width of DND list (0 = auto).
        ImVec2 fit_padding{0.3f, 0.5f};             ///< Fit padding for auto-fit.
        int max_visible = 8; ///< Max items before legend disabled.
        ImVec4 drag_line_color{1.0f, 1.0f, 1.0f, 0.5f}; ///< Crosshair color.
        ImVec4 plot_color{0.012f, 0.66f, 0.95f, 1.0f};  ///< Default bar fill color.
    };

    /// \brief Data for MetricsPlot.
    struct MetricsPlotData {
        std::vector<std::string_view> labels;    ///< Category labels.
        std::vector<double> values;              ///< Bar values per category.
        std::vector<std::vector<double>> line_x; ///< Line X coordinates.
        std::vector<std::vector<double>> line_y; ///< Line Y coordinates.
    };

    /// \brief Runtime state for MetricsPlot.
    struct MetricsPlotState {
        struct DndItem {
            size_t index = 0;       ///< Index in data arrays.
            bool is_plot = false;   ///< True if item plotted.
            std::string_view label; ///< Display label.
            ImVec4 color{};         ///< Item color.
        };

        std::vector<DndItem> dnd;    ///< DND items.
        bool show_annotation = true; ///< Show value annotations.
        bool show_legend = true;     ///< Show legend.
        int update_counter = 0;      ///< Auto-fit counter.
    };

    /// \brief Calculate default DND list width.
    /// \return Recommended width in pixels.
    float CalcMetricsDndWidth();

    /// \brief Calculate button width inside DND list.
    /// \param dnd_width Width of the DND list.
    /// \return Button width.
    float CalcMetricsDndButtonWidth(float dnd_width);

    /// \brief Render metrics plot with drag-and-drop selection.
    /// \param data Input data arrays.
    /// \param state Runtime state (modified in-place).
    /// \param cfg Widget configuration.
    void MetricsPlot(const MetricsPlotData &data, MetricsPlotState &state,
                     const MetricsPlotConfig &cfg = {});
                     
    /// \brief Engineering demo for MetricsPlot.
    /// \details Shows: (1) categorical bars; (2) time-series lines.
    /// \note Uses static state to persist selections between frames.
    inline void DemoMetricsPlot() {
        // ---------- 1) Bars: subsystem latency ----------
        ImGui::TextUnformatted("MetricsPlot / Bars: Subsystem latency");
        {
            static MetricsPlotData  data;
            static MetricsPlotState state;
            static bool init = false;

            if (!init) {
                // Categories (subsystems)
                const char* kLabels[] = {
                    "ADC","DSP","IO","CTRL","UI","NET","LOG","FS","DB","PHY","RF","PMU"
                };
                data.labels.assign(std::begin(kLabels), std::end(kLabels));
                data.values.resize(data.labels.size());
                // Synthetic values in ms; some negative for demo
                for (size_t i = 0; i < data.values.size(); ++i)
                    data.values[i] = (i % 2 ? +1.0 : -1.0) * (5.0 + 3.0 * std::sin(0.45 * double(i)));
                init = true;
            }

            MetricsPlotConfig cfg;
            cfg.id           = "MetricsPlot_Bars";
            cfg.x_label      = "Subsystem";
            cfg.left_list_header = "Series";
            cfg.y_label      = "Latency, ms";
            cfg.plot_height  = 0.0f;
            cfg.max_visible  = 8;            // >8 disables legend
            cfg.value_fmt    = "%+.2f";
            cfg.label_value_fmt = "%s = %+.2f";

            MetricsPlot(data, state, cfg);
        }

        // ---------- 2) Lines: temperatures over time ----------
        ImGui::TextUnformatted("MetricsPlot / Lines: Sensor temperatures");
        {
            static MetricsPlotData  data;
            static MetricsPlotState state;
            static bool init = false;

            if (!init) {
                const int series = 3;  // three sensors
                data.labels = { "Temp A", "Temp B", "Temp C" };
                data.line_x.resize(series);
                data.line_y.resize(series);

                const int N = 120; // ~120 time samples
                for (int s = 0; s < series; ++s) {
                    data.line_x[s].resize(N);
                    data.line_y[s].resize(N);
                    for (int i = 0; i < N; ++i) {
                        data.line_x[s][i] = double(i); // time in seconds
                        data.line_y[s][i] = 25.0 + 2.0 * s + 5.0 * std::sin(0.08 * i + 0.6 * s);
                    }
                }
                init = true;
            }

            MetricsPlotConfig cfg;
            cfg.id           = "MetricsPlot_Lines";
            cfg.x_label      = "Time, s";
            cfg.y_label      = "Temperature, °C";
            cfg.plot_height  = 0.0f;
            cfg.max_visible  = 8;
            cfg.value_fmt    = "%.1f";

            // Line mode enabled when data.values is empty
            data.values.clear();
            MetricsPlot(data, state, cfg);
        }
    }

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#include "MetricsPlot.ipp"
#endif

#endif // _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_HPP_INCLUDED
