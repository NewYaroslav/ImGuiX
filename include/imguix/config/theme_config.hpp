#pragma once
#ifndef _IMGUIX_CONFIG_THEME_HPP_INCLUDED
#define _IMGUIX_CONFIG_THEME_HPP_INCLUDED

/// \file theme_config.hpp
/// \brief Theme-related UI configuration values: paddings, roundings, borders etc.

#include <imgui.h>

namespace ImGuiX::Config {

    /// \brief Frame border thickness.
    constexpr float FRAME_BORDER_SIZE     = 1.0f;

    /// \brief Frame corner radius.
    constexpr float FRAME_ROUNDING       = 4.0f;

    /// \brief Child window corner radius.
    constexpr float CHILD_ROUNDING       = 4.0f;

    /// \brief Minimum grab size.
    constexpr float GRAB_MIN_SIZE        = 7.0f;

    /// \brief Popup corner radius.
    constexpr float POPUP_ROUNDING       = 4.0f;

    /// \brief Scrollbar corner radius.
    constexpr float SCROLLBAR_ROUNDING   = 12.0f;

    /// \brief Tab border thickness.
    constexpr float TAB_BORDER_SIZE      = 1.0f;

    /// \brief Tab corner radius.
    constexpr float TAB_ROUNDING         = 4.0f;

    /// \brief Window corner radius.
    constexpr float WINDOW_ROUNDING      = 4.0f;

    /// \brief Scrollbar width.
    constexpr float SCROLLBAR_SIZE       = 15.0f;

    /// \brief Window border thickness.
    constexpr float WINDOW_BORDER_SIZE   = 1.0f;

    /// \brief Child window border thickness.
    constexpr float CHILD_BORDER_SIZE    = 1.0f;

    /// \brief Popup border thickness.
    constexpr float POPUP_BORDER_SIZE    = 1.0f;

    /// \brief Grab corner radius.
    constexpr float GRAB_ROUNDING        = 4.0f;

    /// \brief Window padding.
    constexpr ImVec2 WINDOW_PADDING       = ImVec2(4.0f, 4.0f);

    /// \brief Spacing between items.
    constexpr ImVec2 ITEM_SPACING         = ImVec2(6.0f, 2.0f);

} // namespace ImGuiX::Config

#endif // _IMGUIX_CONFIG_THEME_HPP_INCLUDED
