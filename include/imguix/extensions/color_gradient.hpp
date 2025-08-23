#pragma once
#ifndef _IMGUIX_EXTENSIONS_COLOR_GRADIENT_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_COLOR_GRADIENT_HPP_INCLUDED

/// \file color_gradient.hpp
/// \brief Generates sequential colors from HSV spectrum.

#include <algorithm>
#include <cstddef>

#include <imgui.h>

namespace ImGuiX::Extensions {

    struct Hsv {
        double h; /// angle in degrees
        double s; /// fraction between 0 and 1
        double v; /// fraction between 0 and 1
    };

    inline ImVec4 hsvToRgb(const Hsv& in) {
        double hh = in.h;
        if (hh >= 360.0)
            hh = 0.0;
        hh /= 60.0;
        long i = static_cast<long>(hh);
        double ff = hh - i;
        double p = in.v * (1.0 - in.s);
        double q = in.v * (1.0 - (in.s * ff));
        double t = in.v * (1.0 - (in.s * (1.0 - ff)));
        ImVec4 out;
        out.w = 1.0f;
        switch (i) {
        case 0:
            out.x = in.v;
            out.y = t;
            out.z = p;
            break;
        case 1:
            out.x = q;
            out.y = in.v;
            out.z = p;
            break;
        case 2:
            out.x = p;
            out.y = in.v;
            out.z = t;
            break;
        case 3:
            out.x = p;
            out.y = q;
            out.z = in.v;
            break;
        case 4:
            out.x = t;
            out.y = p;
            out.z = in.v;
            break;
        default:
            out.x = in.v;
            out.y = p;
            out.z = q;
            break;
        }
        return out;
    }

    class ColorGradient {
    public:
        explicit ColorGradient(std::size_t count)
            : m_offset(count / 2 + 2 * count / 3),
              m_max_item(count) {}

        /// \brief Retrieve next color from gradient.
        /// \return RGBA color with alpha = 1.
        ImVec4 next() {
            Hsv col;
            col.h = static_cast<double>((m_next_index + m_offset) % m_max_item)
                    / static_cast<double>(m_max_item) * 360.0;
            m_next_index += std::max<std::size_t>(1, m_max_item / 12);
            col.s = static_cast<double>((m_next_index * 2) % m_max_item)
                    / static_cast<double>(m_max_item);
            col.v = 0.9;
            return hsvToRgb(col);
        }

    private:
        std::size_t m_next_index = 0;
        std::size_t m_offset = 0;
        std::size_t m_max_item = 0;
    };

} // namespace ImGuiX::Extensions

#endif // _IMGUIX_EXTENSIONS_COLOR_GRADIENT_HPP_INCLUDED
