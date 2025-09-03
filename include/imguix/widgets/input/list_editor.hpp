#pragma once
#ifndef _IMGUIX_WIDGETS_LIST_EDITOR_HPP_INCLUDED
#define _IMGUIX_WIDGETS_LIST_EDITOR_HPP_INCLUDED

/// \file list_editor.hpp
/// \brief Combo-like list editor (strings/ints) with popup child. No CSV.
/// \note ImGui 1.89+. Uses public API only.

#include <imgui.h>
#include <string>
#include <vector>

#include <imguix/config/icons.hpp>

namespace ImGuiX::Widgets {

    /// \brief Config for ListEditor.
    struct ListEditorConfig {
        float       combo_width  = 0.0f;                       ///< Width of combo preview button.
        ImVec2      popup_size   = ImVec2(0.0f, 0.0f);         ///< Size of popup child.
        const char* empty_desc  = u8"(empty)";                 ///< Preview when list is empty.

        // Default to Material Icons PUA. Make sure the font is loaded with PUA ranges.
        const char* icon_add        = IMGUIX_ICON_ADD;         ///< 'add'
        const char* icon_remove     = IMGUIX_ICON_REMOVE;      ///< 'remove_circle'
        const char* icon_combo      = IMGUIX_ICON_FILTER_LIST; ///< Icon shown on combo button.
        bool        use_icon_combo  = false;                   ///< Replace default arrow by icon.

        bool        delete_on_right = true;                    ///< Remove button placed at right side.
        bool        deduplicate     = false;                   ///< Keep unique values on add.
    };

    // ------------------------------- strings -------------------------------

    /// \brief Combo-like list editor for strings. One input -> one element. No CSV.
    /// \param id Unique widget identifier.
    /// \param label Visible label for the input.
    /// \param items In/out list of strings.
    /// \param cfg Configuration parameters.
    /// \return True if the list changed.
    bool ListEditor(
            const char* id,
            const char* label,
            std::vector<std::string>& items,
            const ListEditorConfig& cfg = {}
        );

    // ------------------------------- integers -------------------------------

    /// \brief Combo-like list editor for integers. One input -> one element. No CSV.
    /// \param id Unique widget identifier.
    /// \param label Visible label for the input.
    /// \param items In/out list of integers.
    /// \param cfg Configuration parameters.
    /// \return True if the list changed.
    bool ListEditor(
            const char* id,
            const char* label,
            std::vector<int>& items,
            const ListEditorConfig& cfg = {}
        );

#ifdef IMGUIX_DEMO
    /// \brief Render demo for ListEditor widgets.
    inline void DemoListEditor() {
        static std::vector<std::string> names = {"Alice", "Bob"};
        static std::vector<int> numbers = {1, 2, 3};
        ListEditor("##list1.names",   "Names",   names);
        ListEditor("##list1.numbers", "Numbers", numbers);
        
        ImGui::SeparatorText("Witch icon combo");
        
        ListEditorConfig cfg_icon;
        cfg_icon.use_icon_combo = true;

        cfg_icon.icon_combo     = u8"\uE241";
        ListEditor("##list2.names",   "Names",   names, cfg_icon);
        cfg_icon.icon_combo     = u8"\uE242";
        ListEditor("##list2.numbers", "Numbers", numbers, cfg_icon);
    }
#endif

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "list_editor.ipp"
#endif

#endif // _IMGUIX_WIDGETS_LIST_EDITOR_HPP_INCLUDED

