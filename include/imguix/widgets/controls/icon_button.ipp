#include <cmath>

namespace ImGuiX::Widgets {

    bool IconButtonCentered(const char* id, const char* text, const IconButtonConfig& cfg) {
        ImGui::PushID(id);

        ImVec2 sz = cfg.size;
        if (sz.x <= 0 || sz.y <= 0) {
            float h = ImGui::GetFrameHeight();
            sz = ImVec2(h, h); // квадрат высотой с контролы
        }
        
        const ImGuiStyle& st = ImGui::GetStyle();
        const float rounding = (cfg.rounding >= 0.0f) ? cfg.rounding : st.FrameRounding;

        if (cfg.rounding >= 0.0f)
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
        
        // Reveal-on-hover: hide idle background and border
        if (cfg.appear_on_hover) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0)); // no idle bg
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f); // no idle border
        }

        // 1) Рисует стандартный фон кнопки с темой/hover/active/disabled
        bool pressed = ImGui::Button("##bg", sz);
        
        if (cfg.appear_on_hover) {
            bool hovered = ImGui::IsItemHovered();
            bool held    = ImGui::IsItemActive();
            bool focused = ImGui::IsItemFocused(); // keyboard/gamepad nav
            if ((hovered || held || focused) && cfg.show_hover_border && st.FrameBorderSize > 0.0f) {
                ImDrawList* dl = ImGui::GetWindowDrawList();
                ImVec2 a = ImGui::GetItemRectMin();
                ImVec2 b = ImGui::GetItemRectMax();
                dl->AddRect(a, b, ImGui::GetColorU32(ImGuiCol_Border),
                            rounding, 0, st.FrameBorderSize);
            }
            ImGui::PopStyleVar();   // FrameBorderSize
            ImGui::PopStyleColor(); // ImGuiCol_Button
        }

        // 2) Центруем иконку/текст внутри только что отрисованной кнопки
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        ImVec2 cen = ImVec2((min.x + max.x)*0.5f, (min.y + max.y)*0.5f);

        if (cfg.font) ImGui::PushFont(cfg.font);
        ImVec2 ts = ImGui::CalcTextSize(text);
        auto snap = [](float v){ return std::floor(v) + 0.5f; }; // crisp
        ImVec2 pos = ImVec2(snap(cen.x - ts.x*0.5f + cfg.text_offset.x),
                            snap(cen.y - ts.y*0.5f + cfg.text_offset.y));
        dl->AddText(pos, ImGui::GetColorU32(ImGuiCol_Text), text);
        if (cfg.font) ImGui::PopFont();

        if (cfg.rounding >= 0.0f)
            ImGui::PopStyleVar();

        ImGui::PopID();
        return pressed;
    }

} // namespace ImGuiX::Widgets

