#pragma once
#ifndef _IMGUIX_CONFIG_THEME_HPP_INCLUDED
#define _IMGUIX_CONFIG_THEME_HPP_INCLUDED

/// \file theme_config.hpp
/// \brief Theme-related UI configuration values: paddings, roundings, borders etc.

#include <imgui.h>

namespace ImGuiX::Config {

    // --- Scalar Values ---
    constexpr float FRAME_BORDER_SIZE     = 1.0f;
    constexpr float FRAME_ROUNDING       = 4.0f;
    constexpr float CHILD_ROUNDING       = 4.0f;
    constexpr float GRAB_MIN_SIZE        = 7.0f;
    constexpr float POPUP_ROUNDING       = 4.0f;
    constexpr float SCROLLBAR_ROUNDING   = 12.0f;
    constexpr float TAB_BORDER_SIZE      = 1.0f;
    constexpr float TAB_ROUNDING         = 4.0f;
    constexpr float WINDOW_ROUNDING      = 4.0f;
    constexpr float SCROLLBAR_SIZE       = 15.0f;
    constexpr float WINDOW_BORDER_SIZE   = 1.0f;
    constexpr float CHILD_BORDER_SIZE    = 1.0f;
    constexpr float POPUP_BORDER_SIZE    = 1.0f;
    constexpr float GRAB_ROUNDING        = 4.0f;

    // --- Vec2 Values ---
    constexpr ImVec2 WINDOW_PADDING       = ImVec2(4.0f, 4.0f);
    constexpr ImVec2 ITEM_SPACING         = ImVec2(6.0f, 2.0f);

} // namespace ImGuiX::Config

#endif // _IMGUIX_CONFIG_THEME_HPP_INCLUDED
