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
    
    enum class MarkerMode : int {
        TooltipOnly = 0,  ///< icon/label + tooltip on hover
        InlineText  = 1   ///< icon/label + inline wrapped text
    };

    void TooltipWrapped(const char* desc, float wrap_cols = 35.0f);
    
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
    void HelpMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const char* icon_utf8 = IMGUIX_ICON_HELP
        );

    /// \brief Warning marker with yellow icon and text.
    /// \param desc Warning message.
    void WarningMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const ImVec4& color = IMGUIX_COLOR_WARNING,
            const char* icon_utf8 = IMGUIX_ICON_WARNING
        );

    /// \brief Info marker with blue icon and text.
    /// \param desc Information message.
    void InfoMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const ImVec4& color = IMGUIX_COLOR_INFO,
            const char* icon_utf8 = IMGUIX_ICON_INFO
        );

    /// \brief Success marker with green icon and text.
    /// \param desc Success message.
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
