#pragma once
#ifndef _IMGUIX_EXTENSIONS_COLOR_GRADIENT_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_COLOR_GRADIENT_HPP_INCLUDED

/// \file color_gradient.hpp
/// \brief Generates sequential colors from HSV spectrum.

#include <cstddef>
#include <imgui.h>

namespace ImGuiX::Extensions {

    /// \brief HSV color components.
    struct Hsv {
        /// \brief Hue angle in degrees.
        double h;
        /// \brief Saturation as fraction [0,1].
        double s;
        /// \brief Value as fraction [0,1].
        double v;
    };

    /// \brief Convert HSV color to RGBA vector.
    /// \param in HSV color.
    /// \return Corresponding RGBA color with alpha = 1.
    ImVec4 hsvToRgb(const Hsv& in);

    /// \brief Generates sequential colors from HSV spectrum.
    class ColorGradient {
    public:
        /// \brief Construct gradient for given number of colors.
        /// \param count Number of colors to cycle through.
        explicit ColorGradient(std::size_t count);

        /// \brief Retrieve next color from gradient.
        /// \return RGBA color with alpha = 1.
        ImVec4 next();

    private:
        std::size_t m_next_index = 0;
        std::size_t m_offset = 0;
        std::size_t m_max_item = 0;
    };

} // namespace ImGuiX::Extensions

#ifdef IMGUIX_HEADER_ONLY
#   include "color_gradient.ipp"
#endif

#endif // _IMGUIX_EXTENSIONS_COLOR_GRADIENT_HPP_INCLUDED
