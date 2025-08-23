#pragma once
#ifndef _IMGUIX_WIDGETS_LIST_EDITOR_HPP_INCLUDED
#define _IMGUIX_WIDGETS_LIST_EDITOR_HPP_INCLUDED

/// \file list_editor.hpp
/// \brief Dynamic list editor for strings or integers.

#include <string>
#include <vector>
#include <cstring>

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Configuration for ListEditor.
    struct ListEditorConfig {
        std::string add_label    = u8"+"; ///< Label for the add button.
        std::string remove_label = u8"x"; ///< Label for the remove button.
        std::string new_item_hint;         ///< Initial value for new string items.
    };

    /// \brief Edit a dynamic list of strings with add/remove controls.
    /// \param id Unique widget ID.
    /// \param items List of strings to modify.
    /// \param cfg Additional configuration options.
    /// \return True if the list changed this frame.
    inline bool ListEditor(const char* id, std::vector<std::string>& items, const ListEditorConfig& cfg = {}) {
        bool changed = false;
        ImGui::PushID(id);

        for (size_t i = 0; i < items.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            char buf[256];
            std::strncpy(buf, items[i].c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = '\0';
            if (ImGui::InputText(u8"##item", buf, sizeof(buf))) {
                items[i] = buf;
                changed = true;
            }
            ImGui::SameLine();
            if (ImGui::SmallButton(cfg.remove_label.c_str())) {
                items.erase(items.begin() + static_cast<long>(i));
                --i;
                changed = true;
            }
            ImGui::PopID();
        }

        if (ImGui::SmallButton(cfg.add_label.c_str())) {
            items.emplace_back(cfg.new_item_hint);
            changed = true;
        }

        ImGui::PopID();
        return changed;
    }

    /// \brief Edit a dynamic list of integers with add/remove controls.
    /// \param id Unique widget ID.
    /// \param items List of integers to modify.
    /// \param cfg Additional configuration options.
    /// \return True if the list changed this frame.
    inline bool ListEditor(const char* id, std::vector<int>& items, const ListEditorConfig& cfg = {}) {
        bool changed = false;
        ImGui::PushID(id);

        for (size_t i = 0; i < items.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::InputInt(u8"##item", &items[i])) {
                changed = true;
            }
            ImGui::SameLine();
            if (ImGui::SmallButton(cfg.remove_label.c_str())) {
                items.erase(items.begin() + static_cast<long>(i));
                --i;
                changed = true;
            }
            ImGui::PopID();
        }

        if (ImGui::SmallButton(cfg.add_label.c_str())) {
            items.emplace_back(0);
            changed = true;
        }

        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_LIST_EDITOR_HPP_INCLUDED
