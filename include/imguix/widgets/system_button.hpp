#pragma once
#ifndef _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED

/// \file system_button.hpp
/// \brief Provides custom system-style buttons (close, minimize, maximize) for ImGui windows.

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Type of the system-style button.
    enum class SystemButtonType {
        Close,
        Minimize,
        Maximize
    };

    /// \brief Draws a system-style button with icon based on type (e.g., close, minimize, maximize).
    ///
    /// The button is visually highlighted on hover/active states using style colors.
    ///
    /// \param id Unique ID string.
    /// \param type Button type (close, minimize, maximize).
    /// \param size Size of the button in pixels.
    /// \return True if the button was clicked.
    inline bool SystemButton(const char* id, SystemButtonType type, ImVec2 size) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton(id, size);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        const bool is_hovered = ImGui::IsItemHovered();
        const bool is_active  = ImGui::IsItemActive();

        if (is_hovered || is_active) {
            ImVec4 bg_col = ImGui::GetStyleColorVec4(is_active ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
            draw_list->AddRectFilled(pos,
                                     ImVec2(pos.x + size.x, pos.y + size.y),
                                     ImGui::ColorConvertFloat4ToU32(bg_col),
                                     2.0f);
        }

        ImVec4 fg_col = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        ImU32 col_u32 = ImGui::ColorConvertFloat4ToU32(fg_col);

        ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);
        float cross_extent = size.x * 0.25f;
        float line_thickness = 1.0f;

        if (type == SystemButtonType::Close) {
            ImVec2 a1 = ImVec2(center.x - cross_extent, center.y - cross_extent);
            ImVec2 b1 = ImVec2(center.x + cross_extent, center.y + cross_extent);
            ImVec2 a2 = ImVec2(center.x - cross_extent, center.y + cross_extent);
            ImVec2 b2 = ImVec2(center.x + cross_extent, center.y - cross_extent);
            draw_list->AddLine(a1, b1, col_u32, line_thickness);
            draw_list->AddLine(a2, b2, col_u32, line_thickness);
        }
        else if (type == SystemButtonType::Minimize) {
            ImVec2 a = ImVec2(center.x - cross_extent, center.y);
            ImVec2 b = ImVec2(center.x + cross_extent, center.y);
            draw_list->AddLine(a, b, col_u32, line_thickness);
        }
        else if (type == SystemButtonType::Maximize) {
            float half = cross_extent * 0.85f;
            ImVec2 tl = ImVec2(center.x - half, center.y - half);
            ImVec2 br = ImVec2(center.x + half, center.y + half);
            draw_list->AddRect(tl, br, col_u32, 0.0f, 0, line_thickness);
        }

        return ImGui::IsItemClicked();
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED