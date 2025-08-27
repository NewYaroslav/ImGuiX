#pragma once
#ifndef _IMGUIX_WIDGETS_INPUT_VERTICAL_STEPPER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_INPUT_VERTICAL_STEPPER_HPP_INCLUDED

/// \file input_vertical_stepper.hpp
/// \brief InputInt with a vertical stepper column on the right: [+] on top, [−] below.
///        Designed for dear imgui 1.89+ (public API).
///
/// Minimal usage:
/// ```cpp
/// int value = 42;
/// if (ImGuiX::Widgets::InputIntVerticalStepper("Amount", &value, 1, 10)) {
///     // value changed
/// }
/// ```
/// Notes:
/// - Buttons auto-repeat while held (PushButtonRepeat).
/// - Holding Ctrl applies `step_fast`.
/// - Width is computed so the buttons fit to the right of the input on one line.
/// - Optional min/max clamping.

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief InputInt with vertical +/− buttons to the right ( + on top, − below ).
    /// \param label        Visible label (and ID scope). Must be unique in the current scope.
    /// \param v            In/out integer value.
    /// \param step         Increment/decrement applied by the buttons or keyboard arrows. Default: 1.
    /// \param step_fast    Increment/decrement when Ctrl is held. Default: 100.
    /// \param v_min        Optional lower bound (inclusive). Use INT_MIN to disable.
    /// \param v_max        Optional upper bound (inclusive). Use INT_MAX to disable.
    /// \param flags        Extra ImGuiInputTextFlags for InputInt (step inside the input is disabled internally).
    /// \return true if value changed (via typing or buttons).
    bool InputIntVerticalStepper(
            const char* label,
            int* v,
            int step = 1,
            int step_fast = 100,
            int v_min = INT_MIN,
            int v_max = INT_MAX,
            ImGuiInputTextFlags flags = 0
        );

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "input_vertical_stepper.ipp"
#endif

#endif // _IMGUIX_WIDGETS_INPUT_VERTICAL_STEPPER_HPP_INCLUDED
