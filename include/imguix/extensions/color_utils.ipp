namespace ImGuiX::Extensions {

#if defined(IMGUIX_USE_SFML_BACKEND)

    inline sf::Color ColorToSfml(const ImVec4& color) {
#       if defined(SFML_VERSION_MAJOR) && SFML_VERSION_MAJOR >= 3
        return sf::Color(
            static_cast<std::uint8_t>(color.x * 255.0f),
            static_cast<std::uint8_t>(color.y * 255.0f),
            static_cast<std::uint8_t>(color.z * 255.0f),
            static_cast<std::uint8_t>(color.w * 255.0f)
        );
#       else
        return sf::Color(
            static_cast<sf::Uint8>(color.x * 255.0f),
            static_cast<sf::Uint8>(color.y * 255.0f),
            static_cast<sf::Uint8>(color.z * 255.0f),
            static_cast<sf::Uint8>(color.w * 255.0f)
        );
#       endif
    }

#endif // IMGUIX_USE_SFML_BACKEND

} // namespace ImGuiX::Extensions

