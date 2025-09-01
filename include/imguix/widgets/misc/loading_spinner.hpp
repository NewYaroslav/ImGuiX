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
        float radius        = 24.0f;     ///< Circle radius in pixels.
        float thickness     = 6.5f;      ///< Stroke thickness in pixels.
        int   segments      = 20;        ///< Segment count for arc (>= 8 recommended).
        float angular_speed = 4.0f;      ///< Radians per second added to the phase.
        float sweep_ratio   = 0.80f;     ///< Arc sweep as fraction of full circle (0..1], e.g. 0.8 → 288°.
        ImU32 color         = 0;         ///< 0 → use style's ImGuiCol_Text color.
        float extra_top_padding = 0.0f;  ///< Optional extra top padding in pixels.
    };

    /// \brief Compute default spinner color from style.
    /// \return Spinner color based on current style.
    ImU32 DefaultSpinnerColor();

    /// \brief Draw an animated loading spinner.
    /// \param id Unique widget identifier.
    /// \param cfg Spinner parameters.
    /// \return True if the item was submitted (not clipped).
    /// \code{.cpp}
    /// LoadingSpinner("busy");
    /// \endcode
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

#   ifdef IMGUIX_DEMO
    /// \brief Render demo for LoadingSpinner widget.
    /// \param cfg Spinner configuration.
    inline void DemoLoadingSpinner(LoadingSpinnerConfig& cfg) {
        LoadingSpinner("spinner", cfg);
        ImGui::SliderFloat("radius",    &cfg.radius,        2.0f, 48.0f, "%.0f");
        ImGui::SliderFloat("thickness", &cfg.thickness,     1.0f, 12.0f, "%.1f");
        ImGui::SliderInt  ("segments",  &cfg.segments,         8, 128);
        ImGui::SliderFloat("sweep",     &cfg.sweep_ratio,   0.1f, 1.0f, "%.2f");
        ImGui::SliderFloat("speed",     &cfg.angular_speed, 0.0f, 20.0f, "%.1f");
    }
#   endif

} // namespace ImGuiX::Widgets
#ifdef IMGUIX_HEADER_ONLY
#   include "loading_spinner.ipp"
#endif

#endif // _IMGUIX_WIDGETS_LOADING_SPINNER_HPP_INCLUDED
