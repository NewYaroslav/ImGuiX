#pragma once
#ifndef _IMGUIX_WIDGETS_HOURS_SELECTOR_HPP_INCLUDED
#define _IMGUIX_WIDGETS_HOURS_SELECTOR_HPP_INCLUDED

/// \file hours_selector.hpp
/// \brief Combo widget to pick hours (0..23) via a grid (default 3x8) with a compact summary preview.

#include <imgui.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>

namespace ImGuiX::Widgets {

    /// \brief Configuration for HoursSelector.
    struct HoursSelectorConfig {
        const char* label        = "Hours";     ///< Combo label (left of preview)
        const char* empty_hint   = "none";      ///< Preview text when no hours selected
        ImVec2      child_size   = ImVec2(260, 110); ///< Scrolling area size inside combo
        ImVec2      cell_size    = ImVec2(18, 18);   ///< Clickable cell size for each hour
        int         rows         = 3;           ///< Grid rows (rows*cols should cover 24)
        int         cols         = 8;           ///< Grid cols
        float       combo_width  = 260.0f;      ///< SetNextItemWidth for the combo
        bool        use_header_color_for_selected = true; ///< Use ImGuiCol_Header for selected bg
    };

    /// \brief Renders hours picker (0..23) as a combo with a grid inside.
    /// \param id Unique ID.
    /// \param selected_hours In/out: list of selected hours (0..23). Order is normalized to ascending on exit.
    /// \param cfg Config.
    /// \return true if selection changed this frame.
    inline bool HoursSelector(const char* id, std::vector<int>& selected_hours, const HoursSelectorConfig& cfg = {}) {
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
            if (selected_hours.empty()) return std::string(cfg.empty_hint ? cfg.empty_hint : "none");
            std::string acc;
            acc.reserve(64);
            for (size_t i = 0; i < selected_hours.size(); ++i) {
                char tmp[8];
                std::snprintf(tmp, sizeof(tmp), "%s%d", (i ? "," : ""), selected_hours[i]);
                std::string candidate = acc;
                candidate += tmp;
                float w = ImGui::CalcTextSize(candidate.c_str()).x;
                if (w <= max_w) {
                    acc.swap(candidate);
                } else {
                    // Add ellipsis if anything was added before; otherwise show first with ellipsis
                    if (!acc.empty()) acc += "...";
                    else {
                        // Show first number and ellipsis anyway
                        std::snprintf(tmp, sizeof(tmp), "%d...", selected_hours[i]);
                        acc = tmp;
                    }
                    break;
                }
            }
            return acc;
        };

        const float preview_max_w = std::max(0.0f, cfg.combo_width - 32.0f); // leave a little slack
        std::string preview = build_preview(preview_max_w);

        // Local 24-bit mask
        bool mark[24] = {};
        for (int h : selected_hours) if (0 <= h && h < 24) mark[h] = true;

        bool changed = false;

        ImGui::PushID(id);
        ImGui::SetNextItemWidth(cfg.combo_width);
        if (ImGui::BeginCombo(cfg.label ? cfg.label : "Hours", preview.c_str())) {
            ImGui::BeginChild("##hours_grid", cfg.child_size, true);

            // Grid layout
            const int rows = std::max(1, cfg.rows);
            const int cols = std::max(1, cfg.cols);
            const ImVec2 cell = cfg.cell_size;

            // Safety: ensure we can cover all 24 hours even if rows*cols < 24
            const int total = std::max(rows * cols, 24);

            // Small toolbar (optional): Select All / Clear
            if (ImGui::SmallButton("All")) {
                for (int i = 0; i < 24; ++i) mark[i] = true;
                changed = true;
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("None")) {
                for (int i = 0; i < 24; ++i) mark[i] = false;
                changed = true;
            }

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
                    std::snprintf(label_txt, sizeof(label_txt), "%d", idx);

                    bool selected = mark[idx];
                    if (selected && cfg.use_header_color_for_selected) {
                        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_TextSelectedBg));
                    }

                    // Use Selectable for highlight/hover; fixed size via Size parameter
                    if (ImGui::Selectable(label_txt, selected, 0, cell)) {
                        mark[idx] = !selected;
                        changed = true;
                    }

                    if (selected && cfg.use_header_color_for_selected) {
                        ImGui::PopStyleColor();
                    }
                    ImGui::PopID();
                }
            }

            ImGui::EndChild();
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

#endif // _IMGUIX_WIDGETS_HOURS_SELECTOR_HPP_INCLUDED

