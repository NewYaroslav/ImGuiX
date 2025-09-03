#pragma once
#ifndef _IMGUIX_WIDGETS_ICON_COMBO_HPP_INCLUDED
#define _IMGUIX_WIDGETS_ICON_COMBO_HPP_INCLUDED

/// \file icon_combo.hpp
/// \brief Combo widget with a custom right-side icon that replaces the default arrow.

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Begin a combo that preserves standard behavior but renders an icon instead of the arrow.
    /// \param label ImGui label/id.
    /// \param preview_value Text shown in the preview area.
    /// \param icon_text UTF-8 icon glyph (e.g., Material Icons PUA).
    /// \param flags ImGuiComboFlags for BeginCombo.
    /// \param icon_font Optional font used for the icon (nullptr = current font).
    /// \return true if the combo popup is open.
    inline bool BeginIconCombo(const char* label,
                               const char* preview_value,
                               const char* icon_text,
                               ImGuiComboFlags flags = 0,
                               ImFont* icon_font = nullptr);

    /// \brief End combo started with BeginIconCombo().
    inline void EndIconCombo();

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "icon_combo.ipp"
#endif

#endif // _IMGUIX_WIDGETS_ICON_COMBO_HPP_INCLUDED
