#include "domain_selector.hpp"

namespace ImGuiX::Widgets {

    inline bool DomainSelector(
            const char* label,
            DomainSelectorConfig& cfg,
            std::string& host
        ) {
        ImGui::PushID(label);
        ImGuiStorage* st = ImGui::GetStateStorage();
        const ImGuiID key_combo_index = ImGui::GetID("##combo_index");

        float height = ImGui::GetTextLineHeightWithSpacing() + 3.0f * ImGui::GetFrameHeightWithSpacing();
        ImVec2 size = cfg.panel_size;
        if (size.x <= 0.0f) size.x = ImGui::CalcItemWidth();
        if (size.y <= 0.0f) size.y = std::max(height, ImGui::GetFrameHeightWithSpacing());

        ImGui::BeginChild(u8"##DomainSelector", size, cfg.border);
        ImGui::TextUnformatted(cfg.header.c_str());
        ImGui::Separator();

        if (cfg.inputs_fill_width) ImGui::PushItemWidth(-FLT_MIN);

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

        if (cfg.inputs_fill_width) ImGui::PopItemWidth();

        ImGui::EndChild();
        ImGui::PopID();
        return updated;
    }

} // namespace ImGuiX::Widgets

