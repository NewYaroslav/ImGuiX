#pragma once
#ifndef _IMGUIX_WIDGETS_VIRTUAL_KEYBOARD_OVERLAY_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VIRTUAL_KEYBOARD_OVERLAY_HPP_INCLUDED

/// \file virtual_keyboard_overlay.hpp
/// \brief Overlay window for VirtualKeyboard with focus/close behavior.

#include <imgui.h>
#include <string>
#include "virtual_keyboard.hpp"

namespace ImGuiX::Widgets {

    /// \brief Render VirtualKeyboard in a centered overlay window.
    /// \param id Unique id for the overlay window (per field).
    /// \param text In/out string bound to keyboard.
    /// \param cfg Base keyboard config (copied, then wrapped for auto-close).
    /// \param visible_io In/out: when true shows overlay, when false hides. Sets false on close.
    /// \param overlay_size Preferred size (0,0 => auto).
    /// \return true if text was modified this frame.
bool VirtualKeyboardOverlay(
            const char* id,
            std::string& text,
            VirtualKeyboardConfig cfg,
            bool* visible_io,
            ImVec2 overlay_size = ImVec2(0,0)
        );

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "virtual_keyboard_overlay.ipp"
#endif

#endif // _IMGUIX_WIDGETS_VIRTUAL_KEYBOARD_OVERLAY_HPP_INCLUDED
