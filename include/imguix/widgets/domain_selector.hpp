#pragma once
#ifndef _IMGUIX_WIDGETS_DOMAIN_SELECTOR_HPP_INCLUDED
#define _IMGUIX_WIDGETS_DOMAIN_SELECTOR_HPP_INCLUDED

/// \file domain_selector.hpp
/// \brief Domain chooser widget (combo with "Custom" fallback or plain input).

#include <imgui.h>
#include <string>
#include <vector>

namespace ImGuiX::Widgets {

    /// \brief Configuration for DomainSelector.
    struct DomainSelectorConfig {
        std::string header      = "Domain";
        std::string hint_domain = "domain";
        std::string custom_text = "Custom";
        std::string default_domain;
        std::string help_text;
        bool        show_help   = false;

        /// \brief List of predefined domains. If empty, InputText is used.
        std::vector<std::string> domains;
    };

    /// \brief Renders domain selection UI. If current host not in list, "Custom" path is used.
    /// \param id Unique widget ID.
    /// \param cfg Configuration.
    /// \param host Host string to edit.
    /// \return True if value changed this frame.
    inline bool DomainSelector(const char* id, DomainSelectorConfig& cfg, std::string& host) {
        bool updated = false;
        ImGui::PushID(id);

        ImGui::BeginChild("##DomainSelector", ImVec2(ImGui::GetWindowWidth() * 0.65f, 100.0f), true);
        ImGui::Text("%s", cfg.header.c_str());
        ImGui::Separator();

        bool use_input = cfg.domains.empty();
        int combo_index = -1;

        if (!cfg.domains.empty()) {
            // Build items + detect current
            std::vector<const char*> items;
            items.reserve(cfg.domains.size() + 1);
            for (size_t i = 0; i < cfg.domains.size(); ++i) {
                items.push_back(cfg.domains[i].c_str());
                if (cfg.domains[i] == host) combo_index = static_cast<int>(i);
            }
            items.push_back(cfg.custom_text.c_str());
            if (combo_index < 0) { combo_index = static_cast<int>(items.size()) - 1; use_input = true; }

            if (ImGui::Combo("##domain.combo", &combo_index, items.data(), static_cast<int>(items.size()))) {
                if (combo_index == static_cast<int>(cfg.domains.size())) {
                    host = cfg.default_domain;
                    use_input = true;
                } else {
                    host = cfg.domains[combo_index];
                    use_input = false;
                }
                updated = true;
            }

            if (cfg.show_help) { ImGui::SameLine(); ImGui::TextDisabled("(?)"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", cfg.help_text.c_str()); }
        }

        if (use_input) {
            char buf[512];
            std::strncpy(buf, host.c_str(), sizeof(buf));
            buf[sizeof(buf)-1] = '\0';
            if (ImGui::InputTextWithHint("##domain.input", cfg.hint_domain.c_str(), buf, sizeof(buf)-1)) {
                host = buf;
                updated = true;
            }
        }

        if (cfg.domains.empty() && cfg.show_help) {
            ImGui::SameLine();
            ImGui::TextDisabled("(?)"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", cfg.help_text.c_str());
        }

        ImGui::EndChild();
        ImGui::PopID();
        return updated;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_DOMAIN_SELECTOR_HPP_INCLUDED
