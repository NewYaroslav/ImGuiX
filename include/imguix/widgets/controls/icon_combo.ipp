namespace ImGuiX::Widgets {

    inline bool BeginIconCombo(
            const char* id,
            const char* preview,
            const char* icon,
            float icon_slot_w,
            ImGuiComboFlags flags
    ) {
        bool open = ImGui::BeginCombo(id, preview, flags | ImGuiComboFlags_NoArrowButton);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();

        ImVec2 isz = ImGui::CalcTextSize(icon ? icon : "");
        float slot_w = icon_slot_w > 0.0f ? icon_slot_w
            : (isz.x + ImGui::GetStyle().FramePadding.x * 2.0f);

        ImU32 slot_bg = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);
        dl->AddRectFilled(ImVec2(p1.x - slot_w, p0.y), p1, slot_bg, 0.0f);

        ImVec2 ip;
        ip.x = p1.x - slot_w + (slot_w - isz.x) * 0.5f;
        ip.y = p0.y + (p1.y - p0.y - isz.y) * 0.5f;
        dl->AddText(ip, ImGui::GetColorU32(ImGuiCol_Text), icon ? icon : "");

        return open;
    }

    inline void EndIconCombo() {
        ImGui::EndCombo();
    }

} // namespace ImGuiX::Widgets
