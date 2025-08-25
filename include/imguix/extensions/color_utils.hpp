#pragma once
#ifndef _IMGUIX_EXTENSIONS_COLOR_UTILS_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_COLOR_UTILS_HPP_INCLUDED

/// \file color_utils.hpp
/// \brief Utilities for color conversions between ImGui and SFML.

#include <imgui.h>

#if defined(IMGUIX_USE_SFML_BACKEND)
    #include <SFML/Graphics/Color.hpp>
#endif

namespace ImGuiX::Extensions {

#if defined(IMGUIX_USE_SFML_BACKEND)

    /// \brief Convert ImVec4 color to sf::Color.
    /// \param color Color with components in [0,1].
    /// \return 8-bit RGBA color.
    inline sf::Color ColorToSfml(const ImVec4& color);

#endif // IMGUIX_USE_SFML_BACKEND

} // namespace ImGuiX::Extensions

#ifdef IMGUIX_HEADER_ONLY
#   include "color_utils.ipp"
#endif

#endif // _IMGUIX_EXTENSIONS_COLOR_UTILS_HPP_INCLUDED
