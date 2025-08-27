#pragma once
#ifndef _IMGUIX_WIDGETS_ARROW_STEPPER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_ARROW_STEPPER_HPP_INCLUDED

/// \file arrow_stepper.hpp
/// \brief Compact numeric stepper with Up/Down arrow buttons and optional wrap.

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Config for ArrowStepper (integer).
    struct ArrowStepperConfig {
        int   min_value   = 0;      ///< Inclusive
        int   max_value   = 59;     ///< Inclusive
        int   step        = 1;      ///< Increment/decrement step
        bool  wrap        = true;   ///< If exceed bounds, wrap to the other side
        float input_width = 48.0f;  ///< Width of InputInt
        const char* fmt   = u8"%02d"; ///< Small preview text to the right
        const char* input_id = u8"##val"; ///< ID for InputInt inside local PushID
        float same_line_spacing = 2.0f; ///< Spacing between items on SameLine
        bool  disable_at_edges = true; ///< Disable buttons when value is at min/max
    };

    /// \brief Arrow stepper for an integer value.
    /// \param id Unique widget identifier.
    /// \param v Reference to value being edited.
    /// \param cfg Configuration parameters.
    /// \param last_delta Optional output: last delta applied by arrows or wheel.
    /// \return True if value changed.
    bool ArrowStepper(
            const char* id,
            int& v,
            const ArrowStepperConfig& cfg = {},
            int* last_delta = nullptr
        );

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "arrow_stepper.ipp"
#endif

#endif // _IMGUIX_WIDGETS_ARROW_STEPPER_HPP_INCLUDED
