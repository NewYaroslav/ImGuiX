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
    inline bool ProxyPanel(const char* id, ProxyPanelConfig& cfg, ProxySettings& st) {
        bool changed = false;

        ImGui::PushID(id);
        const ImVec2 size = (cfg.panel_size.x <= 0.0f || cfg.panel_size.y <= 0.0f) ? 
            ImVec2(ImGui::CalcItemWidth(), 
                   ImGui::GetTextLineHeightWithSpacing() + 4.0f * ImGui::GetFrameHeightWithSpacing()) : 
            cfg.panel_size;
                          
        auto calc_w = [&](const char* s) -> float {
            // ширина текста + паддинги
            ImVec2 ts = ImGui::CalcTextSize(s, nullptr, false);
            return ts.x + 2.0f * ImGui::GetStyle().FramePadding.x;
        };

        ImGui::BeginChild(u8"##proxy_panel", size, cfg.border);
        ImGui::Text(u8"%s", cfg.header ? cfg.header : u8"Proxy");
        ImGui::Separator();
        
        if (cfg.inputs_fill_width) ImGui::PushItemWidth(-FLT_MIN);
        
        const ImGuiStyle& style = ImGui::GetStyle();
        float item_w = cfg.field_width_host_user > 0.0f ? 
            cfg.field_width_host_user : 
            0.5f * (ImGui::CalcItemWidth() - calc_w(u8":") - 2.0f * style.ItemSpacing.x);

        // Collapse inputs when proxy disabled (but still visible)
        ImGui::BeginDisabled(!st.use_proxy);

        // --- IP ---
        ImGui::PushItemWidth(item_w);
        {
            if (cfg.vk_ip) {
                changed = InputTextWithVKValidated(
                    u8"##proxy.ip", cfg.hint_ip,
                    st.ip, cfg.validate_ip, InputValidatePolicy::OnTouch,
                    cfg.ip_regex, st.ip_valid,
                    cfg.error_color,
                    ImGuiInputTextFlags_AutoSelectAll, /*cb*/nullptr, /*ud*/nullptr,
                    cfg.kb_cfg, cfg.vk_cfg
                );
            } else {
                changed = InputTextValidated(
                    u8"##proxy.ip", cfg.hint_ip,
                    st.ip, cfg.validate_ip, InputValidatePolicy::OnTouch,
                    cfg.ip_regex, st.ip_valid, cfg.error_color
                );
            }
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::TextUnformatted(u8":");

        // --- Port ---
        const float frame_h = ImGui::GetFrameHeight();
        const float spacing = style.ItemInnerSpacing.x;
        ImVec2 btn_sz = cfg.kb_cfg.button_size;
        if (btn_sz.x <= 0.0f || btn_sz.y <= 0.0f) {
            btn_sz = ImVec2(frame_h, frame_h);
        }
        const float port_w = cfg.field_width_port > 0.0f ? 
            cfg.field_width_port : 
            calc_w(u8"65535") + (cfg.vk_port ? (btn_sz.x + spacing) : 0.0f);

        ImGui::SameLine();
        ImGui::PushItemWidth(port_w);
        {
            std::string pbuf(std::to_string(std::max(0, st.port)));
            if (cfg.vk_port) {
                changed = InputTextWithVKValidated(
                    u8"##proxy.port", cfg.hint_port,
                    pbuf, cfg.validate_port, InputValidatePolicy::OnTouch,
                    cfg.port_regex, st.port_valid,
                    cfg.error_color,
                    ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal, 
                    /*cb*/nullptr, /*ud*/nullptr,
                    cfg.kb_cfg, cfg.vk_cfg
                );
            } else {
                changed = InputTextValidated(
                    u8"##proxy.port", cfg.hint_port,
                    pbuf, cfg.validate_port, InputValidatePolicy::OnTouch,
                    cfg.port_regex, st.port_valid, cfg.error_color
                );
            }
            if (changed) {
                int v = std::stoi(pbuf);
                v = std::clamp(v, 0, 65535);
                if (v != st.port) { st.port = v; changed = true; }
                else changed = false;
            }
        }
        ImGui::PopItemWidth();

        // --- Type ---
        if (cfg.show_type) {
            float type_w = cfg.field_width_type > 0.0f ? 
                cfg.field_width_type :
                item_w - port_w - 2.0f * spacing;
                //calc_w(u8"SOCKS 5") 
            type_w = std::max(calc_w(u8"SOCKS 5") + 2.0f * spacing, type_w);
            ImGui::SameLine();
            ImGui::PushItemWidth(type_w);
            int t = static_cast<int>(st.type);
            const char* items[] = { u8"HTTP", u8"SOCKS 4", u8"SOCKS 5" };
            if (ImGui::Combo(u8"##proxy.type", &t, items, IM_ARRAYSIZE(items))) {
                st.type = static_cast<ProxyType>(t);
                changed = true;
            }
            ImGui::PopItemWidth();
        }

        // --- User ---
        ImGui::PushItemWidth(item_w);
        {
            if (cfg.vk_username) {
                changed = InputTextWithVKValidated(
                    u8"##proxy.user", cfg.hint_username,
                    st.username, cfg.validate_username, InputValidatePolicy::OnTouch,
                    cfg.username_regex, st.username_valid,
                    cfg.error_color,
                    ImGuiInputTextFlags_AutoSelectAll, 
                    /*cb*/nullptr, /*ud*/nullptr,
                    cfg.kb_cfg, cfg.vk_cfg
                );
            } else {
                changed = InputTextValidated(
                    u8"##proxy.user", cfg.hint_username,
                    st.username, cfg.validate_username, InputValidatePolicy::OnTouch,
                    cfg.username_regex, st.username_valid, cfg.error_color
                );
            }
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::TextUnformatted(u8":");

        // --- Pass ---
        ImGui::SameLine();
        ImGui::PushItemWidth(item_w);
        {
            if (cfg.vk_password) {
                changed = InputPasswordWithToggleVK(
                    u8"##proxy.pass", cfg.hint_password,
                    st.password, cfg.validate_password, InputValidatePolicy::OnTouch,
                    cfg.password_regex, st.password_valid,
                    cfg.toggle_cfg, cfg.kb_cfg, cfg.vk_cfg,
                    cfg.error_color
                );
            } else {
                changed = InputPasswordWithToggle(
                    u8"##proxy.pass", cfg.hint_password,
                    st.password, cfg.validate_password, InputValidatePolicy::OnTouch,
                    cfg.password_regex, st.password_valid,
                    cfg.toggle_cfg, cfg.error_color
                );
            }
        }
        ImGui::PopItemWidth();

        ImGui::EndDisabled();

        // --- Use proxy checkbox ---
        if (ImGui::Checkbox(cfg.label_use_proxy, &st.use_proxy)) {
            changed = true;
        }

        // --- Check button + status ---
        if (cfg.show_check && st.use_proxy) {
            ImGui::SameLine();
            // Hide button if IP is invalid
            bool disabled = !st.ip_valid || 
                            !st.port_valid || 
                            !st.password_valid || 
                            !st.username_valid;

            if (disabled) ImGui::BeginDisabled();
            if (ImGui::Button(cfg.button_check)) {
                if (!disabled && cfg.on_check) cfg.on_check();
            }
            if (disabled) ImGui::EndDisabled();

            ImGui::SameLine();
            if (cfg.checked) {
                ImGui::PushStyleColor(ImGuiCol_CheckMark,
                    cfg.check_ok ? ImVec4(0.14f, 0.8f, 0.27f, 1.0f)
                                 : ImVec4(0.78f, 0.14f, 0.14f, 1.0f));
                ImGui::RadioButton(cfg.label_checked, true);
                ImGui::PopStyleColor();
            } else {
                ImGui::RadioButton(cfg.label_checked, false);
            }
        }
        
        if (cfg.inputs_fill_width) ImGui::PopItemWidth();

        ImGui::EndChild();
        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PROXY_PANEL_HPP_INCLUDED
