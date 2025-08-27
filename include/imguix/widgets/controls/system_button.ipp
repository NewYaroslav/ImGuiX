#include <algorithm>
#include <cmath>

namespace ImGuiX::Widgets {

    bool SystemButton(const char* id, SystemButtonType type, ImVec2 size) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton(id, size);
        ImDrawList* dl = ImGui::GetWindowDrawList();

        if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
            ImVec4 bg = ImGui::GetStyleColorVec4(
                    ImGui::IsItemActive() ?
                        ImGuiCol_ButtonActive :
                        ImGuiCol_ButtonHovered
            );
            dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                              ImGui::ColorConvertFloat4ToU32(bg), 2.0f);
        }

        ImU32 col = ImGui::GetColorU32(ImGuiCol_Text);
        const float thickness = IMGUIX_SYSBTN_LINE_THICKNESS;

        auto snap = [](float v) -> float { return std::floor(v) + 0.5f; };

        ImVec2 c(snap(pos.x + size.x * 0.5f), snap(pos.y + size.y * 0.5f));

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

