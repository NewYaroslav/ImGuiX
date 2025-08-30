#include <algorithm>
#include <cmath>

namespace ImGuiX::Widgets {

    bool SystemButton(const char* id, SystemButtonType type, ImVec2 size) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton(id, size);
        ImDrawList* dl = ImGui::GetWindowDrawList();

        // Hover/active background
        if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
            ImVec4 bg = ImGui::GetStyleColorVec4(ImGui::IsItemActive() ? ImGuiCol_ButtonActive
                                                                       : ImGuiCol_ButtonHovered);
            dl->AddRectFilled(pos,
                              ImVec2(pos.x + size.x, pos.y + size.y),
                              ImGui::ColorConvertFloat4ToU32(bg),
                              IMGUIX_SYSBTN_BG_ROUNDING);
        }

        const ImU32 col = ImGui::GetColorU32(ImGuiCol_Text);

        // Inner rect (from button edges)
        const float m   = IMGUIX_SYSBTN_MARGIN;
        const float inL = pos.x + m, inT = pos.y + m;
        const float inR = pos.x + size.x - m, inB = pos.y + size.y - m;
        const float inW = std::max(0.0f, inR - inL);
        const float inH = std::max(0.0f, inB - inT);
        if (inW <= 0.0f || inH <= 0.0f)
            return ImGui::IsItemClicked(ImGuiMouseButton_Left);

        const float base = std::min(inW, inH);

        // Quantize padding/side once (integer px)
        const float pad_px  = std::round(IMGUIX_SYSBTN_CROSS_EXTENT * base);
        const float side_px = std::max(1.0f, std::round(base - 2.0f * pad_px));

        // Center & icon square edges (symmetric)
        const float cx = (inL + inR) * 0.5f;
        const float cy = (inT + inB) * 0.5f;
        const float L  = std::round(cx - 0.5f * side_px);
        const float T  = std::round(cy - 0.5f * side_px);
        const float R  = L + side_px;
        const float B  = T + side_px;

        // Stroke metrics
        const float th  = std::max(1.0f, std::round(IMGUIX_SYSBTN_LINE_THICKNESS));
        const float hp  = 0.5f * th;
        const float d45 = hp * 0.70710678f; // hp / sqrt(2)

        // helper: draw a thin diagonal as a filled quad WITHOUT AA fill (to avoid fat look)
        const auto draw_diag_noAA = [&](ImVec2 a, ImVec2 b) {
            ImVec2 v(b.x - a.x, b.y - a.y);
            float len = std::sqrt(v.x * v.x + v.y * v.y);
            if (len <= 0.0f) return;
            v.x /= len; v.y /= len;                  // unit dir
            ImVec2 n(-v.y, v.x);                     // unit normal
            ImVec2 off(n.x * hp, n.y * hp);          // half-width offset

            ImDrawListFlags saved = dl->Flags;
            dl->Flags &= ~ImDrawListFlags_AntiAliasedFill; // crisp 1px body

            dl->AddQuadFilled(ImVec2(a.x - off.x, a.y - off.y),
                              ImVec2(a.x + off.x, a.y + off.y),
                              ImVec2(b.x + off.x, b.y + off.y),
                              ImVec2(b.x - off.x, b.y - off.y), col);

            dl->Flags = saved; // restore AA settings
        };

        switch (type) {
            case SystemButtonType::Close: {
                // Endpoints pulled inside so outer edge aligns with L/T/R/B
                draw_diag_noAA(ImVec2(L + d45, T + d45), ImVec2(R - d45, B - d45));
                draw_diag_noAA(ImVec2(L + d45, B - d45), ImVec2(R - d45, T + d45));
                break;
            }
            case SystemButtonType::Minimize: {
                // Solid bar with integer edges
                const float y1 = std::round(B - th);
                dl->AddRectFilled(ImVec2(L, y1), ImVec2(R, y1 + th), col);
                break;
            }
            case SystemButtonType::Maximize: {
                // 4 solid edges (integer coords)
                dl->AddRectFilled(ImVec2(L, T),       ImVec2(R, T + th), col); // top
                dl->AddRectFilled(ImVec2(L, B - th),  ImVec2(R, B),     col); // bottom
                dl->AddRectFilled(ImVec2(L, T),       ImVec2(L + th, B), col); // left
                dl->AddRectFilled(ImVec2(R - th, T),  ImVec2(R, B),     col); // right
                break;
            }
        }

        return ImGui::IsItemClicked(ImGuiMouseButton_Left);
    }

} // namespace ImGuiX::Widgets

