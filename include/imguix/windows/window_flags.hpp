#pragma once
#ifndef _IMGUIX_WINDOWS_WINDOW_FLAG_HPP_INCLUDED
#define _IMGUIX_WINDOWS_WINDOW_FLAG_HPP_INCLUDED

#include <cstdint>

namespace ImGuiX::Windows {

    /// \brief Flags used to configure window behavior and appearance.
    enum class WindowFlags : uint32_t {
        None                       = 0,
        HasMenuBar                 = 1 << 0, ///< Enable ImGui menu bar region
        EnableTransparency         = 1 << 1, ///< Use DWM transparency (Windows only)
        DisableBackground          = 1 << 2, ///< Disable background color in ImGui::Begin
        ShowControlButtons         = 1 << 3, ///< Show minimize/maximize/close buttons
        MacStyledControlButtons    = 1 << 4, ///< macOS style control buttons
        ImGuiStyledControlButtons  = 1 << 5, ///< ImGui style control buttons

        /// \brief Shortcut flag to enable both control buttons and ImGui styling.
        DefaultControlButtons      = ShowControlButtons | ImGuiStyledControlButtons
    };

    /// \brief Bitwise OR for WindowFlags.
    inline WindowFlags operator|(WindowFlags a, WindowFlags b) {
        return static_cast<WindowFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    /// \brief Bitwise AND test for WindowFlags.
    inline bool hasFlag(WindowFlags value, WindowFlags flag) {
        return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
    }

} // namespace ImGuiX::Windows

#endif // _IMGUIX_WINDOWS_WINDOW_FLAG_HPP_INCLUDED