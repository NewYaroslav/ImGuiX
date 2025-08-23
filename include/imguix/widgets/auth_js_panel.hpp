#pragma once
#ifndef _IMGUIX_WIDGETS_AUTH_JS_PANEL_HPP_INCLUDED
#define _IMGUIX_WIDGETS_AUTH_JS_PANEL_HPP_INCLUDED

/// \file auth_js_panel.hpp
/// \brief Browser auth config panel: user-agent, accept-language, and DNT flag.

#include <imgui.h>
#include <string>
#include <cstring>

namespace ImGuiX::Widgets {

    /// \brief Mutable JS auth settings.
    struct AuthJsSettings {
        std::string user_agent;        ///< User-Agent header
        std::string accept_language;   ///< Accept-Language header
        bool        dnt = false;       ///< Do Not Track flag
    };

    /// \brief UI configuration for AuthJsPanel.
    struct AuthJsPanelConfig {
        std::string header              = u8"JS Config";
        std::string hint_user_agent     = u8"user agent";
        std::string hint_accept_language= u8"accept language";
        std::string label_dnt           = u8"Do Not Track";
        ImVec2      panel_size          = ImVec2(0, 0); ///< 0 → auto width/height
        bool        border              = true;
    };

    /// \brief Draw browser auth settings panel.
    /// \param id Unique widget identifier.
    /// \param cfg Panel configuration.
    /// \param st Settings model to edit.
    /// \return True if any field changed this frame.
    inline bool AuthJsPanel(const char* id, AuthJsPanelConfig& cfg, AuthJsSettings& st) {
        bool changed = false;
        ImGui::PushID(id);
        const ImVec2 size = (cfg.panel_size.x <= 0.f || cfg.panel_size.y <= 0.f)
                            ? ImVec2(ImGui::GetWindowWidth() * 0.65f, 128.0f)
                            : cfg.panel_size;

        ImGui::BeginChild(u8"##auth_js_panel", size, cfg.border);
        ImGui::Text(u8"%s", cfg.header.c_str());
        ImGui::Separator();

        char buf[512];

        std::strncpy(buf, st.user_agent.c_str(), sizeof(buf));
        buf[sizeof(buf) - 1] = '\0';
        if (ImGui::InputTextWithHint(u8"##authjs.user_agent",
                                     cfg.hint_user_agent.c_str(),
                                     buf,
                                     sizeof(buf) - 1,
                                     ImGuiInputTextFlags_AutoSelectAll)) {
            st.user_agent = buf;
            changed = true;
        }

        std::strncpy(buf, st.accept_language.c_str(), sizeof(buf));
        buf[sizeof(buf) - 1] = '\0';
        if (ImGui::InputTextWithHint(u8"##authjs.accept_lang",
                                     cfg.hint_accept_language.c_str(),
                                     buf,
                                     sizeof(buf) - 1,
                                     ImGuiInputTextFlags_AutoSelectAll)) {
            st.accept_language = buf;
            changed = true;
        }

        if (ImGui::Checkbox(cfg.label_dnt.c_str(), &st.dnt)) {
            changed = true;
        }

        ImGui::EndChild();
        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_AUTH_JS_PANEL_HPP_INCLUDED
