#include "InputTextWithIconToggle.hpp"

namespace ImGuiX::Widgets {

bool InputTextWithIconToggle(
        const char* label,
        const char* hint,
        char*       buf,
        int         buf_sz,
        bool*       reveal,
        ImGuiInputTextFlags flags,
        const char* eye_on,
        const char* eye_off,
        ImFont*     icon_font,
        ImGuiInputTextCallback callback,
        void*       user_data,
        ImU32 eye_text_col_override,
        ImU32 eye_bg_col_override
    ) {
    IM_ASSERT(label && buf && buf_sz > 0 && reveal);
    IM_ASSERT((flags & ImGuiInputTextFlags_Multiline) == 0);

    ImGuiContext& g = *GImGui;
    ImGuiWindow*  w = ImGui::GetCurrentWindow();
    if (w->SkipItems) return false;
    const ImGuiStyle& st = g.Style;

    ImGui::PushID(label);
    const ImGuiID input_id = w->GetID("##input");
    const ImGuiID eye_id   = w->GetID("##eye");

    const float h        = ImGui::GetFrameHeight();
    const float w_total  = ImGui::CalcItemWidth();
    const float input_w  = (w_total > h ? w_total - h : ImMax(1.0f, w_total * 0.66f));

    const ImVec2 pos0    = w->DC.CursorPos;
    const ImRect input_bb(pos0, pos0 + ImVec2(input_w, h));
    const ImRect eye_bb(ImVec2(input_bb.Max.x, input_bb.Min.y),
                        ImVec2(input_bb.Max.x + h, input_bb.Max.y));

    // *** Рисуем в 2 канала: 0 — фон, 1 — всё остальное
    ImDrawList* dl = w->DrawList;
    dl->ChannelsSplit(2);
    dl->ChannelsSetCurrent(1);

    // *** Отключаем фон у InputTextEx и убираем скругление (чтобы не было правого «выкуса»)
    const ImVec4 c0 = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
    const ImVec4 cH = ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered);
    const ImVec4 cA = ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg,        ImVec4(c0.x, c0.y, c0.z, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(cH.x, cH.y, cH.z, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  ImVec4(cA.x, cA.y, cA.z, 0.0f));

    ImGuiInputTextFlags eff = flags | (*reveal ? 0 : ImGuiInputTextFlags_Password);
    ImGui::SetNextItemWidth(input_bb.GetWidth());
    bool changed = ImGui::InputTextEx("##input", hint, buf, buf_sz, ImVec2(input_bb.GetWidth(), 0.0f), eff, callback, user_data);

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    // Параметры состояния для вбора цвета фона
    const bool input_active  = (g.ActiveId == input_id);
    const bool input_hovered = ImGui::IsItemHovered();

    // *** Рисуем ЛЕВЫЙ фон (FrameBg/*) с кругл. левыми углами — в ЗАДНЕМ канале
    dl->ChannelsSetCurrent(0);
    ImU32 left_bg_col = ImGui::GetColorU32(input_active ? ImGuiCol_FrameBgActive
                                   : input_hovered ? ImGuiCol_FrameBgHovered
                                                   : ImGuiCol_FrameBg);
    dl->AddRectFilled(input_bb.Min, input_bb.Max, left_bg_col, st.FrameRounding, ImDrawFlags_RoundCornersLeft);

    // *** Возвращаемся на передний канал и рисуем правый сегмент (как раньше)
    dl->ChannelsSetCurrent(1);

    ImGui::KeepAliveID(eye_id);
    bool eye_added = ImGui::ItemAdd(eye_bb, eye_id, nullptr, ImGuiItemFlags_NoNav);
    bool hovered = false, held = false, pressed = false;
    if (eye_added)
        pressed = ImGui::ButtonBehavior(eye_bb, eye_id, &hovered, &held);

    const ImU32 seg_bg  = eye_bg_col_override
        ? eye_bg_col_override
        : ImGui::GetColorU32((held || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

    const ImU32 seg_text = eye_text_col_override
        ? eye_text_col_override
        : ImGui::GetColorU32(ImGuiCol_Text);

    dl->AddRectFilled(eye_bb.Min, eye_bb.Max, seg_bg, st.FrameRounding, ImDrawFlags_RoundCornersRight);

    const char* icon = (*reveal ? eye_on : eye_off);
    if (icon && *icon) {
        if (icon_font) ImGui::PushFont(icon_font);
        const ImVec2 isz = ImGui::CalcTextSize(icon);
        const ImVec2 ip  = ImVec2(eye_bb.Min.x + (eye_bb.GetWidth()  - isz.x) * 0.5f,
                                  eye_bb.Min.y + (eye_bb.GetHeight() - isz.y) * 0.5f);
        dl->AddText(ip, seg_text, icon);
        if (icon_font) ImGui::PopFont();
    }

    if (pressed) {
        *reveal = !*reveal;
        ImGui::SetActiveID(input_id, w);
        ImGui::SetFocusID(input_id, w);
        ImGui::FocusWindow(w);
    }

    // Общая рамка по периметру (если нужна)
    if (st.FrameBorderSize > 0.0f)
        ImGui::RenderFrameBorder(input_bb.Min, ImVec2(eye_bb.Max.x, input_bb.Max.y), st.FrameRounding);

    // Лейбл справа
    const ImVec2 label_sz = ImGui::CalcTextSize(label, nullptr, true);
    if (label_sz.x > 0.0f) {
        const ImVec2 lp = ImVec2(eye_bb.Max.x + st.ItemInnerSpacing.x,
                                 input_bb.Min.y + st.FramePadding.y);
        ImGui::RenderText(lp, label);
    }

    // Сливаем каналы (фон остаётся позади текста/каретки)
    dl->ChannelsMerge();

    // Итоговый размер слота — input + eye + label
    const float total_w = input_bb.GetWidth() + h + (label_sz.x > 0.0f ? (st.ItemInnerSpacing.x + label_sz.x) : 0.0f);
    ImGui::SetCursorScreenPos(pos0);
    ImGui::ItemSize(ImVec2(total_w, input_bb.GetHeight()), st.FramePadding.y);

    ImGui::PopID();
    return changed;
}

#ifdef IMGUIX_DEMO
void DemoInputTextWithIconToggle() {
    // Icons: Material Symbols PUA
    static const char* kEyeOn  = u8"\uE8F4";
    static const char* kEyeOff = u8"\uE8F5";

    static bool reveal_pwd = true;
    static char pass_buf[128]{};

    ImFont* material_font = /* your icon font */ nullptr;

    ImGuiX::Widgets::InputTextWithIconToggle(
        "Password",
        "Enter password...",
        pass_buf, IM_ARRAYSIZE(pass_buf),
        &reveal_pwd,
        ImGuiInputTextFlags_None,
        kEyeOn, kEyeOff,
        material_font);
}
#endif

} // namespace ImGuiX::Widgets
