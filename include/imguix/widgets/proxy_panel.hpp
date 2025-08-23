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
    };

    /// \brief UI configuration for ProxyPanel.
    struct ProxyPanelConfig {
        // Labels
        const char* header            = "Proxy settings";
        const char* hint_ip           = "ip";
        const char* hint_port         = "port";
        const char* hint_user         = "user";
        const char* hint_pass         = "password";
        const char* label_use_proxy   = "use proxy";
        const char* button_check      = "check proxy";
        const char* label_checked     = "checked";

        // Options
        bool show_type                = true;   ///< show type combo (HTTP/SOCKS)
        bool show_check               = true;   ///< show "check proxy" button + status
        bool checked                  = false;  ///< result of last check (visual only)
        bool check_ok                 = false;  ///< last check OK (green) vs fail (red)

        // Callbacks
        std::function<void()> on_check = nullptr;

        // Style
        ImVec4 error_text_color       = ImVec4(0.9f, 0.5f, 0.5f, 1.0f);

        // Layout
        float field_width_host_user   = 160.0f;
        float field_width_port        = 70.0f;
        float field_width_type        = 110.0f;
        ImVec2 panel_size             = ImVec2(0, 0); ///< 0→auto width/height
        bool   border                 = true;
    };

    /// \brief Draw proxy settings panel.
    /// \return true if any settings changed this frame.
    inline bool ProxyPanel(const char* id, ProxyPanelConfig& cfg, ProxySettings& st) {
        bool changed = false;

        ImGui::PushID(id);
        const ImVec2 size = (cfg.panel_size.x <= 0.f || cfg.panel_size.y <= 0.f)
                          ? ImVec2(ImGui::GetWindowWidth() * 0.65f, 148.0f)
                          : cfg.panel_size;

        ImGui::BeginChild("##proxy_panel", size, cfg.border);
        ImGui::Text("%s", cfg.header ? cfg.header : "Proxy");
        ImGui::Separator();

        // Collapse inputs when proxy disabled (but still visible)
        ImGui::BeginDisabled(!st.use_proxy);

        // --- IP ---
        bool bad_ip = false;
        if (!st.ip.empty()) {
            // Minimal IPv4 check. For hostnames, расширь по необходимости.
            static const std::regex re_ip(R"(^\d{1,3}(\.\d{1,3}){3}$)");
            bad_ip = !std::regex_match(st.ip, re_ip);
        }

        if (bad_ip) ImGui::PushStyleColor(ImGuiCol_Text, cfg.error_text_color);

        ImGui::PushItemWidth(cfg.field_width_host_user);
        {
            char buf[256];
            std::strncpy(buf, st.ip.c_str(), sizeof(buf));
            buf[sizeof(buf)-1] = '\0';
            if (ImGui::InputTextWithHint("##proxy.ip", cfg.hint_ip, buf, sizeof(buf)-1,
                                         ImGuiInputTextFlags_AutoSelectAll)) {
                st.ip = buf;
                changed = true;
            }
        }
        ImGui::PopItemWidth();
        if (bad_ip) ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::TextUnformatted(":");

        // --- Port ---
        ImGui::SameLine();
        ImGui::PushItemWidth(cfg.field_width_port);
        {
            char pbuf[32];
            std::snprintf(pbuf, sizeof(pbuf), "%d", std::max(0, st.port));
            if (ImGui::InputTextWithHint("##proxy.port", cfg.hint_port, pbuf, sizeof(pbuf)-1,
                                         ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal)) {
                int v = 0;
                if (*pbuf) v = std::clamp(std::atoi(pbuf), 0, 65535);
                if (v != st.port) { st.port = v; changed = true; }
            }
        }
        ImGui::PopItemWidth();

        // --- Type ---
        if (cfg.show_type) {
            ImGui::SameLine();
            ImGui::PushItemWidth(cfg.field_width_type);
            int t = static_cast<int>(st.type);
            const char* items[] = { "HTTP", "SOCKS 4", "SOCKS 5" };
            if (ImGui::Combo("##proxy.type", &t, items, IM_ARRAYSIZE(items))) {
                st.type = static_cast<ProxyType>(t);
                changed = true;
            }
            ImGui::PopItemWidth();
        }

        // --- User ---
        ImGui::PushItemWidth(cfg.field_width_host_user);
        {
            char ubuf[256];
            std::strncpy(ubuf, st.username.c_str(), sizeof(ubuf));
            ubuf[sizeof(ubuf)-1] = '\0';
            if (ImGui::InputTextWithHint("##proxy.user", cfg.hint_user, ubuf, sizeof(ubuf)-1,
                                         ImGuiInputTextFlags_AutoSelectAll)) {
                st.username = ubuf;
                changed = true;
            }
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::TextUnformatted(":");

        // --- Pass ---
        ImGui::SameLine();
        ImGui::PushItemWidth(cfg.field_width_host_user);
        {
            char pbuf2[256];
            std::strncpy(pbuf2, st.password.c_str(), sizeof(pbuf2));
            pbuf2[sizeof(pbuf2)-1] = '\0';
            if (ImGui::InputTextWithHint("##proxy.pass", cfg.hint_pass, pbuf2, sizeof(pbuf2)-1,
                                         ImGuiInputTextFlags_Password)) {
                st.password = pbuf2;
                changed = true;
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
            // Прячем кнопку, если ip невалидный
            if (!bad_ip) {
                ImGui::SameLine();
                if (ImGui::Button(cfg.button_check)) {
                    if (cfg.on_check) cfg.on_check();
                }
            }

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

        ImGui::EndChild();
        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PROXY_PANEL_HPP_INCLUDED
