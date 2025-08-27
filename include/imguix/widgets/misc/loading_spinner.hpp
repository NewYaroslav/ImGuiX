#pragma once
#ifndef _IMGUIX_WIDGETS_LOADING_SPINNER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_LOADING_SPINNER_HPP_INCLUDED

/// \file loading_spinner.hpp
/// \brief Animated arc spinner (loading indicator) for ImGuiX.
///
/// Original idea: https://github.com/ocornut/imgui/issues/1901 (adapted).

#include <imgui.h>

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
    bool LoadingSpinner(const char* id, const LoadingSpinnerConfig& cfg = {});

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
#ifdef IMGUIX_HEADER_ONLY
#   include "loading_spinner.ipp"
#endif

#endif // _IMGUIX_WIDGETS_LOADING_SPINNER_HPP_INCLUDED
