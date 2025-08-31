#pragma once
#ifndef _IMGUIX_WIDGETS_TOGGLE_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_TOGGLE_BUTTON_HPP_INCLUDED

/// \file toggle_button.hpp
/// \brief Animated toggle switch for ImGui.

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Draw animated toggle switch.
    /// \param id Unique string identifier.
    /// \param state Pointer to boolean storing current state.
    /// \code{.cpp}
    /// bool enabled = false;
    /// ToggleButton("enabled", &enabled);
    /// \endcode
    void ToggleButton(const char* id, bool* state);

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "toggle_button.ipp"
#endif

#endif // _IMGUIX_WIDGETS_TOGGLE_BUTTON_HPP_INCLUDED

