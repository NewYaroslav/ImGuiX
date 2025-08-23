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
#include "icon_button.hpp"
#include "virtual_keyboard.hpp"

namespace ImGuiX::Widgets {

    /// \brief Bitmask result for AuthPanel events.
    enum class AuthPanelResult : unsigned {
        None            = 0u,
        EmailChanged    = 1u << 0,
        PasswordChanged = 1u << 1,
        HostChanged     = 1u << 2,
        ConnectClicked  = 1u << 3
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
        const char* tooltip_show  = u8"Show password";
        const char* tooltip_hide  = u8"Hide password";
        const char* text_label    = u8"Show";      ///< Текст для чекбокса, если use_icon == false
        
        ImFont*     icon_font     = nullptr;     ///< шрифт с иконками (если null — текущий)
        float       icon_baseline = 5.0f;        ///< сдвиг по Y в пикселях (подгонка базлайна)
        float       icon_rounding = -1.0f;       ///< скругление фона кнопки
    };
	
	struct VKButtonConfig {
		bool        enabled_host      = false;     ///< show VK button near host
		bool        enabled_email     = false;     ///< show VK button near email
		bool        enabled_password  = false;     ///< show VK button near password

		bool        use_icon          = true;      ///< icon or text
		const char* icon_text         = u8"\uE312";///< default Material PUA 'keyboard' (alt: u8"\uE23E")
		const char* text              = u8"[KB]";    ///< text label if use_icon==false
		ImFont*     icon_font         = nullptr;   ///< icon font (merged or dedicated)
		ImVec2      button_size       = ImVec2(0,0);
		float       same_line_w       = 0.0f;      ///< SameLine(offset) before button
		float       icon_baseline     = 4.0f;      ///< Y offset to fit baseline
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

        // Options
        bool show_host              = false;
        bool show_connect_button    = true;
        bool show_connection_state  = true;
        bool show_password_toggle   = true;  ///< show "Show password" checkbox
        bool validate_email         = true;  ///< apply simple regex validation
        
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
        const char* email_regex = u8R"(.+@.+\.\w+)";
    };

    /// \brief Renders login form. Optionally includes host input.
    /// \param id Unique widget ID.
    /// \param cfg Panel configuration.
    /// \param email Reference to email string.
    /// \param password Reference to password string.
    /// \param host Optional pointer to host string (used only if cfg.show_host is true).
    /// \param email_valid_out Optional output: whether email matches regex (if validate_email).
    /// \return Bitmask of events happened this frame.
    inline AuthPanelResult AuthPanel(const char* id,
                                     AuthPanelConfig& cfg,
                                     std::string& email,
                                     std::string& password,
                                     std::string* host = nullptr,
                                     bool* email_valid_out = nullptr) {
		AuthPanelResult res = AuthPanelResult::None;
        ImGui::PushID(id);
        
        const ImGuiID key_show_pwd = ImGui::GetID(u8"show_password");
        ImGuiStorage* st = ImGui::GetStateStorage();

		//--
		enum class VKTarget : int { None=0, Host=1, Email=2, Password=3 };

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
		if (vk_target == VKTarget::Email) vk_text_ptr = &email;
		else if (vk_target == VKTarget::Password) vk_text_ptr = &password;
		else if (vk_target == VKTarget::Host && host) vk_text_ptr = host;
		
		//---

        auto get_show = [&]() -> bool {
            return st->GetInt(key_show_pwd, 0) != 0;
        };
        auto set_show = [&](bool v) {
            st->SetInt(key_show_pwd, v ? 1 : 0);
        };

        float height = 0.0f;
        height += ImGui::GetTextLineHeightWithSpacing();     // заголовок
        height += 2.0f * ImGui::GetFrameHeightWithSpacing(); // password
        if(cfg.password_toggle.enabled) {
            height = std::max(height, ImGui::GetFrameHeightWithSpacing()); // кнопка/чекбокс сбоку
        }
        if(cfg.show_host) {
            height += ImGui::GetFrameHeightWithSpacing();
        }
        if(cfg.show_connection_state) {
            height += ImGui::GetFrameHeightWithSpacing();
        }
        height += ImGui::GetFrameHeightWithSpacing(); // кнопка Connect
        ImGui::BeginChild(u8"##AuthPanel", ImVec2(ImGui::GetWindowWidth() * 0.65f, height), true);

        ImGui::Text(u8"%s", cfg.header.c_str());
        ImGui::Separator();

        // Host
        if (cfg.show_host) {
            char buf[512];
            std::strncpy(buf, (host ? host->c_str() : u8""), sizeof(buf));
            buf[sizeof(buf)-1] = '\0';
            if (ImGui::InputTextWithHint(u8"##host", cfg.hint_host.c_str(), buf, sizeof(buf)-1)) {
                if (host) { *host = buf; res |= AuthPanelResult::HostChanged; }
            }
			
			if (cfg.vk.enabled_host) {
				if (draw_kb_button(u8"##vk_host")) {
					toggle_vk_for(VKTarget::Host);
				}
			}
        }

        // Email
        bool email_valid = true;
        if (cfg.validate_email) {
            try {
                std::regex re(cfg.email_regex);
                email_valid = std::regex_match(email, re);
            } catch (...) { /* ignore invalid regex */ }
        }
		
		bool pushed_invalid_color = false;
        if (cfg.validate_email && !email_valid) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.5f, 0.5f, 1.0f));
			pushed_invalid_color = true;
        }

        {
            char buf[512];
            std::strncpy(buf, email.c_str(), sizeof(buf));
            buf[sizeof(buf)-1] = '\0';
            if (ImGui::InputTextWithHint(u8"##email", cfg.hint_email.c_str(), buf, sizeof(buf)-1)) {
                email = buf;
                res |= AuthPanelResult::EmailChanged;
                // revalidate quickly
                if (cfg.validate_email) {
                    try {
                        std::regex re(cfg.email_regex);
                        email_valid = std::regex_match(email, re);
                    } catch (...) { email_valid = true; }
                }
            }
			if (cfg.vk.enabled_email) {
				if (draw_kb_button(u8"##vk_email")) {
					toggle_vk_for(VKTarget::Email);
				}
			}
        }

        if (pushed_invalid_color) ImGui::PopStyleColor();

        // Optional email state pill
        if (cfg.use_email_state) {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_CheckMark, cfg.email_state_color);
            ImGui::RadioButton(cfg.email_state_label.empty() ? u8"state" : cfg.email_state_label.c_str(),
                               cfg.email_state);
            ImGui::PopStyleColor();
        }

        // Password
        bool show_password = get_show();
        int flags = show_password ? 0 : ImGuiInputTextFlags_Password;
        {
            char buf[512];
            std::strncpy(buf, password.c_str(), sizeof(buf));
            buf[sizeof(buf)-1] = '\0';
            if (ImGui::InputTextWithHint(u8"##password", cfg.hint_password.c_str(), buf, sizeof(buf)-1, flags)) {
                password = buf;
                res |= AuthPanelResult::PasswordChanged;
            }
			if (cfg.vk.enabled_password) {
				if (draw_kb_button(u8"##vk_password")) {
					toggle_vk_for(VKTarget::Password);
				}
			}
        }
        if (cfg.password_toggle.enabled) {
            ImGui::SameLine();

            if (cfg.password_toggle.use_icon) {
                // Подберём размер: квадрат по высоте текущего фрейма
				/*
                ImVec2 sz = cfg.password_toggle.button_size;
                if (sz.x <= 0.0f || sz.y <= 0.0f) {
                    float h = ImGui::GetFrameHeight();
                    sz = ImVec2(h, h);
                }
				*/
				
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
					default: break;
				}
			};

			// Если эмбед — просто под формой
			if (!cfg.vk.vk_as_overlay) {
				std::string& ref = *vk_text_ptr;
				ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y));
				ImGui::Separator();
				ImGui::TextUnformatted(u8"Virtual keyboard:");
				ImGuiX::Widgets::VirtualKeyboard(u8"##vk_embed", ref, vkcfg);
			} else {
				// ОВЕРЛЕЙ: поверх всех окон. Рендерим как отдельное окно ближе к концу кадра.
				const ImGuiID key_vk_was_visible = ImGui::GetID(u8"vk_was_visible");
				bool vk_visible_now  = get_vk_visible();
				bool vk_visible_prev = (st->GetInt(key_vk_was_visible, 0) != 0);
				bool vk_just_opened  = vk_visible_now && !vk_visible_prev;

				// If just opened, request focus once
				if (vk_just_opened) {
					ImGui::SetNextWindowFocus();
				}
				
				const ImGuiViewport* vp = ImGui::GetMainViewport();
				ImVec2 center = vp->GetCenter();
				if (cfg.vk.overlay_size.x > 0.0f && cfg.vk.overlay_size.y > 0.0f) {
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
					ImGui::SetNextWindowSize(cfg.vk.overlay_size, ImGuiCond_Appearing);
				} else {
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				}
				
				bool should_close_on_blur = false; // computed inside, then acted after End()
				
				ImGui::SetNextWindowBgAlpha(0.98f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
				if (ImGui::Begin(u8"##vk_overlay_window",
								 nullptr,
								 ImGuiWindowFlags_NoTitleBar |
								 ImGuiWindowFlags_NoResize |
								 ImGuiWindowFlags_NoCollapse |
								 ImGuiWindowFlags_NoSavedSettings |
								 //ImGuiWindowFlags_NoDocking |
								 ImGuiWindowFlags_AlwaysAutoResize))
				{
					// Render VK as usual
					std::string& ref = *vk_text_ptr;
					bool mod = ImGuiX::Widgets::VirtualKeyboard(u8"##vk_overlay", ref, vkcfg);
					
					// Close on ESC
					if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
						should_close_on_blur = true;
					}
					
					// Close when focus leaves the overlay (but not on the very first frame)
					const bool focused_now = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
					const bool appearing   = ImGui::IsWindowAppearing();
					if (!focused_now && !appearing) {
						should_close_on_blur = true;
					}
				}
				ImGui::End();
				ImGui::PopStyleVar();
				
				// Apply close if needed
				if (should_close_on_blur) {
					set_vk_visible(false);
					set_vk_target(VKTarget::None);
				}
				
				// Update "was visible" flag for next frame
				st->SetInt(key_vk_was_visible, get_vk_visible() ? 1 : 0);
			}
		}

        if (email_valid_out) *email_valid_out = email_valid;

        ImGui::PopID();
        return res;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_AUTH_PANEL_HPP_INCLUDED
