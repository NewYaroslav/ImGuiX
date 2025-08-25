#pragma once
#ifndef _IMGUIX_EXTENSIONS_COLOR_GRADIENT_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_COLOR_GRADIENT_HPP_INCLUDED

/// \file color_gradient.hpp
/// \brief Generates sequential colors from HSV spectrum.

#include <cstddef>

#include <imgui.h>

namespace ImGuiX::Extensions {

    struct Hsv {
        double h; /// angle in degrees
        double s; /// fraction between 0 and 1
        double v; /// fraction between 0 and 1
    };

    ImVec4 hsvToRgb(const Hsv& in);

class ColorGradient {
    public:
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
