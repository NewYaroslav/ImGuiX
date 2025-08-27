#ifndef _IMGUIX_WIDGETS_CIRCLE_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_CIRCLE_BUTTON_HPP_INCLUDED

#include <imgui.h>

/// \file circle_button.hpp
/// \brief Provides a utility for drawing circular ImGui buttons.

namespace ImGuiX::Widgets {

    /// \brief Draw simple circular button with solid fill.
    /// \param id Unique string ID.
    /// \param diameter Button diameter in pixels.
    /// \param color Base color.
    /// \return True if button was clicked.
    /// \note Button changes color on hover or active using style.
    bool CircleButton(const char* id, float diameter, const ImVec4& color);

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "circle_button.ipp"
#endif

#endif // _IMGUIX_WIDGETS_CIRCLE_BUTTON_HPP_INCLUDED
