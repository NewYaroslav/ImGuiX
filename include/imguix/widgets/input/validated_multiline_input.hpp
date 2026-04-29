#pragma once
#ifndef _IMGUIX_WIDGETS_VALIDATED_MULTILINE_INPUT_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VALIDATED_MULTILINE_INPUT_HPP_INCLUDED

/// \file validated_multiline_input.hpp
/// \brief Multiline std::string input with validation, VK toggle and optional help marker.

#include <imgui.h>
#include <imgui_stdlib.h>

#include <imguix/config/colors.hpp>

#include "validated_input.hpp"

namespace ImGuiX::Widgets {

    /// \brief Multiline InputText wrapper with regex validation and error tinting.
    bool InputTextMultilineValidated(
            const char* label,
            std::string& value,
            const ImVec2& size,
            bool validate,
            InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            ImVec4 error_color = IMGUIX_COLOR_ERROR,
            ImGuiInputTextFlags flags = 0,
            ImGuiInputTextCallback callback = nullptr,
            void* user_data = nullptr
        );

    /// \brief Multiline InputText wrapper with validation, VK trigger and optional help marker.
    bool InputTextMultilineWithVKValidated(
            const char* label,
            std::string& value,
            const ImVec2& size,
            bool validate,
            InputValidatePolicy policy,
            const std::string& pattern,
            bool& out_valid,
            ImVec4 error_color = IMGUIX_COLOR_ERROR,
            ImGuiInputTextFlags extra_flags = 0,
            ImGuiInputTextCallback callback = nullptr,
            void* user_data = nullptr,
            const KeyboardToggleConfig& kb_cfg = {},
            VirtualKeyboardConfig vk_cfg = {},
            bool show_help = false,
            const char* help_text = nullptr
        );

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "validated_multiline_input.ipp"
#endif

#endif // _IMGUIX_WIDGETS_VALIDATED_MULTILINE_INPUT_HPP_INCLUDED
