#include <algorithm>
#include <cmath>

#include <imgui.h>
#include <implot.h>

namespace ImGuiX::Widgets {

    namespace detail {
        // reuse kUpdateCounterMax and calc_plot_height from MetricsPlot
    }

    inline void MetricsPlotSet(
            const MetricsPlotSetData& data,
            MetricsPlotSetState& state,
            const MetricsPlotConfig& cfg
        ) {
        ImGui::PushID(cfg.id);

        const float dnd_w = cfg.dnd_width > 0.0f ? cfg.dnd_width : CalcMetricsDndWidth();
        const float plot_h = IM_TRUNC(detail::calc_plot_height(cfg));

        if (!data.sets.empty()) {
            if (state.selected < 0 || state.selected >= (int)data.sets.size()) {
                state.selected = 0;
                state.plot_state.initialized = false;
            }
        }

        ImGui::BeginGroup();
        {
            const ImGuiStyle& st = ImGui::GetStyle();
            if (cfg.left_panel_bordered) {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, st.Colors[ImGuiCol_FrameBg]);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, st.FrameRounding);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, st.FrameBorderSize);
            }

            ImGui::BeginChild("##child_left", ImVec2(dnd_w, plot_h), ImGuiChildFlags_Borders);
            {
                if (!data.sets.empty() && !data.sets[state.selected].values.empty()) {
                    ImGui::Checkbox(cfg.annotation_checkbox, &state.plot_state.show_annotation);
                }
                if (!cfg.legend_force_off) {
                    ImGui::Checkbox(cfg.legend_checkbox, &state.plot_state.show_legend);
                }

                if (cfg.left_list_header) ImGui::SeparatorText(cfg.left_list_header);
                else ImGui::Separator();

                ImGuiChildFlags list_flags = cfg.list_bordered ? ImGuiChildFlags_Borders : ImGuiChildFlags_None;
                ImGui::BeginChild("##sets_scroll", ImVec2(0,0), list_flags);
                {
                    for (int i = 0; i < (int)data.sets.size(); ++i) {
                        bool selected = (i == state.selected);
                        if (ImGui::Selectable(data.labels[i].c_str(), selected, 0, ImVec2(dnd_w,0))) {
                            state.selected = i;
                            state.plot_state.initialized = false;
                            state.plot_state.update_counter = detail::kUpdateCounterMax;
                        }
                    }
                }
                ImGui::EndChild();
            }
            ImGui::EndChild();

            if (cfg.left_panel_bordered) {
                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor();
            }
        }

        ImGui::SameLine();

        if (!data.sets.empty()) {
            MetricsPlotConfig inner_cfg = cfg;
            inner_cfg.plot_height = plot_h;
            inner_cfg.force_all_visible = true;
            const MetricsPlotData& sel = data.sets[state.selected];
            MetricsPlot(sel, state.plot_state, inner_cfg);
        }

        ImGui::EndGroup();

        ImGui::PopID();
    }

#   ifdef IMGUIX_DEMO
    inline void DemoMetricsPlotSet() {
        ImGui::TextUnformatted("MetricsPlotSet / Data sets demo");
        static MetricsPlotSetData data;
        static MetricsPlotSetState state;
        static bool init = false;
        if (!init) {
            // Set 1: bars
            MetricsPlotData bars;
            const char* kLabels[] = {"A","B","C","D"};
            bars.labels.assign(std::begin(kLabels), std::end(kLabels));
            bars.values = {1.0, 2.0, 3.0, 2.5};
            data.sets.push_back(bars);
            data.labels.emplace_back("Bars sample");

            // Set 2: lines
            MetricsPlotData lines;
            lines.labels = {"L1","L2"};
            const int N = 50;
            lines.line_x.resize(2);
            lines.line_y.resize(2);
            for (int s=0; s<2; ++s) {
                lines.line_x[s].resize(N);
                lines.line_y[s].resize(N);
                for (int i=0; i<N; ++i) {
                    lines.line_x[s][i] = i;
                    lines.line_y[s][i] = (s+1)*0.5 + std::sin(0.1*i + s);
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

