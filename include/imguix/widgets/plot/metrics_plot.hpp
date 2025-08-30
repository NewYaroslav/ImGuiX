#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_HPP_INCLUDED

/// \file metrics_plot.hpp
/// \brief Plot categorical metrics as bars or lines with drag-and-drop.

#include <string_view>
#include <vector>

#include <imgui.h>
#include <implot.h>

namespace ImGuiX::Widgets {

/// \brief Configuration for MetricsPlot.
struct MetricsPlotConfig {
  const char *id = "MetricsPlot";           ///< Plot identifier.
  const char *x_label = "X";                ///< X axis label.
  const char *y_label = "Y";                ///< Y axis label.
  const char *dnd_payload = "METRICS_PLOT"; ///< Drag-n-drop payload ID.
  const char *annotation_checkbox =
      u8"Annotations";                        ///< Annotation checkbox label.
  const char *legend_checkbox = u8"Legend";   ///< Legend checkbox label.
  const char *button_show_all = u8"Show all"; ///< Select all button label.
  const char *button_reset = u8"Reset";       ///< Reset selection button label.
  const char *value_fmt = "%+.2f";            ///< Format for values.
  const char *label_value_fmt = "%s = %+.2f"; ///< Annotation format.
  float plot_height = 256.0f;                 ///< Height of the plot area.
  float dnd_width = 0.0f;                     ///< Width of DND list (0 = auto).
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

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#include "metrics_plot.ipp"
#endif

#endif // _IMGUIX_WIDGETS_PLOT_METRICS_PLOT_HPP_INCLUDED
