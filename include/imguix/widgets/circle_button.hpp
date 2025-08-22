#include <imgui.h>
#ifndef _IMGUIX_WIDGETS_CIRCLE_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_CIRCLE_BUTTON_HPP_INCLUDED

/// \file circle_button.hpp
/// \brief Provides a utility for drawing circular ImGui buttons.

namespace ImGuiX::Widgets {

    /// \brief Draws a simple circular button with a solid color fill.
    ///
    /// The button reacts to hover and active states by changing its color according to the current ImGui style.
    ///
    /// \param id Unique string ID for the button.
    /// \param diameter Diameter of the button in pixels.
    /// \param color Base color of the button.
    /// \return True if the button was clicked.
    bool CircleButton(const char* id, float diameter, const ImVec4& color) {
        ImVec2 size = ImVec2(diameter, diameter);
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton(id, size);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 center = ImVec2(pos.x + diameter * 0.5f, pos.y + diameter * 0.5f);

        ImVec4 final_col = color;
        if (ImGui::IsItemHovered()) final_col = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        if (ImGui::IsItemActive()) final_col = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);

        draw_list->AddCircleFilled(center, diameter * 0.5f, ImGui::ColorConvertFloat4ToU32(final_col), 16);

        return ImGui::IsItemClicked();
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_CIRCLE_BUTTON_HPP_INCLUDED
