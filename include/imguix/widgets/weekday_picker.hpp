#pragma once
#ifndef _IMGUIX_WIDGETS_WEEKDAY_PICKER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_WEEKDAY_PICKER_HPP_INCLUDED

/// \file weekday_picker.hpp
/// \brief Multi-selection widget for choosing weekdays.
/// \note Standalone header with minimal time_shield enums.

#include <imgui.h>
#include <string>
#include <vector>

namespace time_shield {

    enum FormatType {
        UPPERCASE_NAME = 0,
        SHORT_NAME,
        FULL_NAME,
    };

    enum Weekday {
        SUN = 0,
        MON,
        TUE,
        WED,
        THU,
        FRI,
        SAT,
    };

    inline const char* to_cstr(Weekday value, FormatType format = SHORT_NAME) {
        static const char* const uppercase_names[] = {
            "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
        static const char* const short_names[] = {
            "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        static const char* const full_names[] = {
            "Sunday", "Monday", "Tuesday", "Wednesday",
            "Thursday", "Friday", "Saturday"};
        switch (format) {
        default:
        case UPPERCASE_NAME:
            return uppercase_names[static_cast<size_t>(value)];
        case SHORT_NAME:
            return short_names[static_cast<size_t>(value)];
        case FULL_NAME:
            return full_names[static_cast<size_t>(value)];
        }
    }

    constexpr int kDaysPerWeek = 7;

} // namespace time_shield

namespace ImGuiX::Widgets {

    struct WeekdayPickerConfig {
        const char* label       = u8"Weekdays";
        const char* empty_desc  = u8"None";
        float       combo_width = 160.0f;
        ImVec2      list_size   = ImVec2(0.0f, 0.0f);
        float       item_size   = 32.0f;
    };

    inline bool WeekdayPicker(
            const char* id,
            std::vector<int>& items,
            const WeekdayPickerConfig& cfg = {}) {
        std::string current_text;
        const float avg_char = ImGui::CalcTextSize("Hamburgefontsiv").x / 15.0f;
        const size_t max_len =
                static_cast<size_t>((cfg.combo_width - 32.0f) / avg_char);
        if (items.empty()) {
            current_text = cfg.empty_desc ? cfg.empty_desc : "";
        } else {
            for (size_t i = 0; i < items.size(); ++i) {
                if (i) current_text += ",";
                if (items[i] >= 0 && items[i] < time_shield::kDaysPerWeek) {
                    current_text +=
                            time_shield::to_cstr(
                                    static_cast<time_shield::Weekday>(items[i]),
                                    time_shield::SHORT_NAME);
                }
                if (current_text.size() >= max_len) break;
            }
        }
        if (current_text.size() >= max_len && max_len > 3) {
            current_text.resize(max_len - 3);
            current_text += "...";
        }

        ImGui::SetNextItemWidth(cfg.combo_width);
        if (!ImGui::BeginCombo(cfg.label ? cfg.label : id, current_text.c_str()))
            return false;

        bool changed = false;
        ImGui::BeginChild("##weekday_list", cfg.list_size, true);

        ImGuiContext& g = *GImGui;
        ImGuiStyle& style = g.Style;
        const ImVec4& selected_color = style.Colors[ImGuiCol_TextSelectedBg];
        const ImVec4& base_text = style.Colors[ImGuiCol_Text];

        int8_t selected[time_shield::kDaysPerWeek] = {0};
        for (int v : items) {
            if (v >= 0 && v < time_shield::kDaysPerWeek) selected[v] = 1;
        }

        for (int day = 0; day < time_shield::kDaysPerWeek; ++day) {
            if (day) ImGui::SameLine();
            ImGui::PushID(day);
            if (day == time_shield::SUN || day == time_shield::SAT) {
                ImGui::PushStyleColor(
                        ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, base_text.w));
            }
            const bool is_sel = selected[day] != 0;
            if (is_sel) {
                ImGui::PushStyleColor(ImGuiCol_Header, selected_color);
            }
            if (ImGui::Selectable(
                        time_shield::to_cstr(
                                static_cast<time_shield::Weekday>(day),
                                time_shield::SHORT_NAME),
                        is_sel,
                        0,
                        ImVec2(cfg.item_size, cfg.item_size))) {
                selected[day] ^= 1;
                changed = true;
            }
            if (is_sel) {
                ImGui::PopStyleColor();
            }
            if (day == time_shield::SUN || day == time_shield::SAT) {
                ImGui::PopStyleColor();
            }
            ImGui::PopID();
        }

        if (changed) {
            items.clear();
            for (int i = 0; i < time_shield::kDaysPerWeek; ++i) {
                if (selected[i]) items.push_back(i);
            }
        }

        ImGui::EndChild();
        ImGui::EndCombo();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_WEEKDAY_PICKER_HPP_INCLUDED
