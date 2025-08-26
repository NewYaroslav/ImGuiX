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
#include <algorithm>
#include <cmath>
#include <climits>
#include "vertical_stepper_buttons.hpp"

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
    inline bool InputIntVerticalStepper(
			const char* label,
			int* v,
			int step = 1,
			int step_fast = 100,
			int v_min = INT_MIN,
			int v_max = INT_MAX,
			ImGuiInputTextFlags flags = 0
		) {
        bool changed = false;
        ImGui::PushID(label);

        ImGuiStyle& style   = ImGui::GetStyle();
        const float frame_h = ImGui::GetFrameHeight();
        const float full_w  = ImGui::CalcItemWidth();

        const float btn_w = frame_h;                         // square column
        const float input_w = std::max(0.0f, full_w - (btn_w + style.ItemInnerSpacing.x));

        // input (disable built-in steppers)
        ImGui::SetNextItemWidth(input_w);
        changed |= ImGui::InputInt("##in", v, 0, 0, flags);

        // stepper column as a grouped widget; must SameLine before drawing the group
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        const int delta = VerticalStepperButtons("##vstep", ImVec2(btn_w, frame_h), step, step_fast);

        if (delta != 0) {
            long long nv = static_cast<long long>(*v) + delta; // avoid UB on overflow
            nv = std::max<long long>(v_min, std::min<long long>(v_max, nv));
            if (nv != *v) { *v = static_cast<int>(nv); changed = true; }
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

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_INPUT_VERTICAL_STEPPER_HPP_INCLUDED
