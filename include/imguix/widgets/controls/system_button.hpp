#pragma once
#ifndef _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED

/// \file system_button.hpp
/// \brief Provides custom system-style buttons (close, minimize, maximize) for ImGui windows.

#include <imgui.h>
#include <algorithm>
#include <cmath>

/// \brief Extent multiplier for system button icons (default 0.25f).
#ifndef IMGUIX_SYSBTN_CROSS_EXTENT
#define IMGUIX_SYSBTN_CROSS_EXTENT 0.25f
#endif

/// \brief Margin in pixels from button border (default 1.0f).
#ifndef IMGUIX_SYSBTN_MARGIN
#define IMGUIX_SYSBTN_MARGIN 1.0f
#endif

/// \brief Line thickness in pixels.
#ifndef IMGUIX_SYSBTN_LINE_THICKNESS
#define IMGUIX_SYSBTN_LINE_THICKNESS 1.0f
#endif

namespace ImGuiX::Widgets {

    /// \brief Type of the system-style button.
    enum class SystemButtonType {
        Close,
        Minimize,
        Maximize
    };

    /// \brief Draw system-style button with icon based on type.
    /// \param id Unique ID string.
    /// \param type Button type.
    /// \param size Button size in pixels.
    /// \return True if button was clicked.
    /// \note Button is highlighted on hover or active using style colors.
    inline bool SystemButton(const char* id, SystemButtonType type, ImVec2 size) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton(id, size);
        ImDrawList* dl = ImGui::GetWindowDrawList();

        if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
            ImVec4 bg = ImGui::GetStyleColorVec4(ImGui::IsItemActive() ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
            dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                              ImGui::ColorConvertFloat4ToU32(bg), 2.0f);
        }

        ImU32 col = ImGui::GetColorU32(ImGuiCol_Text);
        const float thickness = IMGUIX_SYSBTN_LINE_THICKNESS;

        auto snap = [](float v) -> float { return std::floor(v) + 0.5f; };

        // Center of button
        ImVec2 c(snap(pos.x + size.x * 0.5f), snap(pos.y + size.y * 0.5f));

        // Extent of icon
        float min_side = std::min(size.x, size.y);
        float e = IMGUIX_SYSBTN_CROSS_EXTENT * min_side;
        e = std::min(e, 0.5f * min_side - IMGUIX_SYSBTN_MARGIN);

        if (type == SystemButtonType::Close) {
            ImVec2 a1(snap(c.x - e), snap(c.y - e));
            ImVec2 b1(snap(c.x + e), snap(c.y + e));
            ImVec2 a2(snap(c.x - e), snap(c.y + e));
            ImVec2 b2(snap(c.x + e), snap(c.y - e));
            dl->AddLine(a1, b1, col, thickness);
            dl->AddLine(a2, b2, col, thickness);
        } else if (type == SystemButtonType::Minimize) {
            //float y = snap(pos.y + size.y - IMGUIX_SYSBTN_MARGIN - 1.0f);
            ImVec2 a(snap(c.x - e), snap(c.y + e));
            ImVec2 b(snap(c.x + e), snap(c.y + e));
            dl->AddLine(a, b, col, thickness);
        } else if (type == SystemButtonType::Maximize) {
            ImVec2 tl(snap(c.x - e), snap(c.y - e));
            ImVec2 br(snap(c.x + e), snap(c.y + e));
            dl->AddRect(tl, br, col, 0.0f, 0, thickness);
        }

        return ImGui::IsItemClicked(ImGuiMouseButton_Left);
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED
