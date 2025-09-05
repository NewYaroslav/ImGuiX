#pragma once
#ifndef _IMGUIX_WIDGETS_INPUT_TEXT_ICON_TOGGLE_HPP_INCLUDED
#define _IMGUIX_WIDGETS_INPUT_TEXT_ICON_TOGGLE_HPP_INCLUDED

/// \file InputTextWithIconToggle.hpp
/// \brief Single-line InputText with integrated right-side eye toggle (show/hide).
/// \note Uses internal API (imgui_internal.h). ImGui 1.89+.
/// \note The icon lives inside the same framed control with proper rounded corners.
/// \note Multiline is not supported here.

#include <imgui.h>
#include <imgui_internal.h> // ButtonBehavior, RenderFrameBorder, etc.

namespace ImGuiX::Widgets {

    /// \brief Single-line InputText with integrated right-side eye toggle (show/hide).
    /// \param label   Visible label (also used to build a stable PushID scope).
    /// \param hint    Optional gray hint (nullptr = no hint).
    /// \param buf     User buffer (zero-terminated).
    /// \param buf_sz  Size of user buffer (including terminator).
    /// \param reveal  IN/OUT: true = plain text; false = password bullets.
    /// \param flags   Base flags for InputText (Password flag managed internally).
    /// \param eye_on  UTF-8 icon for "visible" (e.g. u8"\uE8F4").
    /// \param eye_off UTF-8 icon for "hidden"  (e.g. u8"\uE8F5").
    /// \param icon_font Optional font for icon (nullptr = current font).
    /// \return True if text changed this frame.
    bool InputTextWithIconToggle(
            const char* label,
            const char* hint,
            char*       buf,
            int         buf_sz,
            bool*       reveal,
            ImGuiInputTextFlags flags = 0,
            const char* eye_on  = u8"\uE8F4",
            const char* eye_off = u8"\uE8F5",
            ImFont*     icon_font = nullptr,
            ImGuiInputTextCallback callback = nullptr,
            void*       user_data = nullptr,
            ImU32 eye_text_col_override = 0,
            ImU32 eye_bg_col_override   = 0
        );

#   ifdef IMGUIX_DEMO
    /// \brief Minimal demo: two fields, one plain validated, one password with VK.
    void DemoInputTextWithIconToggle();
#   endif

} // namespace ImGuiX::Widgets

#if defined(IMGUIX_HEADER_ONLY) || defined(IMGUIX_INCLUDE_IPP)

#   include "InputTextWithIconToggle.ipp"

#endif

#endif // _IMGUIX_WIDGETS_INPUT_TEXT_ICON_TOGGLE_HPP_INCLUDED
