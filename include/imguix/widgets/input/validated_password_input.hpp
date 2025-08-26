#pragma once
#ifndef _IMGUIX_WIDGETS_VALIDATED_PASSWORD_INPUT_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VALIDATED_PASSWORD_INPUT_HPP_INCLUDED

/// \file validated_password_input.hpp
/// \brief Password input with regex validation, inline "eye" toggle, and optional VK trigger.

#include <imgui.h>
#include <string>
#include "validated_input.hpp" // InputTextValidated, InputValidatePolicy, KeyboardToggleConf
#include <imguix/widgets/controls/icon_button.hpp>     // IconButtonCentered
#include <algorithm>

namespace ImGuiX::Widgets {

    /// \brief Configuration for the inline "eye" toggle button.
    ///
    /// Controls only the visibility toggle UI that switches between masked/unmasked input.
    /// The actual password masking is performed via ImGuiInputTextFlags_Password when hidden.
    /// Rounding is applied through style var override if \ref icon_rounding >= 0.
    struct PasswordToggleConfig {
        bool        use_icon      = true;        ///< true: icon button; false: checkbox + optional text label.
        const char* icon_show     = u8"\ue8f4";  ///< Glyph for "show" (depends on your icon font).
        const char* icon_hide     = u8"\ue8f5";  ///< Glyph for "hide".
        ImVec2      button_size   = ImVec2(0,0); ///< (0,0) => square by current frame height.
        float       same_line_w   = 0.0f;        ///< SameLine(offset) before the eye button; 0 => default spacing.
        const char* tooltip_show  = u8"Show password"; ///< Tooltip when currently hidden.
        const char* tooltip_hide  = u8"Hide password"; ///< Tooltip when currently visible.
        const char* text_label    = u8"Show";    ///< Checkbox label when use_icon==false.

        ImFont*     icon_font     = nullptr;     ///< Icon font to use (nullptr => current font).
        float       icon_baseline = 0.0f;        ///< Y offset to tune icon baseline alignment.
        float       icon_rounding = -1.0f;       ///< Button bg rounding; -1 => keep current style.
    };

    /// \brief Password input with inline "eye" toggle and regex validation.
    ///
    /// The widget shows a password entry field and a compact toggle that switches between
    /// masked/unmasked modes. The current "show password" state is persisted per-field in
    /// ImGuiStorage under the key "##show_pwd".
    ///
    /// \param label        Unique label/ID scope for this composite control.
    /// \param hint         Placeholder text.
    /// \param value        In/out password string.
    /// \param validate     Enables/disables regex validation.
    /// \param policy       When to apply validation (see \ref InputValidatePolicy).
    /// \param pattern      Regex pattern (ECMA) to validate \p value against.
    /// \param out_valid    Out: true if \p value matches \p pattern (or \p validate==false).
    /// \param toggle_cfg   Visual config for the eye toggle.
    /// \param error_color  Tint color applied when invalid.
    /// \param extra_flags  Additional ImGuiInputTextFlags OR-ed with masking flag.
    /// \return true if \p value changed this frame.
    inline bool InputPasswordWithToggle(
            const char* label,
            const char* hint,
            std::string& value,
            bool validate,
            InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            const PasswordToggleConfig& toggle_cfg,
            ImVec4 error_color = ImVec4(0.9f,0.5f,0.5f,1.0f),
            ImGuiInputTextFlags extra_flags = 0
        ) {
        ImGui::PushID(label);

        ImGuiStorage* st = ImGui::GetStateStorage();
        const ImGuiID key_show_pwd = ImGui::GetID(u8"##show_pwd");
        bool show_password_io = st->GetInt(key_show_pwd, 0) != 0;

        ImGuiStyle& style = ImGui::GetStyle();
        const float full_width = ImGui::CalcItemWidth();
        const float frame_h    = ImGui::GetFrameHeight();

        // Compute button size (square by default)
        ImVec2 btn_sz = toggle_cfg.button_size;
        if (btn_sz.x <= 0.0f || btn_sz.y <= 0.0f) {
            btn_sz = ImVec2(frame_h, frame_h);
        }

        // Compute input width (reserve space for button + inner spacing if toggle is enabled)
        const float spacing   = style.ItemInnerSpacing.x;
        const float input_w   = std::max(0.0f, full_width - (btn_sz.x + spacing));

        // Draw validated input
        ImGui::SetNextItemWidth(input_w);
        const ImGuiInputTextFlags pwd_flag = show_password_io ? 0 : ImGuiInputTextFlags_Password;
        const ImGuiInputTextFlags flags    = pwd_flag | extra_flags;

        bool changed = InputTextValidated(
            "##input", hint,
            value, validate, policy, pattern,
            out_valid, error_color, flags
        );

        // Draw toggle button (right to the input)
        {
            // SameLine with optional custom offset
            if (toggle_cfg.same_line_w != 0.0f) ImGui::SameLine(toggle_cfg.same_line_w);
            else                                ImGui::SameLine(0.0f, spacing);

            // Match button colors to input frame colors for a cohesive look
            ImVec4 col_btn       = style.Colors[ImGuiCol_FrameBg];
            ImVec4 col_btn_hover = style.Colors[ImGuiCol_FrameBgHovered];
            ImVec4 col_btn_active= style.Colors[ImGuiCol_FrameBgActive];
            ImGui::PushStyleColor(ImGuiCol_Button,        col_btn);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col_btn_hover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  col_btn_active);

            // Optional rounding override
            const bool has_rounding = (toggle_cfg.icon_rounding >= 0.0f);
            if (has_rounding) ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, toggle_cfg.icon_rounding);

            bool toggled = false;
            if (toggle_cfg.use_icon) {
                // Icon button centered within btn_sz
                IconButtonConfig eye{};
                eye.font        = toggle_cfg.icon_font;
                eye.size        = btn_sz;
                eye.text_offset = ImVec2(0.0f, toggle_cfg.icon_baseline);
                eye.rounding    = toggle_cfg.icon_rounding; // purely informational for downstream; visual via style var above
                // Choose icon according to current visibility
                const char* icon = show_password_io
                    ? (toggle_cfg.icon_hide ? toggle_cfg.icon_hide : u8"X")
                    : (toggle_cfg.icon_show ? toggle_cfg.icon_show : u8"V");
                // Draw
                toggled = IconButtonCentered("##eye", icon, eye);
            } else {
                // Text checkbox fallback
                bool tmp = show_password_io;
                ImGui::SetNextItemWidth(btn_sz.x);
                toggled = ImGui::Checkbox("##eye_chk", &tmp);
                if (toggled) show_password_io = tmp;
                // Optional label to the right (kept minimal to preserve width)
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
    
    /// \brief Password input with eye toggle, regex validation, and on-screen keyboard trigger (overlay).
    ///
    /// Extends \ref InputPasswordWithToggle by adding a compact VK trigger on the same line.
    /// The trigger opens a centered \ref VirtualKeyboardOverlay bound to the same \p value.
    /// Layout reserves space for **two** square buttons (eye + VK) to avoid width jumps.
    /// States persisted in ImGuiStorage:
    ///  - "##show_pwd"   — current eye state (masked/unmasked).
    ///  - "##vk_visible" — current keyboard overlay visibility.
    ///
    /// \note On VK submit/close, this function requests focus back via `SetKeyboardFocusHere(-1)`.
    ///       If your layout is `[Input][Eye][VK]`, consider scheduling focus to the input next
    ///       frame with `SetKeyboardFocusHere(0)` before drawing it, to avoid focusing the VK button.
    ///
    /// \param label        Unique label/ID scope for this composite control.
    /// \param hint         Placeholder text.
    /// \param value        In/out password string (shared with VK overlay).
    /// \param validate     Enables/disables regex validation.
    /// \param policy       When to apply validation (see \ref InputValidatePolicy).
    /// \param pattern      Regex pattern (ECMA) to validate \p value against.
    /// \param out_valid    Out: true if \p value matches \p pattern (or \p validate==false).
    /// \param toggle_cfg   Visual config for the eye toggle.
    /// \param kb_cfg       Visual config for the VK trigger button (icon/text, size, tooltip).
    /// \param vk_cfg       Behavior/visual config for the VK overlay (locales, submit behavior, etc.).
    /// \param error_color  Tint color applied when invalid.
    /// \param extra_flags  Additional ImGuiInputTextFlags OR-ed into the input.
    /// \return true if \p value changed this frame (via input or VK).
    inline bool InputPasswordWithToggleVK(
            const char* label,
            const char* hint,
            std::string& value,
            bool validate,
            InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            const PasswordToggleConfig& toggle_cfg,
            const KeyboardToggleConfig& kb_cfg = {},
            VirtualKeyboardConfig vk_cfg = {},
            ImVec4 error_color = ImVec4(0.9f,0.5f,0.5f,1.0f),
            ImGuiInputTextFlags extra_flags = 0
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

        // Compute button size (square by default)
        ImVec2 btn_sz = toggle_cfg.button_size;
        if (btn_sz.x <= 0.0f || btn_sz.y <= 0.0f) {
            btn_sz = ImVec2(frame_h, frame_h);
        }

        // Compute input width (reserve space for button + inner spacing if toggle is enabled)
        const float spacing   = style.ItemInnerSpacing.x;
        const float input_w   = std::max(0.0f, full_width - 2.0f * (btn_sz.x + spacing));

        // Draw validated input
        ImGui::SetNextItemWidth(input_w);
        const ImGuiInputTextFlags pwd_flag = show_password_io ? 0 : ImGuiInputTextFlags_Password;
        const ImGuiInputTextFlags flags    = pwd_flag | extra_flags;

        bool changed = InputTextValidated(
            "##input", hint,
            value, validate, policy, pattern,
            out_valid, error_color, flags
        );

        // Eye toggle (same as in InputPasswordWithToggle)
        {
            // SameLine with optional custom offset
            if (toggle_cfg.same_line_w != 0.0f) ImGui::SameLine(toggle_cfg.same_line_w);
            else                                ImGui::SameLine(0.0f, spacing);

            // Match button colors to input frame colors for a cohesive look
            ImVec4 col_btn       = style.Colors[ImGuiCol_FrameBg];
            ImVec4 col_btn_hover = style.Colors[ImGuiCol_FrameBgHovered];
            ImVec4 col_btn_active= style.Colors[ImGuiCol_FrameBgActive];
            ImGui::PushStyleColor(ImGuiCol_Button,        col_btn);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col_btn_hover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  col_btn_active);

            // Optional rounding override
            const bool has_rounding = (toggle_cfg.icon_rounding >= 0.0f);
            if (has_rounding) ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, toggle_cfg.icon_rounding);

            bool toggled = false;
            if (toggle_cfg.use_icon) {
                // Icon button centered within btn_sz
                IconButtonConfig eye{};
                eye.font        = toggle_cfg.icon_font;
                eye.size        = btn_sz;
                eye.text_offset = ImVec2(0.0f, toggle_cfg.icon_baseline);
                eye.rounding    = toggle_cfg.icon_rounding; // purely informational for downstream; visual via style var above
                // Choose icon according to current visibility
                const char* icon = show_password_io
                    ? (toggle_cfg.icon_hide ? toggle_cfg.icon_hide : u8"X")
                    : (toggle_cfg.icon_show ? toggle_cfg.icon_show : u8"V");
                // Draw
                toggled = IconButtonCentered("##eye", icon, eye);
            } else {
                // Text checkbox fallback
                bool tmp = show_password_io;
                ImGui::SetNextItemWidth(btn_sz.x);
                toggled = ImGui::Checkbox("##eye_chk", &tmp);
                if (toggled) show_password_io = tmp;
                // Optional label to the right (kept minimal to preserve width)
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
        
        // VK trigger (icon or text).
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

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_VALIDATED_PASSWORD_INPUT_HPP_INCLUDED
