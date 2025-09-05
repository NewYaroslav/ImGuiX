#pragma once
#ifndef _IMGUIX_WIDGETS_INPUT_TEXT_ICON_TOGGLE_HPP_INCLUDED
#define _IMGUIX_WIDGETS_INPUT_TEXT_ICON_TOGGLE_HPP_INCLUDED

/// \file InputTextWithIconToggle.hpp
/// \brief Single-line InputText with integrated right-side eye toggle (show/hide).
/// \note Uses internal API (imgui_internal.h). ImGui 1.89+.
/// \note The icon lives inside the same framed control with proper rounded corners.
/// \note Multiline is not supported here.

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Single-line InputText with integrated right-side eye toggle (show/hide).
    /// \param label     Visible label and ID scope.
    /// \param hint      Gray placeholder; nullptr for none.
    /// \param buf       Mutable UTF-8 buffer.
    /// \param buf_sz    Buffer size including terminator.
    /// \param reveal    IN/OUT toggle state; true shows text.
    /// \param flags     Base InputText flags; Password managed internally.
    /// \param eye_on    UTF-8 icon when text visible.
    /// \param eye_off   UTF-8 icon when text hidden.
    /// \param icon_font Font for icons; nullptr uses current.
    /// \param callback  Optional input callback.
    /// \param user_data User context for callback.
    /// \param eye_text_col_override Override icon color; 0 uses style.
    /// \param eye_bg_col_override Override background color; 0 uses style.
    /// \return true if text changed.
    /// \pre label && buf && reveal are not nullptr.
    /// \pre buf_sz > 0.
    /// \pre (flags & ImGuiInputTextFlags_Multiline) == 0.
    /// \thread_safety Not thread-safe.
    /// \code{.cpp}
    /// bool reveal = false;
    /// char buf[128]{};
    /// ImGuiX::Widgets::InputTextWithIconToggle("Password", nullptr, buf, IM_ARRAYSIZE(buf), &reveal);
    /// \endcode
    inline bool InputTextWithIconToggle(
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
    /// \brief Show demo with password toggle and virtual keyboard.
    void DemoInputTextWithIconToggle();
#   endif

} // namespace ImGuiX::Widgets

#if defined(IMGUIX_HEADER_ONLY) || defined(IMGUIX_INCLUDE_IPP)

#   include "InputTextWithIconToggle.ipp"

#endif

#endif // _IMGUIX_WIDGETS_INPUT_TEXT_ICON_TOGGLE_HPP_INCLUDED
