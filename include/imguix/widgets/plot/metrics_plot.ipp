#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#include <imgui.h>
#include <implot.h>

namespace ImGuiX::Widgets {

inline float CalcMetricsDndWidth() {
  const ImGuiStyle &st = ImGui::GetStyle();
  return ImGui::GetFontSize() * 8.0f + st.FramePadding.x * 2.0f;
}

inline float CalcMetricsDndButtonWidth(float dnd_width) {
  const ImGuiStyle &st = ImGui::GetStyle();
  return dnd_width - st.FramePadding.x * 2.0f;
}

inline void MetricsPlot(const MetricsPlotData &data, MetricsPlotState &state,
                        const MetricsPlotConfig &cfg) {
  constexpr int kUpdateCounterMax = 20;

  const float dnd_w =
      cfg.dnd_width > 0.0f ? cfg.dnd_width : CalcMetricsDndWidth();
  const float btn_w = CalcMetricsDndButtonWidth(dnd_w);

  const std::string left_id = std::string(cfg.id) + "_left";
  const std::string right_id = std::string(cfg.id) + "_right";

  // sync dnd items with data
  state.dnd.resize(data.labels.size());
  for (size_t i = 0; i < data.labels.size(); ++i) {
    auto &item = state.dnd[i];
    item.index = i;
    item.label = data.labels[i];
    item.color = ImPlot::GetColormapColor(static_cast<int>(i));
  }

  ImGui::BeginGroup();
  ImGui::BeginChild(left_id.c_str(), ImVec2(dnd_w, cfg.plot_height + 8.0f));
  {
    int plotted = 0;
    for (const auto &it : state.dnd) {
      if (it.is_plot)
        ++plotted;
    }

    if (!data.values.empty()) {
      if (ImGui::Checkbox(cfg.annotation_checkbox, &state.show_annotation)) {
        state.update_counter = kUpdateCounterMax;
      }
    }

    if (!state.show_legend && plotted >= cfg.max_visible) {
      ImGui::BeginDisabled(true);
      ImGui::Checkbox(cfg.legend_checkbox, &state.show_legend);
      ImGui::EndDisabled();
    } else {
      ImGui::Checkbox(cfg.legend_checkbox, &state.show_legend);
    }

    if (ImGui::Button(cfg.button_show_all, ImVec2(btn_w, 0))) {
      if (state.dnd.size() >= static_cast<size_t>(cfg.max_visible)) {
        state.show_legend = false;
      }
      for (auto &it : state.dnd)
        it.is_plot = true;
      state.update_counter = kUpdateCounterMax;
    }

    if (ImGui::Button(cfg.button_reset, ImVec2(btn_w, 0))) {
      for (auto &it : state.dnd)
        it.is_plot = false;
    }

    ImGui::NewLine();

    for (size_t k = 0; k < state.dnd.size(); ++k) {
      auto &it = state.dnd[k];
      if (it.is_plot)
        continue;
      ImPlot::ItemIcon(it.color);
      if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(it.label.data());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
      }
      ImGui::SameLine();
      ImGui::Selectable(it.label.data(), false, 0, ImVec2(dnd_w, 0));
      if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(it.label.data());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
      }
      if (!state.show_legend || plotted < cfg.max_visible) {
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
          ImGui::SetDragDropPayload(cfg.dnd_payload, &k, sizeof(int));
          ImPlot::ItemIcon(it.color);
          ImGui::SameLine();
          ImGui::TextUnformatted(it.label.data());
          ImGui::EndDragDropSource();
        }
      }
    }
  }
  ImGui::EndChild();

  if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload *payload =
            ImGui::AcceptDragDropPayload(cfg.dnd_payload)) {
      int i = *static_cast<const int *>(payload->Data);
      if (i >= 0 && static_cast<size_t>(i) < state.dnd.size()) {
        state.dnd[i].is_plot = false;
      }
    }
    ImGui::EndDragDropTarget();
  }

  ImGui::SameLine();

  ImGui::BeginChild(right_id.c_str(), ImVec2(-1, cfg.plot_height + 8.0f));
  {
    int flags = state.show_legend
                    ? ImPlotFlags_AntiAliased
                    : (ImPlotFlags_AntiAliased | ImPlotFlags_NoLegend);

    if (state.update_counter > 0) {
      if (state.update_counter == kUpdateCounterMax ||
          state.update_counter == 1) {
        ImPlot::SetNextAxisToFit(ImAxis_X1);
        ImPlot::SetNextAxisToFit(ImAxis_Y1);
      }
      --state.update_counter;
    }

    ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, cfg.fit_padding);
    if (ImPlot::BeginPlot(cfg.id, ImVec2(-1, cfg.plot_height), flags)) {
      ImPlot::SetupLegend(ImPlotLocation_West);

      if (data.values.empty()) {
        ImPlot::SetupAxes(cfg.x_label, cfg.y_label, ImPlotAxisFlags_Time);
        ImPlot::SetupAxisFormat(ImAxis_Y1, cfg.value_fmt);
        for (size_t k = 0; k < state.dnd.size(); ++k) {
          auto &it = state.dnd[k];
          if (!it.is_plot)
            continue;
          ImPlot::SetAxis(ImAxis_Y1);
          ImPlot::SetNextLineStyle(it.color);
          ImPlot::PlotLine(it.label.data(), data.line_x[k].data(),
                           data.line_y[k].data(),
                           static_cast<int>(data.line_y[k].size()));
          if (ImPlot::BeginDragDropSourceItem(it.label.data())) {
            ImGui::SetDragDropPayload(cfg.dnd_payload, &k, sizeof(int));
            ImPlot::ItemIcon(it.color);
            ImGui::SameLine();
            ImGui::TextUnformatted(it.label.data());
            ImPlot::EndDragDropSource();
          }
        }
      } else {
        ImPlot::SetupAxes(cfg.x_label, cfg.y_label);
        ImPlot::SetupAxisFormat(ImAxis_Y1, cfg.value_fmt);
        ImPlot::SetupAxisFormat(ImAxis_X1, "");

        std::vector<char *> labels_ptr(state.dnd.size());
        for (size_t k = 0; k < state.dnd.size(); ++k) {
          labels_ptr[k] = const_cast<char *>(state.dnd[k].label.data());
        }

        for (size_t k = 0, i = 0; k < state.dnd.size(); ++k) {
          auto &it = state.dnd[k];
          if (!it.is_plot)
            continue;
          double pos = static_cast<double>(i);
          ImPlot::SetAxis(ImAxis_Y1);
          if (state.show_annotation) {
            ImVec4 col = it.color;
            col.w = 0.5f;
            if (data.values[k] >= 0) {
              ImPlot::Annotation(pos, data.values[k], col, ImVec2(0, -5), false,
                                 cfg.label_value_fmt, labels_ptr[k],
                                 data.values[k]);
            } else {
              ImPlot::Annotation(pos, data.values[k], col, ImVec2(0, 5), false,
                                 cfg.label_value_fmt, labels_ptr[k],
                                 data.values[k]);
            }
          }
          ImPlot::SetNextFillStyle(it.color);
          ImPlot::PlotBars(it.label.data(), &pos, &data.values[k], 1, 0.67);
          if (ImPlot::BeginDragDropSourceItem(it.label.data())) {
            ImGui::SetDragDropPayload(cfg.dnd_payload, &k, sizeof(int));
            ImPlot::ItemIcon(it.color);
            ImGui::SameLine();
            ImGui::TextUnformatted(it.label.data());
            ImPlot::EndDragDropSource();
          }
          ++i;
        }
      }

      if (ImPlot::BeginDragDropTargetPlot()) {
        if (const ImGuiPayload *payload =
                ImGui::AcceptDragDropPayload(cfg.dnd_payload)) {
          int i = *static_cast<const int *>(payload->Data);
          if (i >= 0 && static_cast<size_t>(i) < state.dnd.size()) {
            state.dnd[i].is_plot = true;
            state.update_counter = kUpdateCounterMax;
          }
        }
        ImPlot::EndDragDropTarget();
      }
      if (ImPlot::BeginDragDropTargetLegend()) {
        if (const ImGuiPayload *payload =
                ImGui::AcceptDragDropPayload(cfg.dnd_payload)) {
          int i = *static_cast<const int *>(payload->Data);
          if (i >= 0 && static_cast<size_t>(i) < state.dnd.size()) {
            state.dnd[i].is_plot = true;
            state.update_counter = kUpdateCounterMax;
          }
        }
        ImPlot::EndDragDropTarget();
      }
      if (ImPlot::IsPlotHovered()) {
        ImPlotPoint mouse_pos = ImPlot::GetPlotMousePos();
        static ImPlotDragToolFlags tool_flags =
            ImPlotDragToolFlags_None | ImPlotDragToolFlags_NoInputs;
        ImPlot::DragLineX(1, &mouse_pos.x, cfg.drag_line_color, 1, tool_flags);
        ImPlot::DragLineY(2, &mouse_pos.y, cfg.drag_line_color, 1, tool_flags);
      }
      ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar();
  }
  ImGui::EndChild();
  ImGui::EndGroup();
}

} // namespace ImGuiX::Widgets
