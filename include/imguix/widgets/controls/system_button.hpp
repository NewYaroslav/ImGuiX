#pragma once
#ifndef _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED

/// \file system_button.hpp
/// \brief Custom system-style buttons for ImGui windows.

#include <imgui.h>

#ifndef IMGUIX_SYSBTN_BG_ROUNDING
#define IMGUIX_SYSBTN_BG_ROUNDING 0.0f   // system look -> no rounding
#endif

/// \brief Extent multiplier for system button icons (default 0.25f).
#ifndef IMGUIX_SYSBTN_CROSS_EXTENT
#define IMGUIX_SYSBTN_CROSS_EXTENT 0.25f
#endif

/// \brief Margin in pixels from button border (default 1.0f).
#ifndef IMGUIX_SYSBTN_MARGIN
#define IMGUIX_SYSBTN_MARGIN 1.0f
#endif

/// \brief Line thickness in pixels.
#ifndef IMGUIX_SYSBTN_LINE_THICKNESS
#define IMGUIX_SYSBTN_LINE_THICKNESS 1.0f
#endif

namespace ImGuiX::Widgets {

    /// \brief Type of the system-style button.
    enum class SystemButtonType {
        Close,    ///< Close window
        Minimize, ///< Minimize window
        Maximize  ///< Maximize window
    };

    /// \brief Draw system-style button.
    /// \param id Unique ID string.
    /// \param type Button type.
    /// \param size Button size in pixels.
    /// \return True if button was clicked.
    /// \note Button is highlighted on hover or active using style colors.
    /// \code{.cpp}
    /// if (SystemButton("close", SystemButtonType::Close, ImVec2{16, 16})) {
    ///     // clicked
    /// }
    /// \endcode
    bool SystemButton(const char* id, SystemButtonType type, ImVec2 size);

    /// \brief Borderless close button like titlebar 'X'.
    /// \param id Unique ID string.
    /// \param size Button size in pixels; 0 => square using GetFrameHeight().
    /// \return True if button was clicked.
    /// \code{.cpp}
    /// if (CloseXButton("close")) {
    ///     // clicked
    /// }
    /// \endcode
    inline bool CloseXButton(const char* id, float size = 0.0f) {
        if (size <= 0.0f) size = ImGui::GetFrameHeight();
        return SystemButton(id, SystemButtonType::Close, ImVec2(size, size));
    }

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "system_button.ipp"
#endif

#endif // _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED
