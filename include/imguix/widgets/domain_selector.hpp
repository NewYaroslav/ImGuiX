#pragma once
#ifndef _IMGUIX_WIDGETS_DOMAIN_SELECTOR_HPP_INCLUDED
#define _IMGUIX_WIDGETS_DOMAIN_SELECTOR_HPP_INCLUDED

/// \file domain_selector.hpp
/// \brief Domain chooser widget (combo with "Custom" fallback or validated input with VK).

#include "validated_input.hpp" // InputTextWithVKValidated, KeyboardToggleConfig, InputValidatePolicy
#include "markers.hpp" // HelpMarker

namespace ImGuiX::Widgets {

    /// \brief Configuration for DomainSelector.
    struct DomainSelectorConfig {
        std::string header       = u8"Domain";
        std::string hint_domain  = u8"domain";
        std::string custom_text  = u8"Custom";
        std::string default_domain;
        std::string help_text;
        bool        show_help    = false;

        /// \brief List of predefined domains. If empty, InputTextWithVKValidated is used directly.
        std::vector<std::string> domains;

        // Validation (ECMAScript regex for std::regex)
        bool        validate_domain = true;
        ImGuiX::Widgets::InputValidatePolicy policy = ImGuiX::Widgets::InputValidatePolicy::OnTouch;
        ImVec4      error_color = ImVec4(0.9f, 0.5f, 0.5f, 1.0f);

        // Pragmatic host/domain pattern (allows subdomains and optional :port)
        const char* domain_regex = u8R"(^[A-Za-z0-9.\-:]+$)";

        // VK trigger + keyboard configs
        bool vk_enabled = true;
        ImGuiX::Widgets::KeyboardToggleConfig  kb_cfg{};
        ImGuiX::Widgets::VirtualKeyboardConfig vk_cfg{};

        // Extra input flags
        ImGuiInputTextFlags input_flags = 0; // e.g. ImGuiInputTextFlags_AutoSelectAll
    };

    /// \brief Renders domain selection UI. If current host not in list, "Custom" path is used.
    /// \param label ImGui label (must be unique).
    /// \param cfg Configuration.
    /// \param host Host string to edit.
    /// \return True if value changed this frame.
    inline bool DomainSelector(
            const char* label, 
            DomainSelectorConfig& cfg, 
            std::string& host
        ) {
        ImGui::PushID(label);
        ImGuiStorage* st = ImGui::GetStateStorage();
        const ImGuiID key_combo_index = ImGui::GetID("##combo_index");

        ImGui::BeginChild(u8"##DomainSelector", ImVec2(ImGui::GetWindowWidth() * 0.65f, 100.0f), true);
        ImGui::TextUnformatted(cfg.header.c_str());
        ImGui::Separator();

        bool updated = false;
        bool use_input = false;
        if (!cfg.domains.empty()) {
            // Build items + detect current
            std::vector<const char*> items;
            items.reserve(cfg.domains.size() + 1);
            for (size_t i = 0; i < cfg.domains.size(); ++i) {
                items.push_back(cfg.domains[i].c_str());
            }
            items.push_back(cfg.custom_text.c_str());

            int combo_index = st->GetInt(key_combo_index, (static_cast<int>(items.size()) - 1));
            if (combo_index < 0) combo_index = static_cast<int>(items.size()) - 1;
            if (combo_index >= static_cast<int>(items.size())) combo_index = static_cast<int>(items.size()) - 1;

            if (ImGui::Combo(
                    u8"##domain.combo", 
                    &combo_index, 
                    items.data(), 
                    static_cast<int>(items.size()))) {
                if (combo_index == static_cast<int>(cfg.domains.size())) {
                    host = cfg.default_domain;
                } else {
                    host = cfg.domains[combo_index];
                }
                st->SetInt(key_combo_index, combo_index);
                updated = true;
            }
            
            if (combo_index == static_cast<int>(cfg.domains.size())) {
                use_input = true;
            } else {
                use_input = false;
            }

            if (cfg.show_help) {
                ImGui::SameLine();
                ImGui::TextDisabled(u8"(?)");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip(u8"%s", cfg.help_text.c_str());
            }
        } else {
            use_input = true;
        }

        if (use_input) {
            bool ok = true;
            if (updated) {
                ImGui::SetKeyboardFocusHere(0);
            }
            if (cfg.vk_enabled) {
                if (InputTextWithVKValidated(
                        u8"##domain.input",
                        cfg.hint_domain.c_str(),
                        host,
                        cfg.validate_domain,
                        cfg.policy,
                        cfg.domain_regex,
                        ok,
                        cfg.error_color,
                        cfg.input_flags,
                        /*callback*/nullptr, /*user_data*/nullptr,
                        cfg.kb_cfg, cfg.vk_cfg)) {
                    updated = true;
                }
            } else {
                if (InputTextValidated(
                        u8"##domain.input",
                        cfg.hint_domain.c_str(),
                        host,
                        cfg.validate_domain,
                        cfg.policy,
                        cfg.domain_regex,
                        ok,
                        cfg.error_color,
                        cfg.input_flags)) {
                    updated = true;
                }
            }
        }

        if (cfg.domains.empty() && cfg.show_help) {
            ImGui::SameLine();
			ImGuiX::Widgets::HelpMarker(cfg.help_text.c_str());
        }

        ImGui::EndChild();
        ImGui::PopID();
        return updated;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_DOMAIN_SELECTOR_HPP_INCLUDED

