#pragma once
#ifndef _IMGUIX_WIDGETS_TOGGLE_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_TOGGLE_BUTTON_HPP_INCLUDED

/// \file toggle_button.hpp
/// \brief Provides a simple animated toggle switch for ImGui.

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Draw an animated toggle switch.
    /// \param id Unique string identifier.
    /// \param state Pointer to boolean storing current state.
    /// \note Switch uses style colors when hovered or active.
    inline void ToggleButton(const char* id, bool* state) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        float height = ImGui::GetFrameHeight();
        float width = height * 1.55f;
        float radius = height * 0.50f;

        ImGui::InvisibleButton(id, ImVec2(width, height));
        if (ImGui::IsItemClicked()) {
            *state = !*state;
        }

        float t = *state ? 1.0f : 0.0f;

        ImGuiContext& g = *GImGui;
        const float kAnimSpeed = 0.08f;
        if (g.LastActiveId == g.CurrentWindow->GetID(id)) {
            float t_anim = ImSaturate(g.LastActiveIdTimer / kAnimSpeed);
            t = *state ? t_anim : (1.0f - t_anim);
        }

        ImU32 col_bg;
        if (ImGui::IsItemHovered()) {
            col_bg = ImGui::GetColorU32(
                    ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f),
                           ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
        } else {
            col_bg = ImGui::GetColorU32(
                    ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f),
                           ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));
        }

        draw_list->AddRectFilled(
                pos,
                ImVec2(pos.x + width, pos.y + height),
                col_bg,
                height * 0.5f);
        draw_list->AddCircleFilled(
                ImVec2(pos.x + radius + t * (width - radius * 2.0f),
                       pos.y + radius),
                radius - 1.5f,
                IM_COL32(255, 255, 255, 255));
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_TOGGLE_BUTTON_HPP_INCLUDED
