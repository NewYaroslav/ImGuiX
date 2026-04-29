#include <algorithm>
#include <utility>

#include <imguix/config/build.hpp>
#include <imguix/extensions/validation.hpp>
#include <imguix/widgets/misc/markers.hpp>

namespace ImGuiX::Widgets {

    IMGUIX_IMPL_INLINE bool InputTextMultilineValidated(
            const char* label,
            std::string& value,
            const ImVec2& size,
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

        const ImGuiID key_touched = ImGui::GetID("##validated_touched");
        const ImGuiID key_prev_hash = ImGui::GetID("##validated_prev32");

        auto hash32 = [](const std::string& s) -> uint32_t {
            uint32_t h = 2166136261u;
            for (unsigned char c : s) {
                h ^= c;
                h *= 16777619u;
            }
            return h;
        };

        auto match = [&](const std::string& s) {
            return !validate || RegexMatchSafe(s, pattern);
        };

        const uint32_t prev_hash = static_cast<uint32_t>(st->GetInt(key_prev_hash, -1));
        const uint32_t cur_hash_before = hash32(value);
        const bool first_frame_for_id = (prev_hash == static_cast<uint32_t>(-1));
        const bool external_changed = (!first_frame_for_id && prev_hash != cur_hash_before);

        bool pre_tint = false;
        switch (policy) {
            case InputValidatePolicy::OnDraw:           pre_tint = true; break;
            case InputValidatePolicy::OnChange:         pre_tint = false; break;
            case InputValidatePolicy::OnChangeNonEmpty: pre_tint = !value.empty(); break;
            case InputValidatePolicy::OnTouch:
                pre_tint = st->GetBool(key_touched, false) || external_changed;
                break;
        }

        bool valid_now = match(value);
        const bool error_active = (pre_tint && !valid_now);
        ScopedInvalid tint(error_active, error_color);

        ImGuiInputTextFlags effective_flags = flags & ~ImGuiInputTextFlags_Password;
        bool changed = ImGui::InputTextMultiline(
            "##input_text",
            &value,
            size,
            effective_flags,
            callback,
            user_data
        );

        tint.end();

        if (changed) {
            valid_now = match(value);
            if (policy == InputValidatePolicy::OnTouch) {
                st->SetBool(key_touched, true);
            }
        } else if (external_changed) {
            valid_now = !validate || RegexMatchSafe(value, pattern);
            if (policy == InputValidatePolicy::OnTouch) {
                st->SetBool(key_touched, true);
            }
        }

        out_valid = valid_now;
        st->SetInt(key_prev_hash, static_cast<int>(hash32(value)));

        ImGui::PopID();
        return changed || external_changed;
    }

    IMGUIX_IMPL_INLINE bool InputTextMultilineWithVKValidated(
            const char* label,
            std::string& value,
            const ImVec2& size,
            bool validate,
            InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            ImVec4 error_color,
            ImGuiInputTextFlags extra_flags,
            ImGuiInputTextCallback callback,
            void* user_data,
            const KeyboardToggleConfig& kb_cfg,
            VirtualKeyboardConfig vk_cfg,
            bool show_help,
            const char* help_text
        ) {
        using ImGuiX::Extensions::RegexMatchSafe;

        ImGui::PushID(label);

        ImGuiStorage* st = ImGui::GetStateStorage();
        const ImGuiID key_vk_visible = ImGui::GetID("##vk_visible");
        const ImGuiID key_touched = ImGui::GetID("##validated_touched");
        const ImGuiID key_prev_hash = ImGui::GetID("##validated_prev32");
        bool vk_visible = st->GetInt(key_vk_visible, 0) != 0;

        ImGuiStyle& style = ImGui::GetStyle();
        const float frame_h = ImGui::GetFrameHeight();
        const float spacing = style.ItemInnerSpacing.x;
        const float full_width = size.x > 0.0f ? size.x : ImGui::CalcItemWidth();
        const float input_h = size.y > 0.0f ? size.y : (frame_h * 4.0f);

        ImVec2 btn_sz = kb_cfg.button_size;
        if (btn_sz.x <= 0.0f || btn_sz.y <= 0.0f) {
            btn_sz = ImVec2(frame_h, frame_h);
        }

        float reserved_w = btn_sz.x + spacing;
        if (show_help && help_text && help_text[0] != '\0') {
            reserved_w += spacing + ImGui::CalcTextSize(IMGUIX_ICON_HELP).x;
        }

        const float input_w = std::max(1.0f, full_width - reserved_w);

        bool changed = InputTextMultilineValidated(
            "##input",
            value,
            ImVec2(input_w, input_h),
            validate,
            policy,
            pattern,
            out_valid,
            error_color,
            extra_flags,
            callback,
            user_data
        );

        if (kb_cfg.same_line_w != 0.0f) {
            ImGui::SameLine(kb_cfg.same_line_w);
        } else {
            ImGui::SameLine(0.0f, spacing);
        }

        ImVec4 c0 = style.Colors[ImGuiCol_FrameBg];
        ImVec4 c1 = style.Colors[ImGuiCol_FrameBgHovered];
        ImVec4 c2 = style.Colors[ImGuiCol_FrameBgActive];
        ImGui::PushStyleColor(ImGuiCol_Button, c0);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, c1);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, c2);

        const bool round_override = (kb_cfg.icon_rounding >= 0.0f);
        if (round_override) {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, kb_cfg.icon_rounding);
        }

        bool pressed = false;
        if (kb_cfg.use_icon) {
            IconButtonConfig ib{};
            ib.font = kb_cfg.icon_font;
            ib.size = btn_sz;
            ib.text_offset = ImVec2(0.0f, kb_cfg.icon_baseline);
            ib.rounding = kb_cfg.icon_rounding;
            pressed = ImGuiX::Widgets::IconButtonCentered(
                "##vk_btn",
                kb_cfg.icon_text ? kb_cfg.icon_text : u8">",
                ib
            );
        } else {
            const char* txt = kb_cfg.text ? kb_cfg.text : ">";
            pressed = ImGui::Button(txt, btn_sz);
        }

        if (round_override) {
            ImGui::PopStyleVar();
        }
        ImGui::PopStyleColor(3);

        if (pressed) {
            vk_visible = !vk_visible;
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(
                "%s",
                vk_visible
                    ? (kb_cfg.tooltip_toggle_off ? kb_cfg.tooltip_toggle_off : "Hide keyboard")
                    : (kb_cfg.tooltip_toggle_on ? kb_cfg.tooltip_toggle_on : "Show keyboard")
            );
        }

        if (show_help && help_text && help_text[0] != '\0') {
            ImGui::SameLine(0.0f, spacing);
            HelpMarker(help_text);
        }

        bool overlay_changed = false;
        if (vk_visible) {
            bool should_close = false;
            auto user_submit = vk_cfg.on_submit;
            vk_cfg.on_submit = [&](const std::string& s) {
                if (user_submit) {
                    user_submit(s);
                }
                should_close = true;
            };
            vk_cfg.submit_on_enter = true;

            bool ov = true;
            overlay_changed = ImGuiX::Widgets::VirtualKeyboardOverlay(
                u8"##vk_overlay_window",
                value,
                std::move(vk_cfg),
                &ov,
                kb_cfg.overlay_size
            );
            vk_visible = ov;
            if (should_close) {
                vk_visible = false;
            }
        }

        if (overlay_changed) {
            out_valid = !validate || RegexMatchSafe(value, pattern);
            if (policy == InputValidatePolicy::OnTouch) {
                st->SetBool(key_touched, true);
            }

            auto hash32 = [](const std::string& s) -> uint32_t {
                uint32_t h = 2166136261u;
                for (unsigned char c : s) {
                    h ^= c;
                    h *= 16777619u;
                }
                return h;
            };
            st->SetInt(key_prev_hash, static_cast<int>(hash32(value)));
        }

        st->SetInt(key_vk_visible, vk_visible ? 1 : 0);
        ImGui::PopID();
        return changed || overlay_changed;
    }

} // namespace ImGuiX::Widgets
