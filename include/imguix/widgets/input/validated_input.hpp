#pragma once
#ifndef _IMGUIX_WIDGETS_VALIDATED_INPUT_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VALIDATED_INPUT_HPP_INCLUDED

/// \file validated_input.hpp
/// \brief InputTextWithHint for std::string with regex validation and error tinting.

#include <imgui_stdlib.h>

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
    bool InputTextValidated(
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
        );
    
    /// \brief InputTextWithHint wrapper with validation and optional VK trigger.
    /// \param label       Unique label/ID scope for the pair (input + button).
    /// \param hint        Placeholder text for the input.
    /// \param value       Bound std::string edited by both the input and the VK overlay.
    /// \param validate    Enables/disables validation.
    /// \param policy      When to apply validation (see \ref InputValidatePolicy).
    /// \param pattern     Regex pattern (ECMA) for validation.
    /// \param out_valid   Out: true if \p value matches \p pattern (or validation disabled).
    /// \param error_color Tint applied while invalid (frame/text).
    /// \param extra_flags Extra ImGuiInputTextFlags OR-ed into the input (optional).
    /// \param callback    Optional ImGui input callback for the input field.
    /// \param user_data   Optional user data for the input callback.
    /// \param kb_cfg      Visual config for the VK trigger button.
    /// \param vk_cfg      Behavior/visual config for the on-screen keyboard overlay.
    /// \return true if the value changed this frame (via input or VK).
    bool InputTextWithVKValidated(
            const char* label,
            const char* hint,
            std::string& value,
            bool validate,
            InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            ImVec4 error_color = IMGUIX_COLOR_ERROR,
            ImGuiInputTextFlags extra_flags = 0,
            ImGuiInputTextCallback callback = nullptr,
            void* user_data = nullptr,
            const KeyboardToggleConfig& kb_cfg = {},
            VirtualKeyboardConfig vk_cfg = {}
        );

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

#ifdef IMGUIX_HEADER_ONLY
#   include "validated_input.ipp"
#endif

#endif // _IMGUIX_WIDGETS_VALIDATED_INPUT_HPP_INCLUDED
