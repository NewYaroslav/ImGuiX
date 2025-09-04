#pragma once
#ifndef _IMGUIX_WIDGETS_VALIDATED_PASSWORD_INPUT_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VALIDATED_PASSWORD_INPUT_HPP_INCLUDED

// Header-only thin wrappers over validated_input.hpp to provide
// password inputs with an integrated "eye" (show/hide) and optional VK.
//
// The actual eye button is drawn INSIDE the input frame by InputTextValidated /
// InputTextWithVKValidated (through InputTextWithIconToggle). These wrappers only
// forward configuration and set ImGuiInputTextFlags_Password.

#include <imgui.h>
#include <imgui_stdlib.h>
#include <string>
#include <utility>

#include <imguix/config/icons.hpp>   // IMGUIX_ICON_EYE_SHOW / _HIDE defaults
#include <imguix/config/colors.hpp>
#include "validated_input.hpp"       // InputTextValidated / InputTextWithVKValidated, KeyboardToggleConfig

namespace ImGuiX::Widgets {

    /// Configuration for password "eye" glyphs.
    /// Only `icon_show`, `icon_hide`, and `icon_font` are used by these wrappers.
    struct PasswordToggleConfig {
        // Glyphs (PUA, e.g. Material Symbols). If nullptr, defaults from icons.hpp are used.
        const char* icon_show = IMGUIX_ICON_EYE_SHOW;  ///< Eye-open glyph (visible state).
        const char* icon_hide = IMGUIX_ICON_EYE_HIDE;  ///< Eye-closed glyph (hidden/password state).
        ImFont*     icon_font = nullptr;               ///< Icon font (nullptr => current font).

        // Note: legacy visual knobs (size/rounding/baseline) are intentionally omitted here,
        // because the eye is integrated into the input frame and follows the current style.
    };

    /// Password input with integrated eye toggle and regex validation.
    /// Wrapper around InputTextValidated, enabling Password flag and forwarding eye config.
    ///
    /// \param label       Unique label/ID scope.
    /// \param hint        Placeholder.
    /// \param value       Bound std::string.
    /// \param validate    Enable/disable regex check.
    /// \param policy      When to apply validation.
    /// \param pattern     ECMA regex.
    /// \param out_valid   Out: validation result.
    /// \param toggle_cfg  Eye icon/font config (only glyphs+font used).
    /// \param error_color Tint while invalid.
    /// \param extra_flags Extra ImGuiInputTextFlags (will be OR-ed with Password).
    /// \return true if value changed this frame.
    inline bool InputPasswordWithToggle(
            const char* label,
            const char* hint,
            std::string& value,
            bool validate,
            InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            const PasswordToggleConfig& toggle_cfg = {},
            ImVec4 error_color = IMGUIX_COLOR_ERROR,
            ImGuiInputTextFlags extra_flags = 0
    ) {
        const ImGuiInputTextFlags flags = extra_flags | ImGuiInputTextFlags_Password;

        // Forward to InputTextValidated; eye is rendered internally inside the input.
        return InputTextValidated(
            label,
            hint,
            value,
            validate,
            policy,
            pattern,
            out_valid,
            error_color,
            flags,
            /*callback*/ nullptr,
            /*user_data*/ nullptr,
            /*eye_on*/  toggle_cfg.icon_show,
            /*eye_off*/ toggle_cfg.icon_hide,
            /*eye_icon_font*/ toggle_cfg.icon_font,
            /*show_trailing_label*/ true
        );
    }

    /// Password input with integrated eye toggle + on-screen keyboard (overlay) trigger.
    /// Wrapper around InputTextWithVKValidated, enabling Password flag and forwarding eye config
    /// via KeyboardToggleConfig (its `icon_eye_on/off` and `icon_font` fields are used inside).
    ///
    /// \param kb_cfg  Visual config for the VK trigger button (icon/text, size, tooltip, etc.).
    ///                Its `icon_eye_on/icon_eye_off/icon_font` will be overridden from toggle_cfg.
    /// Other parameters — same as \ref InputPasswordWithToggle.
    inline bool InputPasswordWithToggleVK(
            const char* label,
            const char* hint,
            std::string& value,
            bool validate,
            InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            const PasswordToggleConfig& toggle_cfg = {},
            const KeyboardToggleConfig& kb_cfg = {},
            VirtualKeyboardConfig vk_cfg = {},
            ImVec4 error_color = IMGUIX_COLOR_ERROR,
            ImGuiInputTextFlags extra_flags = 0
    ) {
        const ImGuiInputTextFlags flags = extra_flags | ImGuiInputTextFlags_Password;

        // Prepare a local KeyboardToggleConfig that carries the eye glyphs/font
        // for the integrated eye inside the input.
        KeyboardToggleConfig kb = kb_cfg;
        kb.icon_eye_on  = toggle_cfg.icon_show ? toggle_cfg.icon_show : IMGUIX_ICON_EYE_SHOW;
        kb.icon_eye_off = toggle_cfg.icon_hide ? toggle_cfg.icon_hide : IMGUIX_ICON_EYE_HIDE;
        if (toggle_cfg.icon_font) kb.icon_font = toggle_cfg.icon_font;

        return InputTextWithVKValidated(
            label,
            hint,
            value,
            validate,
            policy,
            pattern,
            out_valid,
            error_color,
            flags,
            /*callback*/ nullptr,
            /*user_data*/ nullptr,
            kb,
            std::move(vk_cfg)
        );
    }

#   ifdef IMGUIX_DEMO
    /// Minimal demo: two fields, one plain validated, one password with VK.
    inline void DemoValidatedPasswordInputs() {
        static std::string username;
        static std::string password;
        bool user_ok = false, pass_ok = false;

        KeyboardToggleConfig kbc{};   // VK button visuals
        PasswordToggleConfig tgc{};   // eye glyphs/font

        // e-mail-like username validation
        InputTextWithVKValidated(
            "user##demo",
            "e-mail",
            username,
            true,
            InputValidatePolicy::OnTouch,
            R"(.+@.+\.\w+)",
            user_ok,
            kbc
        );

        // password: at least 8 chars, alnum/_/-
        InputPasswordWithToggleVK(
            "pass##demo",
            "password",
            password,
            true,
            InputValidatePolicy::OnTouch,
            R"(^[\w\-]{8,}$)",
            pass_ok,
            tgc,
            kbc
        );
    }
#   endif

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_VALIDATED_PASSWORD_INPUT_HPP_INCLUDED

