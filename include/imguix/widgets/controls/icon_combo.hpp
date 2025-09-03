#pragma once
#ifndef _IMGUIX_WIDGETS_ICON_COMBO_HPP_INCLUDED
#define _IMGUIX_WIDGETS_ICON_COMBO_HPP_INCLUDED

/// \file icon_combo.hpp
/// \brief Combo widget with custom icon on the right.

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Begin combo without arrow and draw overlay icon on the right.
    /// \param id ImGui identifier.
    /// \param preview Text preview shown in combo.
    /// \param icon Icon text (UTF-8).
    /// \param icon_slot_w Width reserved for icon overlay (0 -> auto from text).
    /// \param flags ImGuiComboFlags.
    /// \return True if combo is open.
    inline bool BeginIconCombo(
            const char* id,
            const char* preview,
            const char* icon,
            float icon_slot_w = 0.0f,
            ImGuiComboFlags flags = ImGuiComboFlags_HeightLargest | ImGuiComboFlags_PopupAlignLeft
    );

    /// \brief End combo started with BeginIconCombo.
    inline void EndIconCombo();

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "icon_combo.ipp"
#endif

#endif // _IMGUIX_WIDGETS_ICON_COMBO_HPP_INCLUDED
