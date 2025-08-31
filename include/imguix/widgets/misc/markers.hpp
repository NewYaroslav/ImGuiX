#pragma once
#ifndef _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED
#define _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED

/// \file markers.hpp
/// \brief Assorted marker widgets for colored labels and tooltips.

#include <imgui.h>
#include <string>

#include <imguix/config/icons.hpp>
#include <imguix/config/colors.hpp>

namespace ImGuiX::Widgets {

    /// \brief Marker display mode.
    enum class MarkerMode : int {
        TooltipOnly = 0,  ///< Icon/label with tooltip on hover.
        InlineText  = 1   ///< Icon/label with inline wrapped text.
    };

    /// \brief Show tooltip text with word wrapping.
    /// \param desc Tooltip text.
    /// \param wrap_cols Column width for wrapping.
    void TooltipWrapped(const char* desc, float wrap_cols = 35.0f);

    /// \brief Draw icon marker with tooltip or inline text.
    /// \param icon_utf8 Icon UTF-8 string.
    /// \param color Icon color.
    /// \param desc Tooltip or inline text.
    /// \param mode Display mode.
    /// \param wrap_cols Column width for wrapping.
    void IconMarker(
            const char* icon_utf8,
            const ImVec4& color,
            const char* desc,
            MarkerMode mode,
            float wrap_cols = 35.0f
        );

    /// \brief Draw label in custom color with tooltip.
    /// \param label Text displayed on screen.
    /// \param desc Tooltip text shown when hovered.
    /// \param color Text color.
    void ColoredMarker(
            const char* label,
            const char* desc,
            const ImVec4& color
        );

    /// \brief Selectable label with tooltip echoing its text.
    /// \param text Text shown and displayed in tooltip.
    /// \return True if selected.
    bool SelectableMarker(const std::string& text);

    /// \brief Help marker using a question icon with tooltip.
    /// \param desc Help text.
    /// \param mode Display mode.
    /// \param icon_utf8 Icon glyph.
    void HelpMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const char* icon_utf8 = IMGUIX_ICON_HELP
        );

    /// \brief Warning marker with yellow icon and text.
    /// \param desc Warning message.
    /// \param mode Display mode.
    /// \param color Icon and text color.
    /// \param icon_utf8 Icon glyph.
    void WarningMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const ImVec4& color = IMGUIX_COLOR_WARNING,
            const char* icon_utf8 = IMGUIX_ICON_WARNING
        );

    /// \brief Info marker with blue icon and text.
    /// \param desc Information message.
    /// \param mode Display mode.
    /// \param color Icon and text color.
    /// \param icon_utf8 Icon glyph.
    void InfoMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const ImVec4& color = IMGUIX_COLOR_INFO,
            const char* icon_utf8 = IMGUIX_ICON_INFO
        );

    /// \brief Success marker with green icon and text.
    /// \param desc Success message.
    /// \param mode Display mode.
    /// \param color Icon and text color.
    /// \param icon_utf8 Icon glyph.
    void SuccessMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const ImVec4& color = IMGUIX_COLOR_SUCCESS,
            const char* icon_utf8 = IMGUIX_ICON_SUCCESS
        );

} // namespace ImGuiX::Widgets
#ifdef IMGUIX_HEADER_ONLY
#   include "markers.ipp"
#endif

#endif // _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED
