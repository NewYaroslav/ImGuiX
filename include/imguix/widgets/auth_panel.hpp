#pragma once
#ifndef _IMGUIX_WIDGETS_AUTH_PANEL_HPP_INCLUDED
#define _IMGUIX_WIDGETS_AUTH_PANEL_HPP_INCLUDED

/// \file auth_panel.hpp
/// \brief Login/auth panel: email + password (+optional host), connection state and action.

#include <imgui.h>
#include <string>
#include <functional>
#include <regex>
#include <cstring>
#include <algorithm>
#include <type_traits>
#include <imguix/config/fonts.hpp>
#include "validated_password_input.hpp"

namespace ImGuiX::Widgets {

    /// \brief Bitmask result for AuthPanel events.
    enum class AuthPanelResult : unsigned {
        None             = 0u,
        EmailChanged     = 1u << 0,
        PasswordChanged  = 1u << 1,
        HostChanged      = 1u << 2,
        TokenChanged     = 1u << 3,
        ApiKeyChanged    = 1u << 4,
        ApiSecretChanged = 1u << 5,
        ConnectClicked   = 1u << 6
    };

    inline AuthPanelResult operator|(AuthPanelResult a, AuthPanelResult b) {
        return static_cast<AuthPanelResult>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
    }
    
    inline AuthPanelResult operator&(AuthPanelResult a, AuthPanelResult b) {
        return static_cast<AuthPanelResult>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
    }
    
    inline AuthPanelResult& operator|=(AuthPanelResult& a, AuthPanelResult b) { a = a | b; return a; }

    inline bool Any(ImGuiX::Widgets::AuthPanelResult r) noexcept {
        using U = std::underlying_type_t<ImGuiX::Widgets::AuthPanelResult>;
        return static_cast<U>(r) != 0u;
    }

    inline bool Has(ImGuiX::Widgets::AuthPanelResult r,
                    ImGuiX::Widgets::AuthPanelResult f) noexcept {
        using U = std::underlying_type_t<ImGuiX::Widgets::AuthPanelResult>;
        return (static_cast<U>(r) & static_cast<U>(f)) != 0u;
    }

    /// \brief Configuration for AuthPanel.
    struct AuthPanelConfig {
        ImVec2 panel_size = ImVec2(0.0f, 0.0f); ///< x<=0: fill available width; y<=0: auto-computed
        bool   inputs_fill_width = true;        ///< make input fields fill panel width
        
        // Labels (now raw pointers; caller owns lifetime)
        const char* header          = u8"Authorization";
        const char* hint_email      = u8"email";
        const char* hint_password   = u8"password";
        const char* hint_host       = u8"host";
        const char* connected_label = u8"connected";
        const char* connect_label   = u8"connect";

        // Tokens / API keys (now raw pointers)
        const char* hint_token      = u8"token";
        const char* hint_api_key    = u8"api key (public)";
        const char* hint_api_secret = u8"api secret";

        // Options
        bool show_host             = false;
        bool show_email            = true;
        bool show_password         = true;
        bool show_token            = false;
        bool show_api_keys         = false; ///< public + secret
        bool mask_api_secret       = true;  ///< show secret as password
        bool show_connect_button   = true;
        bool show_connection_state = true;

        // Validation toggles
        bool validate_email        = true;
        bool validate_password     = true;
        bool validate_host         = true;
        bool validate_token        = true;
        bool validate_api_key      = true;
        bool validate_api_secret   = true;

        // Virtual keyboard configs (shared across fields)
        VirtualKeyboardConfig      vk_cfg{};   ///< behavior/locale/etc of VK
        KeyboardToggleConfig       kb_cfg{};   ///< visuals of the VK trigger button
        
        // Inline password eye config
        PasswordToggleConfig toggle_cfg{};     ///< password eye config

        // Per-field VK enable switches
        bool vk_host        = false;
        bool vk_email       = false;
        bool vk_password    = false;
        bool vk_token       = false;
        bool vk_api_key     = false;
        bool vk_api_secret  = false;

        // Connection state visuals
        bool   init      = false;
        bool   connected = false;

        // Optional email state pill on the right
        bool        use_email_state   = false;
        bool        email_state       = false;
        ImVec4      email_state_color = ImVec4(0,0,0,1);
        std::string email_state_label;

        // Callbacks
        std::function<void()> on_connect;

        // Regexes (basic)
        const char* email_regex      = u8R"(.+@.+\.\w+)";                    ///< 
        const char* password_regex   = u8R"(^\S+$)";                         ///< 
        const char* host_regex       = u8R"(^[A-Za-z0-9.\-:]+$)";            ///< host:port / domain
        const char* token_regex      = u8R"(^(?:[A-Za-z0-9._\-]{16,256})$)"; ///< 
        const char* api_key_regex    = u8R"(^(?:[A-Za-z0-9._\-]{16,256})$)"; ///< 
        const char* api_secret_regex = u8R"(^(?:[A-Za-z0-9._\-]{16,256})$)"; ///< 

        ImVec4      error_color      = ImVec4(0.9f, 0.5f, 0.5f, 1.0f);
    };
    
    /// \brief All auth-related fields passed in/out by reference.
    struct AuthData {
        std::string email;       ///< in/out
        std::string password;    ///< in/out
        std::string host;        ///< in/out, used if cfg.show_host
        std::string token;       ///< in/out, used if cfg.show_token
        std::string api_key;     ///< in/out, used if cfg.show_api_keys
        std::string api_secret;  ///< in/out, used if cfg.show_api_keys

        bool email_valid     = true;  ///< out (set by widget if cfg.validate_email)
        bool password_valid  = true;  ///< out
        bool host_valid      = true;  ///< out
        bool token_valid     = true;  ///< out
        bool api_key_valid   = true;  ///< out
        bool api_secret_valid= true;  ///< out
    };

    /// \brief Renders login form. Optionally includes host input.
    /// \param id Unique widget ID.
    /// \param cfg Panel configuration.
    /// \param data All input/output fields and validity flags (set inside).
    /// \return Bitmask of events happened this frame.
    inline AuthPanelResult AuthPanel(
            const char* id,
            AuthPanelConfig& cfg,
            AuthData& data
        ) {
        AuthPanelResult res = AuthPanelResult::None;
        ImGui::PushID(id);

        // Compute a conservative height to keep the child stable.
        float height = 0.0f;
        height += ImGui::GetTextLineHeightWithSpacing(); // header
        height = std::max(height, ImGui::GetFrameHeightWithSpacing());

        if (cfg.show_host)      height += ImGui::GetFrameHeightWithSpacing();
        if (cfg.show_email)     height += ImGui::GetFrameHeightWithSpacing();
        if (cfg.show_password)  height += ImGui::GetFrameHeightWithSpacing();
        if (cfg.show_token)     height += ImGui::GetFrameHeightWithSpacing();
        if (cfg.show_api_keys)  height += 2.0f * ImGui::GetFrameHeightWithSpacing();
        if (cfg.show_connect_button) height += ImGui::GetFrameHeightWithSpacing();
        height += ImGui::GetFrameHeightWithSpacing();
        
        ImVec2 size = cfg.panel_size;
        if (size.x <= 0.0f) size.x = ImGui::CalcItemWidth();
        if (size.y <= 0.0f) size.y = height;

        ImGui::BeginChild(u8"##AuthPanel", size, true);

        ImGui::TextUnformatted(cfg.header);
        ImGui::Separator();
        
        if (cfg.inputs_fill_width) ImGui::PushItemWidth(-FLT_MIN);

        // Host
        if (cfg.show_host) {
            bool changed = false;
            if (cfg.vk_host) {
                changed = InputTextWithVKValidated(
                    u8"##host", cfg.hint_host,
                    data.host, cfg.validate_host, InputValidatePolicy::OnTouch,
                    cfg.host_regex, data.host_valid,
                    cfg.error_color,
                    /*extra_flags*/0, /*cb*/nullptr, /*ud*/nullptr,
                    cfg.kb_cfg, cfg.vk_cfg
                );
            } else {
                changed = InputTextValidated(
                    u8"##host", cfg.hint_host,
                    data.host, cfg.validate_host, InputValidatePolicy::OnTouch,
                    cfg.host_regex, data.host_valid, cfg.error_color
                );
            }
            if (changed) res |= AuthPanelResult::HostChanged;
        }

        // Token
        if (cfg.show_token) {
            bool changed = false;
            if (cfg.vk_token) {
                changed = InputPasswordWithToggleVK(
                    u8"##token", cfg.hint_token,
                    data.token, cfg.validate_token, InputValidatePolicy::OnTouch,
                    cfg.token_regex, data.token_valid,
                    cfg.toggle_cfg, cfg.kb_cfg, cfg.vk_cfg,
                    cfg.error_color
                );
            } else {
                changed = InputPasswordWithToggle(
                    u8"##token", cfg.hint_token,
                    data.token, cfg.validate_token, InputValidatePolicy::OnTouch,
                    cfg.token_regex, data.token_valid,
                    cfg.toggle_cfg, cfg.error_color
                );
            }
            if (changed) res |= AuthPanelResult::TokenChanged;
        }

        // API keys
        if (cfg.show_api_keys) {
            // public (usually not masked, but we keep the same UI for symmetry)
            {
                bool changed = false;
                if (cfg.vk_api_key) {
                    changed = InputPasswordWithToggleVK(
                        u8"##api_key", cfg.hint_api_key,
                        data.api_key, cfg.validate_api_key, InputValidatePolicy::OnTouch,
                        cfg.api_key_regex, data.api_key_valid,
                        cfg.toggle_cfg, cfg.kb_cfg, cfg.vk_cfg,
                        cfg.error_color
                    );
                } else {
                    changed = InputPasswordWithToggle(
                        u8"##api_key", cfg.hint_api_key,
                        data.api_key, cfg.validate_api_key, InputValidatePolicy::OnTouch,
                        cfg.api_key_regex, data.api_key_valid,
                        cfg.toggle_cfg, cfg.error_color
                    );
                }
                if (changed) res |= AuthPanelResult::ApiKeyChanged;
            }
            // secret
            {
                bool changed = false;
                if (cfg.vk_api_secret) {
                    changed = InputPasswordWithToggleVK(
                        u8"##api_secret", cfg.hint_api_secret,
                        data.api_secret, cfg.validate_api_secret, InputValidatePolicy::OnTouch,
                        cfg.api_secret_regex, data.api_secret_valid,
                        cfg.toggle_cfg, cfg.kb_cfg, cfg.vk_cfg,
                        cfg.error_color
                    );
                } else {
                    changed = InputPasswordWithToggle(
                        u8"##api_secret", cfg.hint_api_secret,
                        data.api_secret, cfg.validate_api_secret, InputValidatePolicy::OnTouch,
                        cfg.api_secret_regex, data.api_secret_valid,
                        cfg.toggle_cfg, cfg.error_color
                    );
                }
                if (changed) res |= AuthPanelResult::ApiSecretChanged;
            }
        }

        // Email
        if (cfg.show_email) {
            bool changed = false;
            if (cfg.vk_email) {
                changed = InputTextWithVKValidated(
                    u8"##email", cfg.hint_email,
                    data.email, cfg.validate_email, InputValidatePolicy::OnTouch,
                    cfg.email_regex, data.email_valid,
                    cfg.error_color,
                    /*extra_flags*/0, /*cb*/nullptr, /*ud*/nullptr,
                    cfg.kb_cfg, cfg.vk_cfg
                );
            } else {
                changed = InputTextValidated(
                    u8"##email", cfg.hint_email,
                    data.email, cfg.validate_email, InputValidatePolicy::OnTouch,
                    cfg.email_regex, data.email_valid, cfg.error_color
                );
            }
            if (changed) res |= AuthPanelResult::EmailChanged;

            // Optional email state pill
            if (cfg.use_email_state) {
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_CheckMark, cfg.email_state_color);
                ImGui::RadioButton(cfg.email_state_label.empty() ? u8"state" : cfg.email_state_label.c_str(),
                                   cfg.email_state);
                ImGui::PopStyleColor();
            }
        }

        // Password
        if (cfg.show_password) {
            bool changed = false;
            if (cfg.vk_password) {
                changed = InputPasswordWithToggleVK(
                    u8"##password", cfg.hint_password,
                    data.password, cfg.validate_password, InputValidatePolicy::OnTouch,
                    cfg.password_regex, data.password_valid,
                    cfg.toggle_cfg, cfg.kb_cfg, cfg.vk_cfg,
                    cfg.error_color
                );
            } else {
                changed = InputPasswordWithToggle(
                    u8"##password", cfg.hint_password,
                    data.password, cfg.validate_password, InputValidatePolicy::OnTouch,
                    cfg.password_regex, data.password_valid,
                    cfg.toggle_cfg, cfg.error_color
                );
            }
            if (changed) res |= AuthPanelResult::PasswordChanged;
        }

        // Connection button
        if (cfg.show_connect_button) {
            ImGui::Separator();
            // Connection state + action
            if (cfg.show_connection_state) {
                if (cfg.init) {
                    ImGui::PushStyleColor(ImGuiCol_CheckMark,
                        cfg.connected ? ImVec4(0.14f, 0.8f, 0.27f, 1.0f)
                                      : ImVec4(0.78f, 0.14f, 0.14f, 1.0f));
                    ImGui::RadioButton(cfg.connected_label, true);
                    ImGui::PopStyleColor();
                } else {
                    ImGui::RadioButton(cfg.connected_label, false);
                }
                ImGui::SameLine();
            }
            if (ImGui::Button(cfg.connect_label)) {
                if (cfg.on_connect) cfg.on_connect();
                res |= AuthPanelResult::ConnectClicked;
            }
        }

        if (cfg.inputs_fill_width) ImGui::PopItemWidth();

        ImGui::EndChild();
        ImGui::PopID();
        return res;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_AUTH_PANEL_HPP_INCLUDED
