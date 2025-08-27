#pragma once
#ifndef _IMGUIX_WIDGETS_VERTICAL_STEPPER_BUTTONS_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VERTICAL_STEPPER_BUTTONS_HPP_INCLUDED

/// \file vertical_stepper_buttons.hpp
/// \brief Stacked [+]/[−] button column (manual drawing) for use beside inputs.
///        Height defaults to ImGui::GetFrameHeight() when size.y <= 0.

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \param id        Unique ID (scoped by caller).
    /// \param size      Column size (width, height). If size.{x|y} <= 0 -> uses ImGui::GetFrameHeight().
    /// \param step      Increment per click/hold.
    /// \param step_fast Increment when Ctrl held.
    /// \param gap_y     Vertical gap between [+] and [−]; default 1.0f. Pass <0 to use style.ItemInnerSpacing.y.
    /// \return Signed delta this frame (>0 '+', <0 '−').
    int VerticalStepperButtons(
            const char* id,
            ImVec2 size,
            int step,
            int step_fast,
            float gap_y = 1.0f
        );

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "vertical_stepper_buttons.ipp"
#endif

#endif // _IMGUIX_WIDGETS_VERTICAL_STEPPER_BUTTONS_HPP_INCLUDED
