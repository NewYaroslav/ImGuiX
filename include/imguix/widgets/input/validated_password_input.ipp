#include <algorithm>

#include <imgui.h>

#include <imguix/widgets/controls/icon_button.hpp>

namespace ImGuiX::Widgets {

bool InputPasswordWithToggle(
        const char* label,
        const char* hint,
        std::string& value,
        bool validate,
        InputValidatePolicy policy,
        const std::string& pattern,
        bool& out_valid,
        const PasswordToggleConfig& toggle_cfg,
        ImVec4 error_color,
        ImGuiInputTextFlags extra_flags
) {
    ImGui::PushID(label);

    ImGuiStorage* st = ImGui::GetStateStorage();
    const ImGuiID key_show_pwd = ImGui::GetID(u8"##show_pwd");
    bool show_password_io = st->GetInt(key_show_pwd, 0) != 0;

    ImGuiStyle& style = ImGui::GetStyle();
    const float full_width = ImGui::CalcItemWidth();
    const float frame_h    = ImGui::GetFrameHeight();

    ImVec2 btn_sz = toggle_cfg.button_size;
    if (btn_sz.x <= 0.0f || btn_sz.y <= 0.0f) {
        btn_sz = ImVec2(frame_h, frame_h);
    }

    const float spacing   = style.ItemInnerSpacing.x;
    const float input_w   = std::max(0.0f, full_width - (btn_sz.x + spacing));

    ImGui::SetNextItemWidth(input_w);
    const ImGuiInputTextFlags pwd_flag = show_password_io ? 0 : ImGuiInputTextFlags_Password;
    const ImGuiInputTextFlags flags    = pwd_flag | extra_flags;

    bool changed = InputTextValidated(
        "##input", hint,
        value, validate, policy, pattern,
        out_valid, error_color, flags
    );

    {
        if (toggle_cfg.same_line_w != 0.0f) ImGui::SameLine(toggle_cfg.same_line_w);
        else                                ImGui::SameLine(0.0f, spacing);

        ImVec4 col_btn       = style.Colors[ImGuiCol_FrameBg];
        ImVec4 col_btn_hover = style.Colors[ImGuiCol_FrameBgHovered];
        ImVec4 col_btn_active= style.Colors[ImGuiCol_FrameBgActive];
        ImGui::PushStyleColor(ImGuiCol_Button,        col_btn);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col_btn_hover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  col_btn_active);

        const bool has_rounding = (toggle_cfg.icon_rounding >= 0.0f);
        if (has_rounding) ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, toggle_cfg.icon_rounding);

        bool toggled = false;
        if (toggle_cfg.use_icon) {
            IconButtonConfig eye{};
            eye.font        = toggle_cfg.icon_font;
            eye.size        = btn_sz;
            eye.text_offset = ImVec2(0.0f, toggle_cfg.icon_baseline);
            eye.rounding    = toggle_cfg.icon_rounding;
            const char* icon = show_password_io
                ? (toggle_cfg.icon_hide ? toggle_cfg.icon_hide : u8"X")
                : (toggle_cfg.icon_show ? toggle_cfg.icon_show : u8"V");
            toggled = IconButtonCentered("##eye", icon, eye);
        } else {
            bool tmp = show_password_io;
            ImGui::SetNextItemWidth(btn_sz.x);
            toggled = ImGui::Checkbox("##eye_chk", &tmp);
            if (toggled) show_password_io = tmp;
            if (toggle_cfg.text_label && *toggle_cfg.text_label) {
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x * 0.5f);
                ImGui::TextUnformatted(toggle_cfg.text_label);
            }
        }

        if (toggled) {
            show_password_io = !show_password_io;
            st->SetInt(key_show_pwd, show_password_io ? 1 : 0);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", show_password_io
                ? (toggle_cfg.tooltip_hide ? toggle_cfg.tooltip_hide : "Hide password")
                : (toggle_cfg.tooltip_show ? toggle_cfg.tooltip_show : "Show password"));
        }

        if (has_rounding) ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
    }

    {
        const char* label_end = label;
        while (*label_end != '\0' && !(label_end[0] == '#' && label_end[1] == '#'))
            ++label_end;
        const bool has_visible = (label_end > label);
        if (has_visible) {
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::TextUnformatted(label, label_end);
        }
    }

    ImGui::PopID();
    return changed;
}

bool InputPasswordWithToggleVK(
        const char* label,
        const char* hint,
        std::string& value,
        bool validate,
        InputValidatePolicy policy,
        const std::string& pattern,
        bool& out_valid,
        const PasswordToggleConfig& toggle_cfg,
        const KeyboardToggleConfig& kb_cfg,
        VirtualKeyboardConfig vk_cfg,
        ImVec4 error_color,
        ImGuiInputTextFlags extra_flags
) {
    ImGui::PushID(label);

    ImGuiStorage* st = ImGui::GetStateStorage();
    const ImGuiID key_show_pwd = ImGui::GetID(u8"##show_pwd");
    const ImGuiID key_vk_visible = ImGui::GetID(u8"##vk_visible");
    bool show_password_io = st->GetInt(key_show_pwd, 0) != 0;
    bool vk_visible = st->GetInt(key_vk_visible, 0) != 0;

    ImGuiStyle& style = ImGui::GetStyle();
    const float full_width = ImGui::CalcItemWidth();
    const float frame_h    = ImGui::GetFrameHeight();

    ImVec2 btn_sz = toggle_cfg.button_size;
    if (btn_sz.x <= 0.0f || btn_sz.y <= 0.0f) {
        btn_sz = ImVec2(frame_h, frame_h);
    }

    const float spacing   = style.ItemInnerSpacing.x;
    const float input_w   = std::max(0.0f, full_width - 2.0f * (btn_sz.x + spacing));

    ImGui::SetNextItemWidth(input_w);
    const ImGuiInputTextFlags pwd_flag = show_password_io ? 0 : ImGuiInputTextFlags_Password;
    const ImGuiInputTextFlags flags    = pwd_flag | extra_flags;

    bool changed = InputTextValidated(
        "##input", hint,
        value, validate, policy, pattern,
        out_valid, error_color, flags
    );

    {
        if (toggle_cfg.same_line_w != 0.0f) ImGui::SameLine(toggle_cfg.same_line_w);
        else                                ImGui::SameLine(0.0f, spacing);

        ImVec4 col_btn       = style.Colors[ImGuiCol_FrameBg];
        ImVec4 col_btn_hover = style.Colors[ImGuiCol_FrameBgHovered];
        ImVec4 col_btn_active= style.Colors[ImGuiCol_FrameBgActive];
        ImGui::PushStyleColor(ImGuiCol_Button,        col_btn);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col_btn_hover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  col_btn_active);

        const bool has_rounding = (toggle_cfg.icon_rounding >= 0.0f);
        if (has_rounding) ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, toggle_cfg.icon_rounding);

        bool toggled = false;
        if (toggle_cfg.use_icon) {
            IconButtonConfig eye{};
            eye.font        = toggle_cfg.icon_font;
            eye.size        = btn_sz;
            eye.text_offset = ImVec2(0.0f, toggle_cfg.icon_baseline);
            eye.rounding    = toggle_cfg.icon_rounding;
            const char* icon = show_password_io
                ? (toggle_cfg.icon_hide ? toggle_cfg.icon_hide : u8"X")
                : (toggle_cfg.icon_show ? toggle_cfg.icon_show : u8"V");
            toggled = IconButtonCentered("##eye", icon, eye);
        } else {
            bool tmp = show_password_io;
            ImGui::SetNextItemWidth(btn_sz.x);
            toggled = ImGui::Checkbox("##eye_chk", &tmp);
            if (toggled) show_password_io = tmp;
            if (toggle_cfg.text_label && *toggle_cfg.text_label) {
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x * 0.5f);
                ImGui::TextUnformatted(toggle_cfg.text_label);
            }
        }

        if (toggled) {
            show_password_io = !show_password_io;
            st->SetInt(key_show_pwd, show_password_io ? 1 : 0);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", show_password_io
                ? (toggle_cfg.tooltip_hide ? toggle_cfg.tooltip_hide : "Hide password")
                : (toggle_cfg.tooltip_show ? toggle_cfg.tooltip_show : "Show password"));
        }

        if (has_rounding) ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
    }

    if (kb_cfg.same_line_w != 0.0f) ImGui::SameLine(kb_cfg.same_line_w);
    else                            ImGui::SameLine(0.0f, spacing);

    ImVec4 c0 = style.Colors[ImGuiCol_FrameBg];
    ImVec4 c1 = style.Colors[ImGuiCol_FrameBgHovered];
    ImVec4 c2 = style.Colors[ImGuiCol_FrameBgActive];
    ImGui::PushStyleColor(ImGuiCol_Button,        c0);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, c1);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  c2);
    const bool round_override = (kb_cfg.icon_rounding >= 0.0f);
    if (round_override) ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, kb_cfg.icon_rounding);

    bool pressed = false;
    if (kb_cfg.use_icon) {
        IconButtonConfig ib{};
        ib.font        = kb_cfg.icon_font;
        ib.size        = btn_sz;
        ib.text_offset = ImVec2(0.0f, kb_cfg.icon_baseline);
        ib.rounding    = kb_cfg.icon_rounding;
        pressed = ImGuiX::Widgets::IconButtonCentered("##vk_btn", kb_cfg.icon_text ? kb_cfg.icon_text : u8">", ib);
    } else {
        const char* txt = kb_cfg.text ? kb_cfg.text : ">";
        pressed = ImGui::Button(txt, btn_sz);
    }

    if (round_override) ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    if (pressed) vk_visible = !vk_visible;
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", vk_visible
            ? (kb_cfg.tooltip_toggle_off ? kb_cfg.tooltip_toggle_off : "Hide keyboard")
            : (kb_cfg.tooltip_toggle_on  ? kb_cfg.tooltip_toggle_on  : "Show keyboard"));
    }

    if (vk_visible) {
        bool should_close = false;
        auto user_submit  = vk_cfg.on_submit;
        vk_cfg.on_submit = [&](const std::string& s){
            if (user_submit) user_submit(s);
            should_close = true;
        };
        vk_cfg.submit_on_enter = true;

        bool ov = true;
        (void)ImGuiX::Widgets::VirtualKeyboardOverlay(
            u8"##vk_overlay_window",
            value, std::move(vk_cfg), &ov, kb_cfg.overlay_size
        );
        vk_visible = ov;
        if (should_close) {
            vk_visible = false;
            ImGui::SetKeyboardFocusHere(-1);
        }
    }

    st->SetInt(key_vk_visible, vk_visible ? 1 : 0);

    {
        const char* label_end = label;
        while (*label_end != '\0' && !(label_end[0] == '#' && label_end[1] == '#'))
            ++label_end;
        const bool has_visible = (label_end > label);
        if (has_visible) {
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::TextUnformatted(label, label_end);
        }
    }

    ImGui::PopID();
    return changed;
}

} // namespace ImGuiX::Widgets

