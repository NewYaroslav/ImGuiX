#pragma once
#ifndef _IMGUIX_EXTENSIONS_COLOR_UTILS_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_COLOR_UTILS_HPP_INCLUDED

/// \file color_utils.hpp
/// \brief Utilities for color conversions between ImGui and SFML.

#include <imgui.h>

#if defined(IMGUIX_USE_SFML_BACKEND)
    #include <SFML/Graphics/Color.hpp>
    #include <cstdint>
#endif

namespace ImGuiX::Extensions {

#if defined(IMGUIX_USE_SFML_BACKEND)

    /// \brief Converts an ImVec4 color to sf::Color.
    /// \param color ImVec4 color (each component in [0.0f, 1.0f])
    /// \return sf::Color with 8-bit RGBA values.
    inline sf::Color ColorToSfml(const ImVec4& color) {
    #if defined(SFML_VERSION_MAJOR) && SFML_VERSION_MAJOR >= 3
        return sf::Color(
            static_cast<std::uint8_t>(color.x * 255.0f),
            static_cast<std::uint8_t>(color.y * 255.0f),
            static_cast<std::uint8_t>(color.z * 255.0f),
            static_cast<std::uint8_t>(color.w * 255.0f)
        );
    #else
        return sf::Color(
            static_cast<sf::Uint8>(color.x * 255.0f),
            static_cast<sf::Uint8>(color.y * 255.0f),
            static_cast<sf::Uint8>(color.z * 255.0f),
            static_cast<sf::Uint8>(color.w * 255.0f)
        );
    #endif
    }

#endif // IMGUIX_USE_SFML_BACKEND

} // namespace ImGuiX::Extensions

#endif // _IMGUIX_EXTENSIONS_COLOR_UTILS_HPP_INCLUDED