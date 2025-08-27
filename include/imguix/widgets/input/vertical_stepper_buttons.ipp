#include <algorithm>
#include <cmath>

#include <imgui.h>

namespace ImGuiX::Widgets {

int VerticalStepperButtons(
        const char* id,
        ImVec2 size,
        int step,
        int step_fast,
        float gap_y
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

    int delta = 0;
    const int step_used = ImGui::GetIO().KeyCtrl ? step_fast : step;

    ImGui::BeginGroup();
    ImVec2 cur = ImGui::GetCursorScreenPos();
    ImGui::PushButtonRepeat(true);
    // --- Top (+)
    {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 sz(size.x, top_h);
        if (ImGui::InvisibleButton("##up", sz)) {
            delta += step_used;
        }
        const bool hov = ImGui::IsItemHovered();
        const bool act = ImGui::IsItemActive();
        draw_bg(pos, sz, hov, act);
        draw_plus(pos, sz);

        //if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        //    delta += step_used;
    }

    // gap
    ImGui::SetCursorScreenPos(ImVec2(
        cur.x,
        std::floor(cur.y + frame_h - bottom_h + 0.5f)
    ));

    // --- Bottom (−)
    {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 sz(size.x, bottom_h);
        if (ImGui::InvisibleButton("##down", sz)) {
            delta -= step_used;
        }
        const bool hov = ImGui::IsItemHovered();
        const bool act = ImGui::IsItemActive();
        draw_bg(pos, sz, hov, act);
        draw_minus(pos, sz);

        // if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        //     delta -= step_used;
    }
    ImGui::PopButtonRepeat();

    ImGui::EndGroup();
    ImGui::PopID();
    return delta;
}

} // namespace ImGuiX::Widgets

