// imguix/core/notify/NotificationTypes.hpp
#pragma once
#ifndef IMGUIX_CORE_NOTIFY_NOTIFICATION_TYPES_HPP_INCLUDED
#define IMGUIX_CORE_NOTIFY_NOTIFICATION_TYPES_HPP_INCLUDED

/// \file NotificationTypes.hpp
/// \brief Common types/config for ImGuiX notifications.

#include <cstdint>
#include <imgui.h>
#include <imguix/config/notifications.hpp> // IMGUIX_NOTIFY_ICON_* and IMGUIX_NOTIFY_COLOR_*

namespace ImGuiX::Notify {
    
    // ---- Types --------------------------------------------------------------

    /// \brief Toast types.
    enum class Type  : std::uint8_t { None, Success, Warning, Error, Info, COUNT };

    /// \brief Toast lifecycle phase.
    enum class Phase : std::uint8_t { FadeIn, Wait, FadeOut, Expired, COUNT };

    /// \brief Auto-duration mode.
    enum class AutoDurationMode : std::uint8_t { Off, PerChar, WPM };
    
    // ---- Config -------------------------------------------------------------

    /// \brief Visual/behavioral configuration of notifications.
    struct Config {
        // Positioning
        float padding_x           = 20.0f;   ///< from right edge
        float padding_y           = 20.0f;   ///< from bottom edge
        float padding_message_y   = 10.0f;   ///< gap between toasts
        bool  outside_main_window = true;    ///< use WorkPos/WorkSize

        // Rendering
        float wrap_width_frac     = 1.0f / 3.0f; ///< text wrap as viewport fraction
        float opacity             = 0.8f;        ///< global opacity
        float close_btn_right_margin = 0.0f;     ///< margin between close button and right edge
        bool  use_separator       = false;       ///< separator between title/content
        bool  use_dismiss_button  = true;        ///< show close button
        int   render_limit        = 5;           ///< 0 = unlimited

        // Timing
        int   fade_in_out_ms      = 150;
        int   default_dismiss_ms  = 3000;

        // Window flags
        bool  use_tooltip_layer   = false;       ///< place window in foreground layer
        ImGuiWindowFlags base_window_flags =
            ImGuiWindowFlags_Tooltip               |
            ImGuiWindowFlags_AlwaysAutoResize      |
            ImGuiWindowFlags_NoDecoration          |
            ImGuiWindowFlags_NoNav                 |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoFocusOnAppearing    |
            // ImGuiWindowFlags_NoDocking             |
            ImGuiWindowFlags_NoSavedSettings;
            
        // --- auto duration ---
        AutoDurationMode auto_mode = AutoDurationMode::WPM;

        // PerChar: milliseconds added per character
        float ms_per_char = 32.0f; ///< tune between 20 and 60 ms

        // WPM: reading speed and average word length
        float reading_wpm        = 220.0f; ///< words per minute
        float avg_chars_per_word = 5.0f;   ///< average characters per word

        // Extra time per wrapped line when text spans multiple lines
        int   extra_per_line_ms = 120;     ///< milliseconds per wrapped line

        // Limits and behaviour
        int   min_dismiss_ms = 1500;       ///< minimum dismiss time
        int   max_dismiss_ms = 15000;      ///< maximum dismiss time
        bool  pause_on_hover = true;       ///< pause timer when hovered
    };
    
    /// \brief Icon config (non-owning pointers to FA6 glyphs).
    struct IconConfig {
        const char* icon_success = IMGUIX_NOTIFY_ICON_SUCCESS; ///< success icon
        const char* icon_warning = IMGUIX_NOTIFY_ICON_WARNING; ///< warning icon
        const char* icon_error   = IMGUIX_NOTIFY_ICON_ERROR;   ///< error icon
        const char* icon_info    = IMGUIX_NOTIFY_ICON_INFO;    ///< info icon
        const char* icon_close   = IMGUIX_NOTIFY_ICON_CLOSE;   ///< close icon
    };

    /// \brief Optional font configuration.
    struct FontConfig {
        ImFont* text  = nullptr; ///< text font; null uses current
        ImFont* icons = nullptr; ///< icon font; null uses merged icons
    };

} // namespace ImGuiX::Notify

#endif // IMGUIX_CORE_NOTIFY_NOTIFICATION_TYPES_HPP_INCLUDED
