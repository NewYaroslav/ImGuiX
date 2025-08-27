#pragma once
#ifndef _IMGUIX_WIDGETS_TOGGLE_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_TOGGLE_BUTTON_HPP_INCLUDED

/// \file toggle_button.hpp
/// \brief Provides a simple animated toggle switch for ImGui (public API only).

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Draw an animated toggle switch (no imgui_internal.h).
    /// \param id Unique string identifier.
    /// \param state Pointer to boolean storing current state.
    void ToggleButton(const char* id, bool* state);

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "toggle_button.ipp"
#endif

#endif // _IMGUIX_WIDGETS_TOGGLE_BUTTON_HPP_INCLUDED

