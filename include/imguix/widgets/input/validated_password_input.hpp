#pragma once
#ifndef _IMGUIX_WIDGETS_VALIDATED_PASSWORD_INPUT_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VALIDATED_PASSWORD_INPUT_HPP_INCLUDED

/// \file validated_password_input.hpp
/// \brief Password input with regex validation, inline "eye" toggle, and optional VK trigger.

#include <imgui.h>
#include <string>

#include <imguix/config/icons.hpp>
#include <imguix/config/colors.hpp>
#include "validated_input.hpp" // InputTextValidated, InputValidatePolicy, KeyboardToggleConf

namespace ImGuiX::Widgets {

    /// \brief Configuration for the inline "eye" toggle button.
    ///
    /// Controls only the visibility toggle UI that switches between masked/unmasked input.
    /// The actual password masking is performed via ImGuiInputTextFlags_Password when hidden.
    /// Rounding is applied through style var override if \ref icon_rounding >= 0.
    struct PasswordToggleConfig {
        bool        use_icon      = true;        ///< true: icon button; false: checkbox + optional text label.
        const char* icon_show     = IMGUIX_ICON_EYE_SHOW;  ///< Glyph for "show" (depends on your icon font).
        const char* icon_hide     = IMGUIX_ICON_EYE_HIDE;  ///< Glyph for "hide".
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
    bool InputPasswordWithToggle(
            const char* label,
            const char* hint,
            std::string& value,
            bool validate,
            InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            const PasswordToggleConfig& toggle_cfg,
            ImVec4 error_color = IMGUIX_COLOR_ERROR,
            ImGuiInputTextFlags extra_flags = 0
        );
    
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
bool InputPasswordWithToggleVK(
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
            ImVec4 error_color = IMGUIX_COLOR_ERROR,
            ImGuiInputTextFlags extra_flags = 0
        );

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "validated_password_input.ipp"
#endif

#endif // _IMGUIX_WIDGETS_VALIDATED_PASSWORD_INPUT_HPP_INCLUDED
