#include <imguix/extensions/validation.hpp>

namespace ImGuiX::Widgets {

bool InputTextValidated(
        const char* label,
        const char* hint,
        std::string& value,
        bool validate,
        InputValidatePolicy policy,
        const std::string& pattern,
        bool& out_valid,
        ImVec4 error_color,
        ImGuiInputTextFlags flags,
        ImGuiInputTextCallback callback,
        void* user_data
) {
    using ImGuiX::Extensions::RegexMatchSafe;
    using ImGuiX::Extensions::ScopedInvalid;

    ImGui::PushID(label);
    ImGuiStorage* st = ImGui::GetStateStorage();

    const ImGuiID key_touched   = ImGui::GetID("##validated_touched");
    const ImGuiID key_prev_hash = ImGui::GetID("##validated_prev32");

    // simple FNV-1a 32-bit
    auto hash32 = [](const std::string& s)->uint32_t {
        uint32_t h = 2166136261u;
        for (unsigned char c : s) { h ^= c; h *= 16777619u; }
        return h;
    };

    auto match = [&](const std::string& s){
                    return !validate || RegexMatchSafe(s, pattern);
            };

    // hash BEFORE drawing the input (to detect external changes)
    const uint32_t prev_hash = static_cast<uint32_t>(st->GetInt(key_prev_hash, -1));
    const uint32_t cur_hash_before = hash32(value);
    const bool first_frame_for_id = (prev_hash == static_cast<uint32_t>(-1));
    const bool external_changed = (!first_frame_for_id && prev_hash != cur_hash_before);

    // choose when to tint
    bool pre_tint = false;
    switch (policy) {
        case InputValidatePolicy::OnDraw:           pre_tint = true; break;
        case InputValidatePolicy::OnChange:         pre_tint = false; break;
        case InputValidatePolicy::OnChangeNonEmpty: pre_tint = !value.empty(); break;
        case InputValidatePolicy::OnTouch: {
            pre_tint = st->GetBool(key_touched, false) || external_changed;
        } break;
    }

    bool valid_now = match(value);
    ScopedInvalid tint(pre_tint && !valid_now, error_color);

    bool changed = ImGui::InputTextWithHint(
        "##input_text", hint, &value, flags, callback, user_data
    );

    tint.end();

    // recompute validity and touched flag
    if (changed) {
        valid_now = match(value);
        if (policy == InputValidatePolicy::OnTouch) {
            st->SetBool(key_touched, true);
        }
    } else
    if (external_changed) {
        valid_now = !validate || RegexMatchSafe(value, pattern);
        if (policy == InputValidatePolicy::OnTouch) {
            st->SetBool(key_touched, true);
        }
    }
    out_valid = valid_now;

    // update hash after input
    st->SetInt(key_prev_hash, static_cast<int>(hash32(value)));

    ImGui::PopID();
    return changed;
}

bool InputTextWithVKValidated(
        const char* label,
        const char* hint,
        std::string& value,
        bool validate,
        InputValidatePolicy policy,
        const std::string& pattern,
        bool& out_valid,
        ImVec4 error_color,
        ImGuiInputTextFlags extra_flags,
        ImGuiInputTextCallback callback,
        void* user_data,
        const KeyboardToggleConfig& kb_cfg,
        VirtualKeyboardConfig vk_cfg
) {
    ImGui::PushID(label);

    ImGuiStorage* st    = ImGui::GetStateStorage();
    const ImGuiID key_vk_visible = ImGui::GetID(u8"##vk_visible");
    bool vk_visible = st->GetInt(key_vk_visible, 0) != 0;

    ImGuiStyle& style   = ImGui::GetStyle();
    const float frame_h = ImGui::GetFrameHeight();
    const float full_width = ImGui::CalcItemWidth();

    // Compute button size (square by default)
    ImVec2 btn_sz = kb_cfg.button_size;
    if (btn_sz.x <= 0.0f || btn_sz.y <= 0.0f) {
        btn_sz = ImVec2(frame_h, frame_h);
    }

    // Compute input width (reserve space for button + inner spacing if toggle is enabled)
    const float spacing = style.ItemInnerSpacing.x;
    const float input_w = std::max(0.0f, full_width - (btn_sz.x + spacing));

    // --- input
    ImGui::SetNextItemWidth(input_w);
    bool changed = ImGuiX::Widgets::InputTextValidated(
        "##input", hint, value, validate, policy, pattern,
        out_valid, error_color, extra_flags, callback, user_data
    );

    // --- VK trigger button
    if (kb_cfg.same_line_w != 0.0f) ImGui::SameLine(kb_cfg.same_line_w);
    else                            ImGui::SameLine(0.0f, spacing);

    // unify visuals with FrameBg colors
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

    // VK overlay (centered). Wrap on_submit to auto-close and refocus.
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

    // --- trailing label (print only visible part of `label` before "##")
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

