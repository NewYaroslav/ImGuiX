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
#include <imguix/config/fonts.hpp>
#include "icon_button.hpp"
#include "validated_input.hpp"
#include "virtual_keyboard_overlay.hpp"

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
    
    inline AuthPanelResult& operator|=(AuthPanelResult& a, AuthPanelResult b) { a = a | b; return a; }

    /// \brief Параметры переключателя "показать пароль".
    struct PasswordToggleConfig {
        bool        enabled       = true;        ///< Включить переключатель
        bool        use_icon      = false;       ///< true: рисовать как кнопку-иконку; false: чекбокс "Show"
        const char* icon_show     = u8"\ue8f4";  ///< «глаз» (зависит от шрифта с иконками)
        const char* icon_hide     = u8"\ue8f5";  ///< «глаз перечёркнутый»
        ImVec2      button_size   = ImVec2(0,0); ///< (0,0) — авто (по высоте текущего фрейма)
        float       same_line_w   = 0.0f;        ///< Отступ перед кнопкой (SameLine(offset))
        const char* tooltip_show  = u8"Show password"; ///<
        const char* tooltip_hide  = u8"Hide password"; ///<
        const char* text_label    = u8"Show";    ///< Текст для чекбокса, если use_icon == false
        
        ImFont*     icon_font     = nullptr;     ///< шрифт с иконками (если null — текущий)
        float       icon_baseline = 0.0f;        ///< сдвиг по Y в пикселях (подгонка базлайна)
        float       icon_rounding = -1.0f;       ///< скругление фона кнопки
    };
    
    /// \brief Параметры переключателя "показать пароль".
    struct VKButtonConfig {
        bool        enabled_host      = false;      ///< show VK button near host
        bool        enabled_email     = false;      ///< show VK button near email
        bool        enabled_password  = false;      ///< show VK button near password
        bool        enabled_token     = false;      ///< show VK near token
        bool        enabled_api_key   = false;      ///< show VK near API key (public)
        bool        enabled_api_secret= false;      ///< show VK near API secret

        bool        use_icon          = true;       ///< icon or text
        const char* icon_text         = u8"\uE312"; ///< default Material PUA 'keyboard' (alt: u8"\uE23E")
        const char* text              = u8"[KB]";   ///< text label if use_icon==false
        ImFont*     icon_font         = nullptr;   ///< icon font (merged or dedicated)
        ImVec2      button_size       = ImVec2(0,0);
        float       same_line_w       = 0.0f;      ///< SameLine(offset) before button
        float       icon_baseline     = 0.0f;      ///< Y offset to fit baseline
        float       icon_rounding     = -1.0f;     ///< bg rounding, -1 => default
        const char* tooltip_toggle_on = u8"Show keyboard";
        const char* tooltip_toggle_off= u8"Hide keyboard";

        // VK render mode
        bool        vk_as_overlay     = true;      ///< true: floating overlay window; false: embed under form
        ImVec2      overlay_size      = ImVec2(0,0); ///< 0,0 => auto
    };

    /// \brief Configuration for AuthPanel.
    struct AuthPanelConfig {
        // Labels
        std::string header          = u8"Authorization";
        std::string hint_email      = u8"email";
        std::string hint_password   = u8"password";
        std::string hint_host       = u8"host";
        std::string connected_label = u8"connected";
        std::string connect_label   = u8"connect";
        
        // Tokens / API keys
        std::string hint_token      = u8"token";
        std::string hint_api_key    = u8"api key (public)";
        std::string hint_api_secret = u8"api secret";

        // Options
        bool show_host             = false;
        bool show_email            = true;
        bool show_password         = true;
        bool show_token            = false;
        bool show_api_keys         = false; ///< public + secret
        bool mask_api_secret       = true;  ///< show secret as password
        bool show_connect_button   = true;
        bool show_connection_state = true;
        bool show_password_toggle  = true;  ///< show "Show password" checkbox
        
        bool validate_email        = true;  ///< apply simple regex validation
        bool validate_password     = false;
        bool validate_host         = false;
        bool validate_token        = false;
        bool validate_api_key      = false;
        bool validate_api_secret   = false;

        // Password toggle
        PasswordToggleConfig password_toggle{};
        
        ImGuiX::Widgets::VirtualKeyboardConfig vk_cfg{};
        VKButtonConfig vk{};

        // Connection state visuals
        bool init                   = false;
        bool connected              = false;

        // Email state pill on the right (optional)
        bool        use_email_state = false;
        bool        email_state     = false;
        ImVec4      email_state_color = ImVec4(0,0,0,1);
        std::string email_state_label;

        // Callbacks
        std::function<void()> on_connect;

        // Email regex (basic)
        const char* email_regex      = u8R"(.+@.+\.\w+)";
        const char* password_regex   = u8R"(.{6,})";              ///< пример: минимум 6 символов
        const char* host_regex       = u8R"(^[A-Za-z0-9.\-:]+$)"; ///< проверка хост:порт/домен
        const char* token_regex      = u8R"(^\S+$)";              ///< пример: без пробелов, не пусто
        const char* api_key_regex    = u8R"(^\S+$)";
        const char* api_secret_regex = u8R"(^\S+$)";
        
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
    /// \param data All input/output fields (email/password/host/token/api keys) and validity flags (set inside).
    /// \return Bitmask of events happened this frame.
    inline AuthPanelResult AuthPanel(
            const char* id,
            AuthPanelConfig& cfg,
            AuthData& data
        ) {
        AuthPanelResult res = AuthPanelResult::None;
        ImGui::PushID(id);
        
        const ImGuiID key_show_pwd = ImGui::GetID(u8"show_password");
        ImGuiStorage* st = ImGui::GetStateStorage();

        //--
        enum class VKTarget : int { None=0, Host=1, Email=2, Password=3, Token=4, ApiKey=5, ApiSecret=6 };

        const ImGuiID key_vk_visible = ImGui::GetID(u8"vk_visible");
        const ImGuiID key_vk_target  = ImGui::GetID(u8"vk_target");

        auto get_vk_visible = [&](){ return st->GetInt(key_vk_visible, 0) != 0; };
        auto set_vk_visible = [&](bool v){ st->SetInt(key_vk_visible, v ? 1 : 0); };
        auto get_vk_target  = [&](){ return (VKTarget)st->GetInt(key_vk_target, (int)VKTarget::None); };
        auto set_vk_target  = [&](VKTarget t){ st->SetInt(key_vk_target, (int)t); };

        // helper: toggle for a field
        auto toggle_vk_for = [&](VKTarget t){
            bool vis = get_vk_visible();
            VKTarget cur = get_vk_target();
            if (vis && cur == t) { set_vk_visible(false); set_vk_target(VKTarget::None); }
            else { set_vk_target(t); set_vk_visible(true); }
        };

        // helper: draw small KB button (icon or text)
        auto draw_kb_button = [&](const char* idlabel)->bool {
            if (cfg.vk.same_line_w != 0.0f) ImGui::SameLine(cfg.vk.same_line_w);
            else ImGui::SameLine();

            bool pressed = false;
            if (cfg.vk.use_icon) {
                ImGuiX::Widgets::IconButtonConfig ib{};
                ib.font        = cfg.vk.icon_font;
                ib.text_offset = ImVec2(0, cfg.vk.icon_baseline);
                ib.rounding    = cfg.vk.icon_rounding;
                pressed = ImGuiX::Widgets::IconButtonCentered(idlabel, 
                            cfg.vk.icon_text ? cfg.vk.icon_text : u8"\uE312", ib);
            } else {
                pressed = ImGui::Button(idlabel, cfg.vk.button_size);
            }
            // tooltip reflects next state
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(u8"%s", get_vk_visible() ? 
                    (cfg.vk.tooltip_toggle_off?cfg.vk.tooltip_toggle_off:u8"Hide keyboard"):
                    (cfg.vk.tooltip_toggle_on? cfg.vk.tooltip_toggle_on :u8"Show keyboard"));
            return pressed;
        };

        // ТЕКУЩАЯ ЦЕЛЬ ДЛЯ ВК + указатель на текст
        VKTarget vk_target = get_vk_target();
        std::string* vk_text_ptr = nullptr;
        if (vk_target == VKTarget::Email) vk_text_ptr = &data.email;
        else if (vk_target == VKTarget::Password) vk_text_ptr = &data.password;
        else if (vk_target == VKTarget::Host) vk_text_ptr = &data.host;
        else if (vk_target == VKTarget::Token) vk_text_ptr = &data.token;
        else if (vk_target == VKTarget::ApiKey) vk_text_ptr = &data.api_key;
        else if (vk_target == VKTarget::ApiSecret) vk_text_ptr = &data.api_secret;
        
        //---

        auto get_show = [&]() -> bool {
            return st->GetInt(key_show_pwd, 0) != 0;
        };
        auto set_show = [&](bool v) {
            st->SetInt(key_show_pwd, v ? 1 : 0);
        };

        float height = 0.0f;
        height += ImGui::GetTextLineHeightWithSpacing();
        if(cfg.password_toggle.enabled) {
            height = std::max(height, ImGui::GetFrameHeightWithSpacing()); // кнопка/чекбокс сбоку
        }
        if(cfg.show_host) {
            height += ImGui::GetFrameHeightWithSpacing();
        }
        if(cfg.show_email) {
            height += ImGui::GetFrameHeightWithSpacing();
        }
        if(cfg.show_password) {
            height += ImGui::GetFrameHeightWithSpacing();
        }
        if(cfg.show_connect_button) {
            height += ImGui::GetFrameHeightWithSpacing();
        }
        if(cfg.show_token) {
            height += ImGui::GetFrameHeightWithSpacing();
        }
        if(cfg.show_api_keys) { 
            height += 2.0f * ImGui::GetFrameHeightWithSpacing(); 
        }

        height += ImGui::GetFrameHeightWithSpacing(); // кнопка Connect
    
        ImGui::BeginChild(u8"##AuthPanel", ImVec2(ImGui::GetWindowWidth() * 0.65f, height), true);

        ImGui::Text(u8"%s", cfg.header.c_str());
        ImGui::Separator();

        // Host
        if (cfg.show_host) {
            if (InputTextValidated(u8"##host", cfg.hint_host.c_str(), 
                data.host, cfg.validate_host, InputValidatePolicy::OnTouch,
                cfg.host_regex, data.host_valid, cfg.error_color)) {
                res |= AuthPanelResult::HostChanged;
            }
            if (cfg.vk.enabled_host && draw_kb_button(u8"##vk_host")) {
                toggle_vk_for(VKTarget::Host);
            }
        }
        
        // Token
        if (cfg.show_token) {
            if (InputTextValidated(u8"##token", cfg.hint_token.c_str(),
                    data.token, cfg.validate_token, InputValidatePolicy::OnTouch,
                    cfg.token_regex, data.token_valid, cfg.error_color)) {
                res |= AuthPanelResult::TokenChanged;
            }
            if (cfg.vk.enabled_token && draw_kb_button(u8"##vk_token")) {
                toggle_vk_for(VKTarget::Token);
            }
        }
        
        // API keys
        if (cfg.show_api_keys) {
            // public
            if (InputTextValidated(u8"##api_key", cfg.hint_api_key.c_str(),
                    data.api_key, cfg.validate_api_key, InputValidatePolicy::OnTouch,
                    cfg.api_key_regex, data.api_key_valid, cfg.error_color)) {
                res |= AuthPanelResult::ApiKeyChanged;
            }
            if (cfg.vk.enabled_api_key && draw_kb_button(u8"##vk_api_key")) {
                toggle_vk_for(VKTarget::ApiKey);
            }
            // secret (masked if enabled)
            if (InputTextValidated(u8"##api_secret", cfg.hint_api_secret.c_str(),
                    data.api_secret, cfg.validate_api_secret, InputValidatePolicy::OnTouch,
                    cfg.api_secret_regex, data.api_secret_valid, cfg.error_color)) {
                res |= AuthPanelResult::ApiSecretChanged;
            }
            if (cfg.vk.enabled_api_secret && draw_kb_button(u8"##vk_api_secret")) {
                toggle_vk_for(VKTarget::ApiSecret);
            }
        }

        // Email
        if (cfg.show_email) {
            if (InputTextValidated(u8"##email", cfg.hint_email.c_str(),
                    data.email, cfg.validate_email, InputValidatePolicy::OnTouch,
                    cfg.email_regex, data.email_valid, cfg.error_color)) {
                res |= AuthPanelResult::EmailChanged;
            }
            if (cfg.vk.enabled_email && draw_kb_button(u8"##vk_email")) {
                toggle_vk_for(VKTarget::Email);
            }
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
            bool show_password = get_show();
            int flags = show_password ? 0 : ImGuiInputTextFlags_Password;
            
            if (InputTextValidated(u8"##password", cfg.hint_password.c_str(),
                    data.password, cfg.validate_password, InputValidatePolicy::OnTouch,
                    cfg.password_regex, data.password_valid, cfg.error_color, flags)) {
                res |= AuthPanelResult::PasswordChanged;
            }
            if (cfg.vk.enabled_password && draw_kb_button(u8"##vk_password")) {
                toggle_vk_for(VKTarget::Password);
            }
            if (cfg.password_toggle.enabled) {
                ImGui::SameLine();
                if (cfg.password_toggle.use_icon) {
                    ImGuiX::Widgets::IconButtonConfig eye{};
                    eye.font        = cfg.password_toggle.icon_font;
                    eye.text_offset = ImVec2(0, cfg.password_toggle.icon_baseline);
                    eye.rounding    = cfg.password_toggle.icon_rounding;
                
                    const char* icon = show_password ? 
                        (cfg.password_toggle.icon_hide ? cfg.password_toggle.icon_hide : u8"X") : 
                        (cfg.password_toggle.icon_show ? cfg.password_toggle.icon_show : u8"V");
                    if (IconButtonCentered(u8"##pwd_eye", icon, eye)) {
                        show_password = !show_password;
                        set_show(show_password);
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip(u8"%s", show_password ? (cfg.password_toggle.tooltip_hide ? cfg.password_toggle.tooltip_hide : u8"Hide")
                                                              : (cfg.password_toggle.tooltip_show ? cfg.password_toggle.tooltip_show : u8"Show"));
                    }
                } else {
                    if (ImGui::Checkbox(cfg.password_toggle.text_label, &show_password)) {
                        set_show(show_password);
                    }
                }
            }
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
                    ImGui::RadioButton(cfg.connected_label.c_str(), true);
                    ImGui::PopStyleColor();
                } else {
                    ImGui::RadioButton(cfg.connected_label.c_str(), false);
                }
                ImGui::SameLine();
            }
            if (ImGui::Button(cfg.connect_label.c_str())) {
                if (cfg.on_connect) cfg.on_connect();
                res |= AuthPanelResult::ConnectClicked;
            }
        }

        ImGui::EndChild();
        
        if (get_vk_visible() && vk_text_ptr) {
            // Скопируем cfg.vk_cfg и обернём on_submit, чтобы закрыть клаву
            ImGuiX::Widgets::VirtualKeyboardConfig vkcfg = cfg.vk_cfg;

            // Compose on_submit
            auto user_submit = vkcfg.on_submit; // may be empty
            vkcfg.on_submit = [&, user_submit](const std::string& s){
                if (user_submit) user_submit(s);
                // close VK on submit
                set_vk_visible(false);
                set_vk_target(VKTarget::None);
                // вернуть фокус на поле (не строго, но помогает UX)
                switch (vk_target) {
                    case VKTarget::Email:    ImGui::SetKeyboardFocusHere(-1); break;
                    case VKTarget::Password: ImGui::SetKeyboardFocusHere(-1); break;
                    case VKTarget::Host:     ImGui::SetKeyboardFocusHere(-1); break;
                    case VKTarget::Token:    ImGui::SetKeyboardFocusHere(-1); break;
                    case VKTarget::ApiKey:   ImGui::SetKeyboardFocusHere(-1); break;
                    case VKTarget::ApiSecret:ImGui::SetKeyboardFocusHere(-1); break;
                    default: break;
                }
            };

            // Если эмбед — просто под формой
            if (!cfg.vk.vk_as_overlay) {
                std::string& ref = *vk_text_ptr;
                ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y));
                ImGui::Separator();
                ImGui::TextUnformatted(u8"Virtual keyboard:");
                (void)ImGuiX::Widgets::VirtualKeyboard(u8"##vk_embed", ref, vkcfg);
            } else {
                // overlay: окно и автозакрытие
				bool vis = true; // уже знаем, что get_vk_visible() == true
				std::string& ref = *vk_text_ptr;
				(void)ImGuiX::Widgets::VirtualKeyboardOverlay(
					u8"##vk_overlay_window", ref, vkcfg, &vis, cfg.vk.overlay_size
				);
				if (!vis) { set_vk_visible(false); set_vk_target(VKTarget::None); }
            }
        }

        ImGui::PopID();
        return res;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_AUTH_PANEL_HPP_INCLUDED
