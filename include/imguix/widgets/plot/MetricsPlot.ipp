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

    inline void MetricsPlot(
            const MetricsPlotData &data, 
            MetricsPlotState &state,
            const MetricsPlotConfig &cfg
        ) {
        ImGui::PushID(cfg.id);

        constexpr int kUpdateCounterMax = 20;
        const float dnd_w = cfg.dnd_width > 0.0f ? 
            cfg.dnd_width : CalcMetricsDndWidth();
        const float btn_w = CalcMetricsDndButtonWidth(dnd_w);

        // sync dnd items with data
        state.dnd.resize(data.labels.size());
        for (size_t i = 0; i < data.labels.size(); ++i) {
            auto &item = state.dnd[i];
            item.index = i;
            item.label = data.labels[i];
            item.color = ImPlot::GetColormapColor(static_cast<int>(i));
        }

        ImGui::BeginGroup();
        
        if (cfg.left_panel_bordered) {
            const ImGuiStyle &st = ImGui::GetStyle();
            ImGui::PushStyleColor(ImGuiCol_ChildBg, st.Colors[ImGuiCol_FrameBg]);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, st.FrameRounding);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, st.FrameBorderSize);
        }
        
        auto ResolvePlotHeight = [&](const MetricsPlotConfig& cfg)->float {
            if (cfg.plot_height > 0.0f) return cfg.plot_height;
            const float avail_w = ImGui::GetContentRegionAvail().x;
            const float avail_y = ImGui::GetContentRegionAvail().y;
            float h = avail_w / ImMax(0.5f, cfg.aspect_w_over_h);
            float auto_height_min = cfg.auto_height_min > 0.0 ?
                cfg.auto_height_min : 
                ImPlot::GetStyle().PlotDefaultSize.y;
            h = ImMax(h, auto_height_min);
            if (cfg.auto_height_max > 0.0f) h = ImMin(h, cfg.auto_height_max);
            return h;
        };
        
        auto IsSeriesVisible = [&](const char* label)->bool {
            if (ImPlotItem* it = ImPlot::GetItem(label)) return it->Show;
            return true;
        };
        
        const float plot_h = IM_TRUNC(ResolvePlotHeight(cfg));

        // --- child_left ---
        int plotted = 0;
        ImGui::BeginChild("##child_left", ImVec2(dnd_w, plot_h), ImGuiChildFlags_Borders);
        {
            for (const auto &it : state.dnd) {
                if (it.is_plot) ++plotted;
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
                for (auto &it : state.dnd) it.is_plot = true;
                state.update_counter = kUpdateCounterMax;
            }

            if (ImGui::Button(cfg.button_reset, ImVec2(btn_w, 0))) {
                for (auto &it : state.dnd) it.is_plot = false;
            }

            if (cfg.left_list_header) ImGui::SeparatorText(cfg.left_list_header);
            else ImGui::Separator();

            ImGuiChildFlags list_flags = ImGuiChildFlags_None;
            if (cfg.list_bordered) list_flags |= ImGuiChildFlags_Borders;
            ImGui::BeginChild("##dnd_scroll", ImVec2(0,0), list_flags);
            {
                for (size_t k = 0; k < state.dnd.size(); ++k) {
                    auto &it = state.dnd[k];
                    if (it.is_plot) continue;
                    
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
                
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(cfg.dnd_payload)) {
                        int i = *static_cast<const int *>(payload->Data);
                        if (i >= 0 && static_cast<size_t>(i) < state.dnd.size()) {
                            state.dnd[i].is_plot = false;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        if (cfg.left_panel_bordered) {
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor();
        }

        ImGui::SameLine();

        // --- right panel ---
        const ImVec4 bg = ImPlot::GetStyle().Colors[ImPlotCol_FrameBg];
        ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
        ImGui::BeginChild("##child_right", ImVec2(-1, plot_h), ImGuiChildFlags_Borders);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        {
            int flags = ImPlotFlags_None | ImPlotFlags_NoMenus | ImPlotFlags_NoFrame;
            flags |= state.show_legend ? ImPlotFlags_None : ImPlotFlags_NoLegend;

            if (state.update_counter > 0) {
                if (state.update_counter == kUpdateCounterMax ||
                    state.update_counter == 1) {
                    ImPlot::SetNextAxisToFit(ImAxis_X1);
                    ImPlot::SetNextAxisToFit(ImAxis_Y1);
                }
                --state.update_counter;
            }

            ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, cfg.fit_padding);
            if (ImPlot::BeginPlot(cfg.id, ImVec2(-1, plot_h), flags)) {
               ImPlot::SetupLegend(ImPlotLocation_West);

                if (data.values.empty()) {
                    ImPlotAxisFlags y_flags = cfg.y_axis_right ? ImPlotAxisFlags_Opposite : ImPlotAxisFlags_None;
                    ImPlot::SetupAxis(ImAxis_Y1, cfg.y_label, y_flags);
                    ImPlot::SetupAxis(ImAxis_X1, cfg.x_label);
                    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
                    ImPlot::SetupAxisFormat(ImAxis_Y1, cfg.value_fmt);
                    
                    for (size_t k = 0; k < state.dnd.size(); ++k) {
                        auto &it = state.dnd[k];
                        if (!it.is_plot) continue;
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
                    
                    // --- tooltip for lines (nearest point) ---
                    if (cfg.show_tooltip && ImPlot::IsPlotHovered()) {
                        const ImPlotPoint mp  = ImPlot::GetPlotMousePos();
                        const ImVec2      mp_px = ImPlot::PlotToPixels(mp);

                        int best_k = -1, best_i = -1;
                        float best_d2 = std::numeric_limits<float>::max();
                        const float max_d2 = cfg.line_tooltip_snap_px * cfg.line_tooltip_snap_px;

                        for (size_t k = 0; k < state.dnd.size(); ++k) {
                            const auto& it = state.dnd[k];
                            if (!it.is_plot) continue;

                            // серия скрыта легендой? — пропускаем
                            if (!IsSeriesVisible(it.label.data())) continue;

                            const auto& X = data.line_x[k];
                            const auto& Y = data.line_y[k];
                            if (X.empty() || X.size() != Y.size()) continue;

                            // ближайший индекс по X (lower_bound)
                            auto itx = std::lower_bound(X.begin(), X.end(), mp.x);
                            int idx = (int)std::distance(X.begin(), itx);
                            int cand[3] = { ImClamp(idx-1,0,(int)X.size()-1),
                                            ImClamp(idx,  0,(int)X.size()-1),
                                            ImClamp(idx+1,0,(int)X.size()-1) };

                            for (int c : cand) {
                                ImVec2 pt_px = ImPlot::PlotToPixels(ImPlotPoint(X[c], Y[c]));
                                float d2 = (pt_px.x - mp_px.x)*(pt_px.x - mp_px.x) + (pt_px.y - mp_px.y)*(pt_px.y - mp_px.y);
                                if (d2 < best_d2) { best_d2 = d2; best_k = (int)k; best_i = c; }
                            }
                        }

                        if (best_k >= 0 && best_i >= 0 && best_d2 <= max_d2) {
                            const auto& it = state.dnd[best_k];
                            const double x = data.line_x[best_k][best_i];
                            const double y = data.line_y[best_k][best_i];

                            char vbuf[64];
                            ImFormatString(vbuf, IM_ARRAYSIZE(vbuf), cfg.value_fmt, y);

                            if (ImGui::BeginTooltip()) {
                                ImPlot::ItemIcon(it.color);
                                ImGui::SameLine();
                                ImGui::TextUnformatted(it.label.data());
                                ImGui::Separator();
                                ImGui::Text("x: %.6g", x);           // при желании заменить на FormatDateTime
                                ImGui::Text("%s: %s", cfg.y_label ? cfg.y_label : "Value", vbuf);
                                ImGui::EndTooltip();
                            }
                        }
                    }

                } else {
                    ImPlotAxisFlags y_flags = cfg.y_axis_right ? ImPlotAxisFlags_Opposite : ImPlotAxisFlags_None;
                    ImPlot::SetupAxis(ImAxis_Y1, cfg.y_label, y_flags);
                    ImPlot::SetupAxis(ImAxis_X1, cfg.x_label);
                    ImPlot::SetupAxisFormat(ImAxis_Y1, cfg.value_fmt);
                    ImPlot::SetupAxisFormat(ImAxis_X1, "");
                    
                    const double bar_w = 0.67;

                    std::vector<char *> labels_ptr(state.dnd.size());
                    for (size_t k = 0; k < state.dnd.size(); ++k) {
                        labels_ptr[k] = const_cast<char *>(state.dnd[k].label.data());
                    }  // for k

                    for (size_t k = 0, i = 0; k < state.dnd.size(); ++k) {
                        auto &it = state.dnd[k];
                        if (!it.is_plot) continue;
                        double pos = static_cast<double>(i);
                        ImPlot::SetAxis(ImAxis_Y1);
                        ImPlot::SetNextFillStyle(it.color);
                        ImPlot::PlotBars(it.label.data(), &pos, &data.values[k], 1, bar_w);
                        // рисуем анотацию
                        if (state.show_annotation && IsSeriesVisible(it.label.data())) {
                            ImVec4 col = it.color; col.w = 0.5f;
                            const ImVec2 off = (data.values[k] >= 0) ? ImVec2(0,-5) : ImVec2(0,5);
                            ImPlot::Annotation(
                                pos, data.values[k], col, off, false,
                                cfg.label_value_fmt, labels_ptr[k], data.values[k]
                            );
                        }
                        
                        if (ImPlot::BeginDragDropSourceItem(it.label.data())) {
                            ImGui::SetDragDropPayload(cfg.dnd_payload, &k, sizeof(int));
                            ImPlot::ItemIcon(it.color);
                            ImGui::SameLine();
                            ImGui::TextUnformatted(it.label.data());
                            ImPlot::EndDragDropSource();
                        }
                        ++i;
                    } // for k
                    
                    // --- tooltip for bars ---
                    if (cfg.show_tooltip && ImPlot::IsPlotHovered())
                    {
                        ImPlotPoint mp = ImPlot::GetPlotMousePos();
                        int   best_k   = -1;
                        double best_dx = 1e9;
                        const double half_snap  = 0.5 * bar_w * cfg.bar_tooltip_snap_factor;

                        // найти ближайший видимый бар по X среди отрисованных
                        for (size_t k = 0, i = 0; k < state.dnd.size(); ++k) {
                            const auto& it = state.dnd[k];
                            if (!it.is_plot)
                                continue;

                            const double pos = static_cast<double>(i);
                            // «серия скрыта легендой»? — пропускаем (должно вызываться после PlotBars)
                            if (!IsSeriesVisible(it.label.data())) {
                                ++i; 
                                continue;
                            }

                            const double dx = std::abs(mp.x - pos);
                            if (dx <= half_snap && dx < best_dx) {
                                best_dx = dx;
                                best_k  = (int)k;
                            }
                            ++i;
                        }

                        if (best_k >= 0) {
                            const auto& it = state.dnd[best_k];
                            // формат значения по cfg.value_fmt
                            char vbuf[64];
                            ImFormatString(vbuf, IM_ARRAYSIZE(vbuf), cfg.value_fmt, data.values[best_k]);

                            if (ImGui::BeginTooltip()) {
                                ImPlot::ItemIcon(it.color);
                                ImGui::SameLine();
                                ImGui::TextUnformatted(it.label.data());
                                ImGui::Separator();
                                ImGui::Text("%s: %s", cfg.y_label ? cfg.y_label : "Value", vbuf);
                                ImGui::EndTooltip();
                            }
                        }
                    }
                } // if (data.values.empty()) else 

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
                
                // контекст-попап по ПКМ
                if (ImPlot::IsPlotHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
                    ImGui::OpenPopup("##custom_menu");
                } // 

                if (ImGui::BeginPopup("##custom_menu")) {
                    ImGui::TextUnformatted(cfg.settings_header);
                    ImGui::Separator();

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
                        for (auto &it : state.dnd) it.is_plot = true;
                        state.update_counter = kUpdateCounterMax;
                    }

                    if (ImGui::Button(cfg.button_reset, ImVec2(btn_w, 0))) {
                        for (auto &it : state.dnd) it.is_plot = false;
                    }

                    ImGui::EndPopup();
                } // if ImGui::BeginPopup

                ImPlot::EndPlot();
            }
            ImPlot::PopStyleVar(); // FitPadding
        }
        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::PopID();
    }

} // namespace ImGuiX::Widgets
