#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_PLOT_IPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_PLOT_IPP_INCLUDED

#include <algorithm>
#include <cstdio>
#include <vector>

namespace ImGuiX::Widgets {

    inline void DualMetricBarsPlot(
            const DualMetricBarsData& data,
            DualMetricBarsPlotState& state,
            const DualMetricBarsPlotConfig& cfg
        ) {
        ImGui::PushID(cfg.id);

        const size_t N = data.labels.size();
        const bool valid =
            data.metric1.size() == N && data.metric2.size() == N;

        const float fit_x = 1.f / float(std::max<size_t>(1, N));
        ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(fit_x, 0.f));
        if (ImPlot::BeginPlot(data.title.c_str())) {
            ImPlot::SetupAxes(data.label_x.c_str(), data.label_y1.c_str());
            ImPlot::SetupAxis(ImAxis_Y2, data.label_y2.c_str(),
                               ImPlotAxisFlags_AuxDefault);

            std::vector<double> positions(N);
            std::vector<const char*> ticks(N);
            for (size_t i = 0; i < N; ++i) {
                positions[i] = double(i);
                ticks[i] = data.labels[i].empty() ? "<Empty>" : data.labels[i].c_str();
            }
            if (N > 0) {
                ImPlot::SetupAxisTicks(ImAxis_X1, positions.data(), int(N), ticks.data());
            }
            ImPlot::SetupAxisLimits(ImAxis_X1, -1.0, double(N));

            double max1 = 0.0, max2 = 0.0;
            if (valid) {
                for (size_t i = 0; i < N; ++i) {
                    max1 = std::max(max1, data.metric1[i]);
                    max2 = std::max(max2, data.metric2[i]);
                }
            }
            double y1_max = cfg.force_y1_percent ? 100.0
                             : std::max(100.0, max1) + cfg.y_indent;
            double y2_max = max2 + cfg.y_indent;
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, y1_max);
            ImPlot::SetupAxisLimits(ImAxis_Y2, 0.0, y2_max);

            if (state.show_legend) {
                ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Horizontal);
            }

            if (valid && N > 0) {
                const float off1 = -cfg.bar_width * 0.5f;
                const float off2 = cfg.bar_width * 0.5f;

                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
                ImPlot::SetNextFillStyle(data.metric1_color);
                ImPlot::PlotBars(data.metric1_name.c_str(),
                                 data.metric1.data(),
                                 int(N), cfg.bar_width, off1);

                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
                ImPlot::SetNextFillStyle(data.metric2_color);
                ImPlot::PlotBars(data.metric2_name.c_str(),
                                 data.metric2.data(),
                                 int(N), cfg.bar_width, off2);

                const char* fmt1 = cfg.metric1_fmt ? cfg.metric1_fmt
                            : (cfg.force_y1_percent ? "%.1f%%" : "%.2f");
                const char* fmt2 = cfg.metric2_fmt;
                bool hov1 = ImPlot::IsLegendEntryHovered(data.metric1_name.c_str());
                bool hov2 = ImPlot::IsLegendEntryHovered(data.metric2_name.c_str());
                bool show1 = state.show_annotations && (!hov2 || hov1);
                bool show2 = state.show_annotations && (!hov1 || hov2);
                char buf[32];
                ImVec4 col1 = data.metric1_color; col1.w = 0.5f;
                ImVec4 col2 = data.metric2_color; col2.w = 0.5f;
                for (size_t i = 0; i < N; ++i) {
                    double px = positions[i];
                    if (show1) {
                        std::snprintf(buf, sizeof buf, fmt1, data.metric1[i]);
                        ImPlot::Annotation(
                                px + off1,
                                data.metric1[i],
                                col1,
                                ImVec2(-5, -5),
                                false,
                                "%s",
                                buf
                        );
                    }
                    if (show2) {
                        std::snprintf(buf, sizeof buf, fmt2, data.metric2[i]);
                        ImPlot::Annotation(
                                px + off2,
                                data.metric2[i],
                                col2,
                                ImVec2(5, -5),
                                false,
                                "%s",
                                buf
                        );
                    }
                }
            }

            if (ImPlot::IsPlotHovered()) {
                ImPlotPoint mouse = ImPlot::GetPlotMousePos();
                static ImPlotDragToolFlags flags =
                    ImPlotDragToolFlags_None | ImPlotDragToolFlags_NoInputs;
                ImPlot::DragLineX(1, &mouse.x, cfg.drag_line_color, 1, flags);
                ImPlot::DragLineY(2, &mouse.y, cfg.drag_line_color, 1, flags);
            }

            ImPlot::EndPlot();
        }
        ImPlot::PopStyleVar();
        ImGui::PopID();
    }

#   ifdef IMGUIX_DEMO
    inline void DemoDualMetricBarsPlot() {
        static DualMetricBarsData data;
        static DualMetricBarsPlotState state;
        static bool init = false;
        if (!init) {
            data.title = "DualMetricBarsPlot Demo";
            data.label_x = "Strategy";
            data.label_y1 = "Winrate";
            data.label_y2 = "Trades";
            data.metric1_name = "Winrate";
            data.metric2_name = "Trades";
            data.labels = {"test-2024","test-2025"};
            data.metric1 = {16.7,50.0};
            data.metric2 = {100,100};
            init = true;
        }
        DualMetricBarsPlotConfig cfg;
        cfg.force_y1_percent = true;
        DualMetricBarsPlot(data, state, cfg);
    }
#   endif

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_PLOT_IPP_INCLUDED
