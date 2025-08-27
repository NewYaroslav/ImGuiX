#pragma once
#ifndef _IMGUIX_WIDGETS_TEXT_CENTER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_TEXT_CENTER_HPP_INCLUDED

/// \file text_center.hpp
/// \brief Helpers to center text within the current content region (single-line, unformatted, wrapped).
///
/// ImGui 1.92.* compatible: uses only public API + <cstdio> for formatting.

#include <imgui.h>
#include <string>

namespace ImGuiX::Widgets {

    /// \brief Center a single-line formatted text using ImGui::Text.
    /// \param fmt Format string for std::vsnprintf.
    /// \note If text width exceeds available width, falls back to left alignment.
    void TextCenteredFmt(const char* fmt, ...);

    /// \brief Center an unformatted string (no printf parsing).
    /// \param text UTF-8 string to render.
    void TextUnformattedCentered(const char* text);

    /// \brief Center a wrapped text block by placing it into a centered child.
    /// \param text Text to render.
    /// \param wrap_width Desired wrap width in pixels (<= avail). If <= 0, uses available width.
    /// \note The function creates a child region of the target width and centers it, then uses TextWrapped inside.
    void TextWrappedCentered(const char* text, float wrap_width = 0.0f);
    
    /// \brief Overload for std::string (unformatted).
    inline void TextUnformattedCentered(const std::string& s) {
        TextUnformattedCentered(s.c_str());
    }

    /// \brief Overload for std::string (wrapped).
    inline void TextWrappedCentered(const std::string& s, float wrap_width = 0.0f) {
        TextWrappedCentered(s.c_str(), wrap_width);
    }

} // namespace ImGuiX::Widgets
#ifdef IMGUIX_HEADER_ONLY
#   include "text_center.ipp"
#endif

#endif // _IMGUIX_WIDGETS_TEXT_CENTER_HPP_INCLUDED
