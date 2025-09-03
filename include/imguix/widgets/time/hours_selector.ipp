#include <imgui.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <imguix/extensions/sizing.hpp> // ImGuiX::Extensions::CalcTimeComboWidth(), etc.
#include <imguix/widgets/controls/icon_combo.hpp>

namespace ImGuiX::Widgets {

    inline bool HoursSelector(const char* id, std::vector<int>& selected_hours, const HoursSelectorConfig& cfg) {
        // Normalize input (unique, sorted, clamped to [0,23])
        {
            selected_hours.erase(
                std::remove_if(selected_hours.begin(), selected_hours.end(),
                               [](int h){ return h < 0 || h > 23; }),
                selected_hours.end());
            std::sort(selected_hours.begin(), selected_hours.end());
            selected_hours.erase(std::unique(selected_hours.begin(), selected_hours.end()), selected_hours.end());
        }

        // Build compact preview string that fits into cfg.combo_width
        auto build_preview = [&](float max_w)->std::string {
            if (selected_hours.empty()) return std::string(cfg.empty_hint ? cfg.empty_hint : u8"none");
            std::string acc;
            acc.reserve(64);
            for (size_t i = 0; i < selected_hours.size(); ++i) {
                char tmp[8];
                std::snprintf(tmp, sizeof(tmp), u8"%s%d", (i ? u8"," : u8""), selected_hours[i]);
                std::string candidate = acc;
                candidate += tmp;
                float w = ImGui::CalcTextSize(candidate.c_str()).x;
                if (w <= max_w) {
                    acc.swap(candidate);
                } else {
                    // Add ellipsis if anything was added before; otherwise show first with ellipsis
                    if (!acc.empty()) acc += u8"...";
                    else {
                        // Show first number and ellipsis anyway
                        std::snprintf(tmp, sizeof(tmp), u8"%d...", selected_hours[i]);
                        acc = tmp;
                    }
                    break;
                }
            }
            return acc;
        };

        const float combo_width = cfg.combo_width > 0.0f ?
            cfg.combo_width :
            ImGuiX::Extensions::CalcWeekdayComboWidth();

        std::string preview = build_preview(
            ImGuiX::Extensions::CalcComboPreviewTextMax(combo_width)
        );

        // Local 24-bit mask
        bool mark[24] = {};
        for (int h : selected_hours) if (0 <= h && h < 24) mark[h] = true;

        ImVec2 cell = cfg.cell_size;
        if (cell.x <= 0.0f || cell.y <= 0.0f) {
            const float h = ImGui::GetFrameHeight();
            cell = ImVec2(h, h);
        }

        bool changed = false;

        ImGui::PushID(id);
        ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, 300.0f));
        ImGui::SetNextItemWidth(combo_width);
        bool open = cfg.use_icon_combo ?
            BeginIconCombo(
                cfg.label ? cfg.label : u8"Hours",
                preview.c_str(),
                cfg.icon_text,
                ImGuiComboFlags_HeightLargest | ImGuiComboFlags_PopupAlignLeft) :
            ImGui::BeginCombo(
                cfg.label ? cfg.label : u8"Hours",
                preview.c_str(),
                ImGuiComboFlags_HeightLargest | ImGuiComboFlags_PopupAlignLeft);
        if (open) {
            const ImGuiStyle& st = ImGui::GetStyle();
            ImGui::Indent(st.FramePadding.x);

            // Grid layout
            const int rows = std::max(1, cfg.rows);
            const int cols = std::max(1, cfg.cols);

            // Safety: ensure we can cover all 24 hours even if rows*cols < 24
            const int total = std::max(rows * cols, 24);

            // Small toolbar (optional): Select All / Clear
            if (ImGui::SmallButton(cfg.label_all)) {
                for (int i = 0; i < 24; ++i) mark[i] = true;
                changed = true;
            }
            ImGui::SameLine();
            if (ImGui::SmallButton(cfg.label_none)) {
                for (int i = 0; i < 24; ++i) mark[i] = false;
                changed = true;
            }

            ImDrawList* dl = ImGui::GetWindowDrawList();
            float cell_rounding = (cfg.cell_rounding < 0.0f)
                ? ImGui::GetStyle().FrameRounding
                : cfg.cell_rounding;

            // Grid of Selectable cells
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    const int idx = r * cols + c;
                    if (c) ImGui::SameLine();
                    if (idx >= 24) {
                        // Past 24, draw empty placeholder to preserve grid alignment
                        ImGui::Dummy(cell);
                        continue;
                    }
                    ImGui::PushID(idx);
                    char label_txt[4];
                    std::snprintf(label_txt, sizeof(label_txt), u8"%d", idx);

                    bool selected = mark[idx];
                    if (selected && cfg.use_header_color_for_selected) {
                        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_TextSelectedBg));
                    }

                    // Use Selectable for highlight/hover; fixed size via Size parameter
                    if (ImGui::Selectable(label_txt, selected, ImGuiSelectableFlags_DontClosePopups, cell)) {
                        mark[idx] = !selected;
                        changed = true;
                    }

                    if (selected && cfg.use_header_color_for_selected) {
                        ImGui::PopStyleColor();
                    }

                    if (cfg.show_cell_borders) {
                        ImVec2 p0 = ImGui::GetItemRectMin();
                        ImVec2 p1 = ImGui::GetItemRectMax();

                        // для чёткой 1px линии на целочисленных координатах
                        p0.x = std::floor(p0.x) + 0.5f;
                        p0.y = std::floor(p0.y) + 0.5f;
                        p1.x = std::floor(p1.x) - 0.5f;
                        p1.y = std::floor(p1.y) - 0.5f;

                        // выбираем цвет: selected/hovered/normal
                        ImU32 col =
                            selected ? (cfg.cell_border_color_selected ? cfg.cell_border_color_selected
                                                                       : ImGui::GetColorU32(ImGuiCol_Header)) :
                            (ImGui::IsItemHovered() ? (cfg.cell_border_color_hovered ? cfg.cell_border_color_hovered
                                                                                      : ImGui::GetColorU32(ImGuiCol_HeaderHovered))
                                                    : (cfg.cell_border_color ? cfg.cell_border_color
                                                                             : ImGui::GetColorU32(ImGuiCol_Border)));

                        dl->AddRect(p0, p1, col, cell_rounding, 0, cfg.cell_border_thickness);
                    }
                    ImGui::PopID();
                }
            }

            ImGui::SameLine(0, 0);
            ImGui::Dummy(ImVec2(st.FramePadding.x, 0));

            ImGui::Unindent(st.FramePadding.x);
            ImGui::EndCombo();

            if (changed) {
                selected_hours.clear();
                selected_hours.reserve(24);
                for (int i = 0; i < 24; ++i)
                    if (mark[i]) selected_hours.push_back(i);
            }
        }

        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

