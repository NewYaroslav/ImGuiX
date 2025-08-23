#pragma once
#ifndef _IMGUIX_WIDGETS_LOADING_SPINNER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_LOADING_SPINNER_HPP_INCLUDED

/// \file loading_spinner.hpp
/// \brief Animated arc spinner (loading indicator) for ImGuiX.
///
/// Original idea: https://github.com/ocornut/imgui/issues/1901 (adapted).

#include <imgui.h>
#include <algorithm>
#include <cmath>

namespace ImGuiX::Widgets {

    /// \brief Configuration for LoadingSpinner.
    struct LoadingSpinnerConfig {
        float radius        = 24.0f;     ///< circle radius in pixels
        float thickness     = 6.5f;      ///< stroke thickness in pixels
        int   segments      = 20;        ///< number of segments used to draw the arc (>= 8 recommended)
        float angular_speed = 4.0f;      ///< radians per second added to the phase
        float sweep_ratio   = 0.80f;     ///< arc sweep as fraction of full circle (0..1], e.g. 0.8 → 288°
        ImU32 color         = 0;         ///< 0 → use style's ImGuiCol_Text color
        float extra_top_padding = 0.0f;  ///< optional extra vertical padding on top, px
    };

    /// \brief Draw an animated loading spinner.
    /// \param id Unique widget identifier.
    /// \param cfg Spinner parameters.
    /// \return True if the item was submitted (not clipped).
    inline bool LoadingSpinner(const char* id, const LoadingSpinnerConfig& cfg = {}) {
        // Validate / clamp
        const float radius    = std::max(0.0f, cfg.radius);
        const float thickness = std::max(1.0f, cfg.thickness);
        const int   segments  = std::max(8, cfg.segments);
        const float sweep_rad = std::clamp(cfg.sweep_ratio, 0.05f, 1.0f) * 6.28318530717958647692f; // TAU
        const float phase     = static_cast<float>(ImGui::GetTime()) * std::max(0.0f, cfg.angular_speed);

        // Reserve item rect using public API
        // Make item square (2R x 2R) + optional extra top padding (keeps spacing similar to old code)
        const ImVec2 item_size(2.0f * radius, 2.0f * radius + cfg.extra_top_padding);
        if (!ImGui::InvisibleButton(id, item_size))
        {
            // Even if not clicked/hovered, the item exists and we can draw (unless clipped).
            // If it was clipped, drawing won't be visible anyway. Continue to draw for consistency.
        }

        // Compute geometry from item rect
        const ImVec2 pos_min = ImGui::GetItemRectMin();
        const ImVec2 center(pos_min.x + radius, pos_min.y + cfg.extra_top_padding * 0.5f + radius);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImU32 col = cfg.color ? cfg.color : ImGui::GetColorU32(ImGuiCol_Text);

        const float a_min = phase;
        const float a_max = phase + sweep_rad;

        dl->PathClear();
        dl->PathArcTo(center, radius, a_min, a_max, segments);
        dl->PathStroke(col, 0 /*flags*/, thickness);
        return true;
    }

    /// \brief Convenience overload with basic parameters.
    /// \param id Unique widget identifier.
    /// \param radius Spinner radius in pixels.
    /// \param thickness Stroke thickness in pixels.
    /// \param color Color in ImU32 format; 0 uses style text color.
    /// \return True if the item was submitted.
    inline bool LoadingSpinner(const char* id, float radius, float thickness, ImU32 color = 0) {
        LoadingSpinnerConfig cfg;
        cfg.radius = radius;
        cfg.thickness = thickness;
        cfg.color = color;
        return LoadingSpinner(id, cfg);
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_LOADING_SPINNER_HPP_INCLUDED
