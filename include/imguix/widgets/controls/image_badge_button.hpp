#pragma once
#ifndef _IMGUIX_WIDGETS_IMAGE_BADGE_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_IMAGE_BADGE_BUTTON_HPP_INCLUDED

#ifdef IMGUIX_USE_SFML_BACKEND

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <imgui.h>

/// \file image_badge_button.hpp
/// \brief Texture button widget with compact corner chips.

namespace ImGuiX::Widgets {

    /// \brief Configuration for ImageBadgeButton.
    struct ImageBadgeButtonConfig {
        ImVec2 size{0.0f, 0.0f};               ///< Outer button size; `{0,0}` means square `GetFrameHeight()`.
        float outer_rounding{4.2f};            ///< Rounding radius of the outer shell.
        float inner_rounding{4.0f};            ///< Rounding radius of the inner card.
        ImVec2 inner_margin{2.0f, 2.0f};       ///< Gap between the outer shell and inner card.
        ImVec2 image_padding{3.0f, 3.0f};      ///< Padding around the image area inside the card body.
        bool draw_outer_border{true};          ///< Draw border around the outer shell.
        float outer_border_thickness{1.0f};    ///< Border thickness in pixels.
        ImU32 outer_border_col{0};             ///< Border color, `0` uses widget default.
        ImU32 outer_bg_col{0};                 ///< Outer shell background color, `0` uses widget default.
        ImU32 inner_bg_col{0};                 ///< Inner card idle background color, `0` uses widget default.
        ImU32 inner_hover_bg_col{0};           ///< Inner card hover background color, `0` uses widget default.
        ImU32 inner_active_bg_col{0};          ///< Inner card pressed background color, `0` uses widget default.
        const char* footer_text{nullptr};      ///< Optional footer text rendered in the bottom-left chip.
        ImFont* footer_font{nullptr};          ///< Footer font; `nullptr` uses current font.
        ImU32 footer_bg_col{0};                ///< Footer background color, `0` uses widget default.
        ImU32 footer_text_col{0};              ///< Footer text color, `0` uses widget default.
        ImVec2 footer_text_padding{3.0f, 1.0f}; ///< Padding around footer text inside bottom-left chip.
        float footer_min_height{9.0f};         ///< Minimum footer chip height.
        float footer_corner_rounding{4.0f};    ///< Footer chip corner rounding.
        bool show_status_chip{false};          ///< Draw a status chip in the top-right corner.
        ImU32 status_chip_col{0};              ///< Status chip color, `0` uses widget default.
        //ImVec2 status_chip_size{6.0f, 6.0f};   ///< Status chip size in pixels.
        //float status_chip_corner_rounding{3.0f}; ///< Status chip corner rounding.
        sf::Color tint_color{sf::Color::White};    ///< Image tint.
    };

    /// \brief Clickable texture button with optional badge overlay.
    /// \param id Unique widget identifier.
    /// \param texture SFML texture rendered inside the button.
    /// \param cfg Button appearance configuration.
    /// \return True if clicked with left mouse button.
    bool ImageBadgeButton(
        const char* id,
        const sf::Texture& texture,
        const ImageBadgeButtonConfig& cfg = {});

} // namespace ImGuiX::Widgets

#endif // IMGUIX_USE_SFML_BACKEND

#ifdef IMGUIX_HEADER_ONLY
#   include "image_badge_button.ipp"
#endif

#endif // _IMGUIX_WIDGETS_IMAGE_BADGE_BUTTON_HPP_INCLUDED
