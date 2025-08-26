#pragma once
#ifndef _IMGUIX_WIDGETS_VALIDATED_INPUT_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VALIDATED_INPUT_HPP_INCLUDED

/// \file validated_input.hpp
/// \brief InputTextWithHint for std::string with regex validation and error tinting.

#include <imgui_stdlib.h>

#include <imguix/extensions/validation.hpp>
#include <imguix/config/icons.hpp>
#include <imguix/config/colors.hpp>
#include "virtual_keyboard_overlay.hpp"

namespace ImGuiX::Widgets {

    /// \brief Policy controlling when validation is applied in InputTextValidated.
    enum class InputValidatePolicy {
        OnDraw,           ///< Validate and tint on every frame (default).
        OnChange,         ///< Validate only after user input; initial value not tinted.
        OnChangeNonEmpty, ///< Validate only after input, but skip empty string (always valid).
        OnTouch           ///< Validate after first user edit; before that, initial value always valid.
    };
    
    /// \brief Visual config for the VK trigger button (reusable across inputs).
    struct KeyboardToggleConfig {
        bool        use_icon           = true;                  ///< icon or text
        const char* icon_text          = IMGUIX_ICON_KEYBOARD;  ///< PUA 'keyboard' (e.g. Material)
        const char* text               = u8"[KB]";              ///< text if use_icon==false
        ImFont*     icon_font          = nullptr;               ///< icon font (nullptr => current)
        ImVec2      button_size        = ImVec2(0,0);           ///< (0,0) => square by frame height
        float       same_line_w        = 0.0f;                  ///< SameLine(offset) before button
        float       icon_baseline      = 0.0f;                  ///< Y offset for baseline tuning
        float       icon_rounding      = -1.0f;                 ///< bg rounding override; -1 => keep style
        const char* tooltip_toggle_on  = u8"Show keyboard";
        const char* tooltip_toggle_off = u8"Hide keyboard";
        ImVec2      overlay_size       = ImVec2(0,0);           ///< 0,0 => auto
    };

    /// \brief InputTextWithHint wrapper with regex validation and error tinting.
    /// \param label      ImGui label (must be unique).
    /// \param hint       Placeholder text.
    /// \param value      Reference to std::string storing the text.
    /// \param validate   Enable regex validation.
    /// \param policy     When to apply validation (see InputValidatePolicy).
    /// \param pattern    Regex pattern to check against.
    /// \param out_valid  Output: true if value matches regex (or validation disabled).
    /// \param error_color Tint color when invalid (default: reddish).
    /// \param flags      ImGui input flags (e.g., ImGuiInputTextFlags_Password).
    /// \param callback   Optional ImGui input callback.
    /// \param user_data  Optional user data for callback.
    /// \return True if the value changed this frame.
    inline bool InputTextValidated(
            const char* label,
            const char* hint,
            std::string& value,
            bool validate,
            InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            ImVec4 error_color = IMGUIX_COLOR_ERROR,
            ImGuiInputTextFlags flags = 0,
            ImGuiInputTextCallback callback = nullptr,
            void* user_data = nullptr
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
        
        // store hash AFTER draw (captures user edits)
        const uint32_t cur_hash_after = hash32(value);
        st->SetInt(key_prev_hash, static_cast<int>(cur_hash_after));
        
        // --- trailing label (print only visible part of `label` before "##")
        {
            // find end of visible label (stop at "##" if present)
            const char* label_end = label;
            while (*label_end != '\0' && !(label_end[0] == '#' && label_end[1] == '#'))
                ++label_end;

            const bool has_visible = (label_end > label); // non-empty prefix
            if (has_visible) {
                // keep standard inner spacing between item and label (like core widgets)
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
                ImGui::TextUnformatted(label, label_end);
            }
        }
        
        ImGui::PopID();
        return changed || external_changed;
    }
    
    /// \brief InputTextWithHint + regex validation + on-screen keyboard trigger (overlay).
    ///
    /// This variant extends \ref InputTextValidated by adding a compact button on the same line
    /// that toggles a centered \ref VirtualKeyboardOverlay for editing the same bound \p value.
    /// The button visuals are provided by \ref KeyboardToggleConfig. The keyboard behavior (locales,
    /// submit behavior, etc.) is configured via \ref VirtualKeyboardConfig.
    ///
    /// **Layout:** the input reserves horizontal space for one square button to avoid width jumps.  
    /// **Focus:** when the overlay submits (Enter) or closes, the function requests focus back
    /// to the input (`SetKeyboardFocusHere(-1)`; consider your layout order).
    /// **State:** keyboard visibility is persisted per-field in ImGuiStorage under "##vk_visible".
    ///
    /// \param label       Unique label/ID scope for the pair (input + button).
    /// \param hint        Placeholder text for the input.
    /// \param value       Bound std::string edited by both the input and the VK overlay.
    /// \param validate    Enables/disables validation.
    /// \param policy      When to apply validation (see \ref InputValidatePolicy).
    /// \param pattern     Regex pattern (ECMA) for validation.
    /// \param out_valid   Out: true if \p value matches \p pattern (or \p validate==false).
    /// \param error_color Tint applied while invalid (frame/text).
    /// \param extra_flags Extra ImGuiInputTextFlags OR-ed into the input (optional).
    /// \param callback    Optional ImGui input callback for the input field.
    /// \param user_data   Optional user data for the input callback.
    /// \param kb_cfg      Visual config for the VK trigger button (icon/text, size, tooltip, etc.).
    /// \param vk_cfg      Behavior/visual config for the on-screen keyboard overlay.
    /// \return true if the value changed this frame (via input or VK).
    inline bool InputTextWithVKValidated(
            const char* label,
            const char* hint,
            std::string& value,
            bool validate,
            ImGuiX::Widgets::InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            ImVec4 error_color = IMGUIX_COLOR_ERROR,
            ImGuiInputTextFlags extra_flags = 0,
            ImGuiInputTextCallback callback = nullptr,
            void* user_data = nullptr,
            const KeyboardToggleConfig& kb_cfg = {},
            VirtualKeyboardConfig vk_cfg = {}
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
            // Text button needs visible label + unique ID suffix.
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

        // --- overlay VK
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
            // find end of visible label (stop at "##" if present)
            const char* label_end = label;
            while (*label_end != '\0' && !(label_end[0] == '#' && label_end[1] == '#'))
                ++label_end;

            const bool has_visible = (label_end > label); // non-empty prefix
            if (has_visible) {
                // keep standard inner spacing between item and label (like core widgets)
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
                ImGui::TextUnformatted(label, label_end);
            }
        }

        ImGui::PopID();
        return changed;
    }
    
    /// \brief Convenience overload of \ref InputTextWithVKValidated without flags/callbacks.
    ///
    /// This overload keeps the same behavior but omits low-level input parameters. It forwards to
    /// the full version with default \p extra_flags, \p callback and \p user_data.
    ///
    /// \param label       Unique label/ID scope for the pair (input + button).
    /// \param hint        Placeholder text for the input.
    /// \param value       Bound std::string edited by both the input and the VK overlay.
    /// \param validate    Enables/disables validation.
    /// \param policy      When to apply validation (see \ref InputValidatePolicy).
    /// \param pattern     Regex pattern (ECMA) for validation.
    /// \param out_valid   Out: true if \p value matches \p pattern (or \p validate==false).
    /// \param kb_cfg Visual config for the VK trigger button.
    /// \param vk_cfg Behavior/visual config for the on-screen keyboard overlay.
    /// \param error_color Tint applied while invalid (frame/text).
    /// \return true if the value changed this frame (via input or VK).
    inline bool InputTextWithVKValidated(
            const char* label,
            const char* hint,
            std::string& value,
            bool validate,
            ImGuiX::Widgets::InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            const KeyboardToggleConfig& kb_cfg = {},
            VirtualKeyboardConfig vk_cfg = {},
            ImVec4 error_color = ImVec4(0.9f,0.5f,0.5f,1.0f)
        ) {
        return InputTextWithVKValidated(
            label,
            hint,
            value,
            validate,
            policy,
            pattern,
            out_valid,
            error_color,
            0,
            nullptr,
            nullptr,
            kb_cfg,
            std::move(vk_cfg));
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_VALIDATED_INPUT_HPP_INCLUDED
