#pragma once
#ifndef _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED

/// \file system_button.hpp
/// \brief Provides custom system-style buttons (close, minimize, maximize) for ImGui windows.

#include <imgui.h>

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
        Close,
        Minimize,
        Maximize
    };

    /// \brief Draw system-style button with icon based on type.
    /// \param id Unique ID string.
    /// \param type Button type.
    /// \param size Button size in pixels.
    /// \return True if button was clicked.
    /// \note Button is highlighted on hover or active using style colors.
    bool SystemButton(const char* id, SystemButtonType type, ImVec2 size);

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "system_button.ipp"
#endif

#endif // _IMGUIX_WIDGETS_SYSTEM_BUTTON_HPP_INCLUDED
