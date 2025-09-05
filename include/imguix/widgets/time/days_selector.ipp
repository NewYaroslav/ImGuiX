#include <imgui.h>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <imguix/extensions/sizing.hpp> // ImGuiX::Extensions::CalcWeekdayComboWidth(), etc.
#include <imguix/widgets/controls/icon_combo.hpp>

namespace ImGuiX::Widgets {

    inline bool DaysOfWeekSelector(
            const char* id,
            std::vector<int>& selected_days,
            const DaysSelectorConfig& cfg) {
        // Normalize input (unique, sorted, clamp to [0,6])
        {
            selected_days.erase(
                std::remove_if(selected_days.begin(), selected_days.end(),
                               [](int d){ return d < 0 || d > 6; }),
                selected_days.end());
            std::sort(selected_days.begin(), selected_days.end());
            selected_days.erase(std::unique(selected_days.begin(), selected_days.end()), selected_days.end());
        }

        // Build precise preview by pixel width (UTF-8 safe by construction of names)
        auto build_preview = [&](float max_w)->std::string {
            if (selected_days.empty())
                return std::string(cfg.empty_hint ? cfg.empty_hint : u8"none");

            std::string acc;
            acc.reserve(32);

            // Reserve a bit for "..." (so we can always append it)
            const float ell_w = ImGui::CalcTextSize("...").x;
            const float limit = std::max(0.0f, max_w - ell_w);

            auto fits = [&](const std::string& s)->bool {
                return ImGui::CalcTextSize(s.c_str()).x <= limit;
            };

            for (size_t i = 0; i < selected_days.size(); ++i) {
                const int d = selected_days[i];
                const char* token = (d >= 0 && d < 7) ? cfg.short_names[d] : "?";
                std::string candidate = acc;
                if (!candidate.empty()) candidate.push_back(',');
                candidate += token;

                if (fits(candidate)) {
                    acc.swap(candidate);
                } else {
                    // если уже что-то есть — ставим троеточие
                    if (!acc.empty()) acc += "...";
                    else {
                        // иначе покажем хотя бы первый и троеточие
                        acc = token;
                        acc += "...";
                    }
                    break;
                }
            }
            return acc.empty()
                ? std::string(cfg.empty_hint ? cfg.empty_hint : u8"none")
                : acc;
        };

        const float combo_width = cfg.combo_width > 0.0f ?
            cfg.combo_width :
            ImGuiX::Extensions::CalcWeekdayComboWidth();

        ImVec2 cell = cfg.cell_size;
        if (cell.x <= 0.0f || cell.y <= 0.0f) {
            const float h = ImGui::GetFrameHeight();
            cell = ImVec2(h, h);
        }

        const ImGuiStyle& style = ImGui::GetStyle();

        ImVec2 popup_size = cfg.popup_size;
        if (popup_size.x <= 0.0f) {
            popup_size.x = ((cell.x + style.ItemInnerSpacing.x) * (1.0f + cfg.cols)) + 2.0f * style.FramePadding.x;
        }
        if (popup_size.y <= 0.0f) {
            popup_size.y = ImGui::GetFrameHeightWithSpacing() + cell.y * cfg.rows + 2.0f * style.FramePadding.y;
        }

        std::string preview = build_preview(ImGuiX::Extensions::CalcComboPreviewTextMax(combo_width));

        // Local 7-bit mask
        bool mark[7] = {};
        for (int d : selected_days) if (0 <= d && d < 7) mark[d] = true;

        bool changed = false;

        ImGui::PushID(id);
        ImGui::SetNextItemWidth(combo_width);
        ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, 300.0f));
        bool open = cfg.use_icon_combo ?
            BeginIconCombo(
                cfg.label ? cfg.label : u8"Days",
                preview.c_str(),
                cfg.icon_text,
                ImGuiComboFlags_HeightLargest | ImGuiComboFlags_PopupAlignLeft) :
            ImGui::BeginCombo(
                cfg.label ? cfg.label : u8"Days",
                preview.c_str(),
                ImGuiComboFlags_HeightLargest | ImGuiComboFlags_PopupAlignLeft);
        if (open) {
            const float cur_x = ImGui::GetCursorPosX();
            // доступная ширина внутри попапа (после Indent)
            const float avail_w = ImGui::GetContentRegionAvail().x;

            // Сетка
            const int rows = std::max(1, cfg.rows);
            const int cols = std::max(1, cfg.cols);
            const ImVec2 cell = cfg.cell_size;
            const int total = std::max(rows * cols, 7); 

            ImDrawList* dl = ImGui::GetWindowDrawList();
            const float rounding = (cfg.cell_rounding < 0.0f)
                ? style.FrameRounding
                : cfg.cell_rounding;
            
            // ширина сетки (ячейки + межколоночные отступы)
            const float grid_w =
                cols * cfg.cell_size.x + (cols - 1) * style.ItemSpacing.x;
                
            // ширина тулбара (SmallButton по тексту + паддинги)
            auto small_btn_w = [&](const char* txt) {
                return ImGui::CalcTextSize(txt, nullptr, true).x + style.FramePadding.x * 2.0f;
            };

            const float toolbar_w =
                small_btn_w("All")      + style.ItemSpacing.x +
                small_btn_w("None")     + style.ItemSpacing.x +
                small_btn_w("Workdays") + style.ItemSpacing.x +
                small_btn_w("Weekend");

            const float toolbar_start_x = cur_x + ImMax(0.0f, (avail_w - toolbar_w) * 0.5f);

            // --- toolbar ---
            ImGui::SetCursorPosX(toolbar_start_x);
            
            if (cfg.show_toolbar_all_none) {
                if (ImGui::SmallButton(cfg.label_all))  { for (int i=0;i<7;++i) mark[i]=true;  changed = true; }
                ImGui::SameLine();
                if (ImGui::SmallButton(cfg.label_none)) { for (int i=0;i<7;++i) mark[i]=false; changed = true; }
            }
            if (cfg.show_toolbar_work_vs_week) {
                if (cfg.show_toolbar_all_none) ImGui::SameLine();
                if (ImGui::SmallButton(cfg.label_workdays)) {
                    for (int i=0;i<7;++i) mark[i] = (i <= 4); // Mon..Fri
                    changed = true;
                }
                ImGui::SameLine();
                if (ImGui::SmallButton(cfg.label_weekend)) {
                    for (int i=0;i<7;++i) mark[i] = (i >= 5); // Sat..Sun
                    changed = true;
                }
            }

            // половина свободного пространства
            const float pad_x = ImMax(0.0f, (avail_w - grid_w) * 0.5f);
            // запомним целевую X-позицию начала строки
            const float row_start_x = ImGui::GetCursorPosX() + pad_x;

            for (int r = 0; r < rows; ++r) {
                ImGui::SetCursorPosX(row_start_x);
                for (int c = 0; c < cols; ++c) {
                    const int idx = r * cols + c;
                    if (c) ImGui::SameLine();
                    if (idx >= 7) { ImGui::Dummy(cell); continue; }

                    ImGui::PushID(idx);
                    const char* txt = (idx>=0 && idx<7) ? cfg.short_names[idx] : "?";

                    bool selected = mark[idx];
                    if (selected && cfg.use_header_color_for_selected) {
                        const ImVec4 sel = ImGui::GetStyleColorVec4(ImGuiCol_TextSelectedBg);
                        ImGui::PushStyleColor(ImGuiCol_Header, sel);
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, sel);
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, sel);
                    }

                    if (ImGui::Selectable(txt, selected, ImGuiSelectableFlags_DontClosePopups, cell)) {
                        mark[idx] = !selected;
                        changed = true;
                    }

                    if (cfg.show_cell_borders) {
                        ImVec2 p0 = ImGui::GetItemRectMin();
                        ImVec2 p1 = ImGui::GetItemRectMax();
                        p0.x = std::floor(p0.x) + 0.5f;
                        p0.y = std::floor(p0.y) + 0.5f;
                        p1.x = std::floor(p1.x) - 0.5f;
                        p1.y = std::floor(p1.y) - 0.5f;

                        ImU32 col =
                            selected ? (cfg.cell_border_color_selected ? cfg.cell_border_color_selected
                                                                       : ImGui::GetColorU32(ImGuiCol_Header)) :
                            (ImGui::IsItemHovered() ? (cfg.cell_border_color_hovered ? cfg.cell_border_color_hovered
                                                                                       : ImGui::GetColorU32(ImGuiCol_HeaderHovered))
                                                    : (cfg.cell_border_color ? cfg.cell_border_color
                                                                             : ImGui::GetColorU32(ImGuiCol_Border)));

                        dl->AddRect(p0, p1, col, rounding, 0, cfg.cell_border_thickness);
                    }

                    if (selected && cfg.use_header_color_for_selected) {
                        ImGui::PopStyleColor(3);
                    }

                    ImGui::PopID();
                }
            }

            ImGui::SameLine(0, 0);
            ImGui::Dummy(ImVec2(style.FramePadding.x, 0));

            ImGui::EndCombo();

            if (changed) {
                selected_days.clear();
                for (int i = 0; i < 7; ++i)
                    if (mark[i]) selected_days.push_back(i);
            }
        }
        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

