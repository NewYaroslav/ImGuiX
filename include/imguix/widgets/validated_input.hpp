#pragma once
#ifndef _IMGUIX_WIDGETS_VALIDATED_INPUT_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VALIDATED_INPUT_HPP_INCLUDED

/// \file validated_input.hpp
/// \brief InputTextWithHint for std::string with regex validation and error tinting.

#include <imgui.h>
#include <imgui_stdlib.h>
#include <string>
#include <regex>
#include <imguix/extensions/validation.hpp>

namespace ImGuiX::Widgets {

    /// \brief Policy controlling when validation is applied in InputTextValidated.
    enum class InputValidatePolicy {
        OnDraw,           ///< Validate and tint on every frame (default).
        OnChange,         ///< Validate only after user input; initial value not tinted.
        OnChangeNonEmpty, ///< Validate only after input, but skip empty string (always valid).
        OnTouch           ///< Validate after first user edit; before that, initial value always valid.
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
        ImVec4 error_color = ImVec4(0.9f,0.5f,0.5f,1.0f),
        ImGuiInputTextFlags flags = 0,
        ImGuiInputTextCallback callback = nullptr,
        void* user_data = nullptr
    ) {
        using ImGuiX::Extensions::RegexMatchSafe;
        using ImGuiX::Extensions::ScopedInvalid;

        auto match = [&](const std::string& s){ return !validate || RegexMatchSafe(s, pattern); };

        bool pre_tint = false;
        switch (policy) {
            case InputValidatePolicy::OnDraw:           pre_tint = true; break;
            case InputValidatePolicy::OnChange:         pre_tint = false; break;
            case InputValidatePolicy::OnChangeNonEmpty: pre_tint = !value.empty(); break;
            case InputValidatePolicy::OnTouch: {
                ImGuiStorage* st = ImGui::GetStateStorage();
                ImGuiID id = ImGui::GetID(label);
                pre_tint = st->GetBool(id, false);
            } break;
        }

        bool valid_now = match(value);
        ScopedInvalid tint(pre_tint && !valid_now, error_color);

        bool changed = ImGui::InputTextWithHint(label, hint, &value, flags, callback, user_data);

        if (changed) {
            valid_now = match(value);
            if (policy == InputValidatePolicy::OnTouch) {
                ImGui::GetStateStorage()->SetBool(ImGui::GetID(label), true);
            }
        }

        out_valid = valid_now;
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_VALIDATED_INPUT_HPP_INCLUDED
