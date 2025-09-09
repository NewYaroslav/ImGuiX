#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_SET_IPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_SET_IPP_INCLUDED

namespace ImGuiX::Widgets {

    inline void DualMetricBarsPlotSet(
            const DualMetricBarsSetData& data,
            DualMetricBarsSetState& state,
            const DualMetricBarsPlotConfig& cfg
        ) {
        ImGui::PushID(cfg.id);

        const float list_w = CalcMetricsDndWidth();
        const float item_h = ImGui::GetTextLineHeightWithSpacing();

        if (state.selected.size() != data.sets.size()) {
            state.selected.assign(data.sets.size(), false);
            state.plot_state.update_counter = 0;
        }

        ImGui::BeginGroup();
        {
            ImGui::BeginChild("##left", ImVec2(list_w, 200), ImGuiChildFlags_Borders);
            if (ImGui::Button("Select All")) {
                for (bool& b : state.selected) b = true;
                state.plot_state.update_counter = 0;
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset")) {
                for (bool& b : state.selected) b = false;
                state.plot_state.update_counter = 0;
            }
            ImGui::Separator();
            for (size_t i = 0; i < data.sets.size(); ++i) {
                bool sel = state.selected[i];
                if (ImGui::Selectable(data.sets[i].title.c_str(), sel,
                                      ImGuiSelectableFlags_SpanAvailWidth,
                                      ImVec2(0,item_h))) {
                    state.selected[i] = !sel;
                    state.plot_state.update_counter = 0;
                }
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();

        size_t first = data.sets.size();
        for (size_t i = 0; i < state.selected.size(); ++i) {
            if (state.selected[i]) { first = i; break; }
        }
        if (first < data.sets.size()) {
            DualMetricBarsPlot(data.sets[first], state.plot_state, cfg);
        } else {
            DualMetricBarsData empty;
            empty.title = data.sets.empty() ? "" : data.sets[0].title;
            DualMetricBarsPlot(empty, state.plot_state, cfg);
        }

        ImGui::EndGroup();
        ImGui::PopID();
    }

#   ifdef IMGUIX_DEMO
    inline void DemoDualMetricBarsPlotSet() {
        static DualMetricBarsSetData data;
        static DualMetricBarsSetState state;
        static bool init = false;
        if (!init) {
            DualMetricBarsData a;
            a.title = "Strategy/Result";
            a.label_x = "Strategy";
            a.label_y1 = "Winrate";
            a.label_y2 = "Trades";
            a.metric1_name = "Winrate";
            a.metric2_name = "Trades";
            a.labels = {"A","B"};
            a.metric1 = {42.0,55.0};
            a.metric2 = {120,80};
            data.sets.push_back(a);
            DualMetricBarsData b = a;
            b.title = "Strategy/Result 2";
            b.metric1 = {30.0,20.0};
            b.metric2 = {90,110};
            data.sets.push_back(b);
            init = true;
        }
        DualMetricBarsPlotSet(data, state);
    }
#   endif

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PLOT_DUAL_METRIC_BARS_SET_IPP_INCLUDED
