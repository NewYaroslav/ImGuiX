#pragma once
#ifndef _IMGUIX_WIDGETS_ICON_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_ICON_BUTTON_HPP_INCLUDED

#include <imgui.h>

/// \file icon_button.hpp
/// \brief Centered icon or text button widget.

namespace ImGuiX::Widgets {

    /// \brief Configuration for IconButtonCentered.
    struct IconButtonConfig {
        ImVec2 size{0, 0};            ///< (0,0) => square with height = GetFrameHeight()
        float rounding{-1.0f};       ///< Background rounding, -1 to use style
        ImFont* font{nullptr};       ///< Font for text/icons; null uses current
        ImVec2 text_offset{0, 0};    ///< Baseline adjustment for text
        bool draw_border{false};     ///< Draw border around button
        ImU32 border_col{0};         ///< Border color, 0 => ImGuiCol_Border
        float border_thickness{1.0f}; ///< Border thickness in pixels
        bool appear_on_hover{false}; ///< No bg/border until hover/active/focus
        bool show_hover_border{true}; ///< Draw border only when visible
    };

    /// \brief Button with centered text or icon.
    /// \param id Unique widget identifier.
    /// \param text UTF-8 text or icon string.
    /// \param cfg Button appearance options.
    /// \return True if clicked with left mouse button.
    /// \code{.cpp}
    /// if (IconButtonCentered("ok", "OK")) {
    ///     // clicked
    /// }
    /// \endcode
    bool IconButtonCentered(
            const char* id,
            const char* text,
            const IconButtonConfig& cfg = {}
    );

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "icon_button.ipp"
#endif

#endif // _IMGUIX_WIDGETS_ICON_BUTTON_HPP_INCLUDED
