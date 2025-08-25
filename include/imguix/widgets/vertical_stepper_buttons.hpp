#pragma once
#ifndef _IMGUIX_WIDGETS_VERTICAL_STEPPER_BUTTONS_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VERTICAL_STEPPER_BUTTONS_HPP_INCLUDED

/// \file vertical_stepper_buttons.hpp
/// \brief Stacked [+]/[−] button column (manual drawing) for use beside inputs.
///        Height defaults to ImGui::GetFrameHeight() when size.y <= 0.

#include <imgui.h>
#include <algorithm>
#include <cmath>

namespace ImGuiX::Widgets {

    /// \param id        Unique ID (scoped by caller).
    /// \param size      Column size (width, height). If size.{x|y} <= 0 -> uses ImGui::GetFrameHeight().
    /// \param step      Increment per click/hold.
    /// \param step_fast Increment when Ctrl held.
    /// \param gap_y     Vertical gap between [+] and [−]; default 1.0f. Pass <0 to use style.ItemInnerSpacing.y.
    /// \return Signed delta this frame (>0 '+', <0 '−').
    inline int VerticalStepperButtons(
            const char* id, 
            ImVec2 size, 
            int step, int 
            step_fast, 
            float gap_y = 1.0f
        ) {
        ImGui::PushID(id);

        ImGuiStyle& style = ImGui::GetStyle();
        ImDrawList* dl    = ImGui::GetWindowDrawList();

        const float frame_h = ImGui::GetFrameHeight();
        if (size.x <= 0.0f) size.x = frame_h;
        if (size.y <= 0.0f) size.y = frame_h;

        const float gap       = (gap_y >= 0.0f) ? gap_y : style.ItemInnerSpacing.y;
        const float top_h     = (size.y - gap) * 0.5f;
        const float bottom_h  = top_h;
        const float rounding  = style.FrameRounding;
        const ImU32 col_text  = ImGui::GetColorU32(ImGuiCol_Text);

        auto draw_bg = [&](const ImVec2& p, const ImVec2& s, bool hovered, bool active) {
            const ImU32 bg = ImGui::GetColorU32(active ? ImGuiCol_ButtonActive
                                                       : (hovered ? ImGuiCol_ButtonHovered
                                                                  : ImGuiCol_FrameBg));
            dl->AddRectFilled(p, ImVec2(p.x + s.x, p.y + s.y), bg, rounding);
        };

        // helper: draw crisp '+' using 1px filled rects, aligned to integer pixels
        auto draw_plus = [&](ImVec2 pos, ImVec2 sz) {
            const float cx = std::floor(pos.x + sz.x * 0.5f);
            const float cy = std::floor(pos.y + sz.y * 0.5f);
            const float e  = std::floor(std::min(sz.x, sz.y) * 0.35f);

            // horizontal: [cx-e, cy]..[cx+e, cy+1]
            dl->AddRectFilled(ImVec2(cx - e, cy), ImVec2(cx + e + 1.0f, cy + 1.0f), col_text, 0.0f);
            // vertical:   [cx, cy-e]..[cx+1, cy+e]
            dl->AddRectFilled(ImVec2(cx, cy - e), ImVec2(cx + 1.0f, cy + e + 1.0f), col_text, 0.0f);
        };

        // helper: draw crisp '−'
        auto draw_minus = [&](ImVec2 pos, ImVec2 sz) {
            const float cx = std::floor(pos.x + sz.x * 0.5f);
            const float cy = std::floor(pos.y + sz.y * 0.5f);
            const float e  = std::floor(std::min(sz.x, sz.y) * 0.35f);
            dl->AddRectFilled(ImVec2(cx - e, cy), ImVec2(cx + e + 1.0f, cy + 1.0f), col_text, 0.0f);
        };

        ImGui::PushButtonRepeat(true);
        int delta = 0;
        const int step_used = ImGui::GetIO().KeyCtrl ? step_fast : step;

        ImGui::BeginGroup();

        // --- Top (+)
        {
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 sz(size.x, top_h);
            ImGui::InvisibleButton("##up", sz);
            const bool hov = ImGui::IsItemHovered();
            const bool act = ImGui::IsItemActive();
            draw_bg(pos, sz, hov, act);
            draw_plus(pos, sz);

            if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
                delta += step_used;
        }

        // gap
        ImVec2 cur = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos(ImVec2(
            cur.x,
            cur.y - ImGui::GetStyle().ItemSpacing.y - top_h + frame_h - bottom_h
        ));

        // --- Bottom (−)
        {
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 sz(size.x, bottom_h);
            ImGui::InvisibleButton("##down", sz);
            const bool hov = ImGui::IsItemHovered();
            const bool act = ImGui::IsItemActive();
            draw_bg(pos, sz, hov, act);
            draw_minus(pos, sz);

            if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
                delta -= step_used;
        }

        ImGui::EndGroup();
        ImGui::PopButtonRepeat();

        ImGui::PopID();
        return delta;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_VERTICAL_STEPPER_BUTTONS_HPP_INCLUDED
