#pragma once
#ifndef _IMGUIX_WIDGETS_PROXY_PANEL_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PROXY_PANEL_HPP_INCLUDED

/// \file proxy_panel.hpp
/// \brief Proxy settings panel: use flag, host:port, type, auth, and "Check" action.

#include <imgui.h>
#include <string>
#include <functional>
#include <regex>
#include <algorithm>
#include <cstring>

#include <imguix/config/colors.hpp>

namespace ImGuiX::Widgets {

    /// \brief Proxy type.
    enum class ProxyType { HTTP = 0, SOCKS4 = 1, SOCKS5 = 2 };

    /// \brief Mutable model of proxy settings.
    struct ProxySettings {
        bool        use_proxy      = false;
        std::string ip;            ///< host or IPv4 string
        int         port           = 0;        ///< 0..65535
        ProxyType   type           = ProxyType::HTTP;
        std::string username;
        std::string password;
        
        bool ip_valid        = true; ///< out (set by widget if cfg.validate_ip)
        bool port_valid      = true; ///< out
        bool username_valid  = true; ///< out
        bool password_valid  = true; ///< out
    };

    /// \brief UI configuration for ProxyPanel.
    struct ProxyPanelConfig {
        ImVec2 panel_size             = ImVec2(0, 0); ///< 0→auto width/height
        bool   inputs_fill_width      = true;         ///< make input fields fill panel width
        bool   border                 = true;

        // Labels
        const char* header            = u8"Proxy settings";
        const char* hint_ip           = u8"ip";
        const char* hint_port         = u8"port";
        const char* hint_username     = u8"user";
        const char* hint_password     = u8"password";
        const char* label_use_proxy   = u8"use proxy";
        const char* button_check      = u8"check proxy";
        const char* label_checked     = u8"checked";

        // Options
        bool show_type                = true;   ///< show type combo (HTTP/SOCKS)
        bool show_check               = true;   ///< show "check proxy" button + status
        bool checked                  = false;  ///< result of last check (visual only)
        bool check_ok                 = false;  ///< last check OK (green) vs fail (red)

        // Virtual keyboard configs (shared across fields)
        VirtualKeyboardConfig      vk_cfg{};   ///< behavior/locale/etc of VK
        KeyboardToggleConfig       kb_cfg{};   ///< visuals of the VK trigger button
        
        PasswordToggleConfig       toggle_cfg{}; ///< password eye config
        
        //
        bool vk_ip                    = true;
        bool vk_port                  = true;
        bool vk_username              = true;
        bool vk_password              = true;
        
        // Validation toggles
        bool validate_ip           = true;
        bool validate_port         = true;
        bool validate_username     = true;
        bool validate_password     = true;
    
        const char* ip_regex          = u8R"(^\d{1,3}(\.\d{1,3}){3}$)";
        const char* port_regex        = u8R"(^[A-Za-z0-9.\-:]+$)";
        const char* username_regex    = u8R"(^\S+$)";
        const char* password_regex    = u8R"(^\S+$)";
        
        // Callbacks
        std::function<void()> on_check = nullptr;

        // Layout
        float field_width_host_user   = 0.0f;
        float field_width_port        = 0.0f;
        float field_width_type        = 0.0f;

        // Style
        ImVec4 error_color = IMGUIX_COLOR_ERROR;
    };

    /// \brief Draw proxy settings panel.
    /// \param id Unique widget identifier.
    /// \param cfg Panel configuration.
    /// \param st Proxy settings model to edit.
    /// \return True if any settings changed this frame.
    bool ProxyPanel(const char* id, ProxyPanelConfig& cfg, ProxySettings& st);

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "proxy_panel.ipp"
#endif

#endif // _IMGUIX_WIDGETS_PROXY_PANEL_HPP_INCLUDED
