#pragma once
#ifndef _IMGUIX_WIDGETS_AUTH_JS_PANEL_HPP_INCLUDED
#define _IMGUIX_WIDGETS_AUTH_JS_PANEL_HPP_INCLUDED

/// \file auth_js_panel.hpp
/// \brief Browser auth config panel: user-agent, accept-language, and DNT flag.
/// Uses InputTextWithVKValidated (validation + on-screen keyboard trigger).

#include "validated_input.hpp" // InputTextWithVKValidated, KeyboardToggleConfig, InputValidatePolicy

namespace ImGuiX::Widgets {

    /// \brief Mutable JS auth settings.
    struct AuthJsSettings {
        std::string user_agent;        ///< User-Agent header
        std::string accept_language;   ///< Accept-Language header
        bool        dnt = false;       ///< Do Not Track flag
    };

    /// \brief UI configuration for AuthJsPanel.
    struct AuthJsPanelConfig {
        // Labels / hints
        std::string header               = u8"JS Config";
        std::string hint_user_agent      = u8"User agent";
        std::string hint_accept_language = u8"Accept language";
        std::string label_dnt            = u8"Do Not Track";

        // Panel
        ImVec2      panel_size           = ImVec2(0, 0); ///< (0,0) → auto width/height
        bool        border               = true;

        // Validation
        bool        validate_user_agent      = true;
        bool        validate_accept_language = true;
        ImGuiX::Widgets::InputValidatePolicy policy = ImGuiX::Widgets::InputValidatePolicy::OnTouch;
        ImVec4      error_color           = ImVec4(0.9f, 0.5f, 0.5f, 1.0f);

        // Pragmatic regexes (ECMAScript for std::regex)
        // UA: printable ASCII (space through ~), at least 1 char
        const char* user_agent_regex      = u8R"(^[ -~]+$)";
        // Accept-Language: allow tokens, subtags, q-factors; permissive char class
        const char* accept_language_regex = u8R"(^(?:[A-Za-z0-9,*_\-.;=,\s])+)$)";

        // VK trigger + keyboard configs
        ImGuiX::Widgets::KeyboardToggleConfig  kb_cfg{};
        ImGuiX::Widgets::VirtualKeyboardConfig vk_cfg{};

        // Enable VK per field
        bool vk_user_agent      = true;
        bool vk_accept_language = true;

        // Extra input flags applied to both fields (e.g., AutoSelectAll)
        ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_AutoSelectAll;
    };

    /// \brief Draw browser auth settings panel.
    /// \param id Unique widget identifier.
    /// \param cfg Panel configuration.
    /// \param st Settings model to edit.
    /// \return True if any field changed this frame.
    inline bool AuthJsPanel(const char* id, AuthJsPanelConfig& cfg, AuthJsSettings& st) {
        bool changed = false;
        ImGui::PushID(id);

        // Auto size similar to other panels
        const ImVec2 size = (cfg.panel_size.x <= 0.f || cfg.panel_size.y <= 0.f)
                            ? ImVec2(ImGui::GetWindowWidth() * 0.65f, ImGui::GetFrameHeightWithSpacing() * 5.0f)
                            : cfg.panel_size;

        ImGui::BeginChild(u8"##auth_js_panel", size, cfg.border);
        ImGui::TextUnformatted(cfg.header.c_str());
        ImGui::Separator();

        // User-Agent
        {
            bool ok = true;
            if (cfg.vk_user_agent) {
                if (InputTextWithVKValidated(u8"##authjs.user_agent",
                        cfg.hint_user_agent.c_str(),
                        st.user_agent,
                        cfg.validate_user_agent,
                        cfg.policy,
                        cfg.user_agent_regex,
                        ok,
                        cfg.error_color,
                        cfg.input_flags,
                        /*callback*/nullptr, /*user_data*/nullptr,
                        cfg.kb_cfg, cfg.vk_cfg)) {
                    changed = true;
                }
            } else {
                if (InputTextValidated(u8"##authjs.user_agent",
                        cfg.hint_user_agent.c_str(),
                        st.user_agent,
                        cfg.validate_user_agent,
                        cfg.policy,
                        cfg.user_agent_regex,
                        ok,
                        cfg.error_color,
                        cfg.input_flags)) {
                    changed = true;
                }
            }
        }

        // Accept-Language
        {
            bool ok = true;
            if (cfg.vk_accept_language) {
                if (InputTextWithVKValidated(u8"##authjs.accept_lang",
                        cfg.hint_accept_language.c_str(),
                        st.accept_language,
                        cfg.validate_accept_language,
                        cfg.policy,
                        cfg.accept_language_regex,
                        ok,
                        cfg.error_color,
                        cfg.input_flags,
                        /*callback*/nullptr, /*user_data*/nullptr,
                        cfg.kb_cfg, cfg.vk_cfg)) {
                    changed = true;
                }
            } else {
                if (InputTextValidated(u8"##authjs.accept_lang",
                        cfg.hint_accept_language.c_str(),
                        st.accept_language,
                        cfg.validate_accept_language,
                        cfg.policy,
                        cfg.accept_language_regex,
                        ok,
                        cfg.error_color,
                        cfg.input_flags)) {
                    changed = true;
                }
            }
        }

        // DNT
        if (ImGui::Checkbox(cfg.label_dnt.c_str(), &st.dnt)) {
            changed = true;
        }

        ImGui::EndChild();
        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_AUTH_JS_PANEL_HPP_INCLUDED

