#pragma once
#ifndef _IMGUIX_WIDGETS_MISC_THEME_PICKER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_MISC_THEME_PICKER_HPP_INCLUDED

/// \file theme_picker.hpp
/// \brief Combo widget for selecting UI theme with persistent storage.

#include <imgui.h>
#include <vector>
#include <string>
#include <algorithm>

#include <imguix/core/controller/Controller.hpp>
#include <imguix/core/window/WindowInstance.hpp>
#include <imguix/extensions/sizing.hpp> // ImGuiX::Extensions::CalcComboWidthForPreview
#include <imguix/themes/theme_ids.hpp>

namespace ImGuiX::Widgets {

    /// \brief Theme entry for ThemePicker.
    struct ThemeItem {
        const char* id;    ///< Theme identifier.
        const char* label; ///< Display name.
    };

    /// \brief Default list of themes.
    /// \return Reference to static theme list.
    inline const std::vector<ThemeItem>& DefaultThemes() {
        static const std::vector<ThemeItem> themes = {
            {IMGUIX_THEME_CLASSIC,        u8"Classic"},
            {IMGUIX_THEME_LIGHT,          u8"Light"},
            {IMGUIX_THEME_DARK,           u8"Dark"},
            {IMGUIX_THEME_CORPORATE_GREY, u8"Corporate Grey"},
            {IMGUIX_THEME_DARK_CHARCOAL,  u8"Dark Charcoal"},
            {IMGUIX_THEME_DARK_GRAPHITE,  u8"Dark Graphite"},
            {IMGUIX_THEME_DARK_TEAL,      u8"Dark Teal"},
            {IMGUIX_THEME_DEEP_DARK,      u8"Deep Dark"},
            {IMGUIX_THEME_GOLD_BLACK,     u8"Gold Black"},
            {IMGUIX_THEME_GREEN_BLUE,     u8"Green Blue"},
            {IMGUIX_THEME_LIGHT_GREEN,    u8"Light Green"},
            {IMGUIX_THEME_OSX_LIGHT,      u8"OSX Light"},
            {IMGUIX_THEME_PEARL_LIGHT,    u8"Pearl Light"},
            {IMGUIX_THEME_SLATE_DARK,     u8"Slate Dark"},
            {IMGUIX_THEME_VS_DARK,        u8"Visual Studio Dark"},
        };
        return themes;
    }

    /// \brief Configuration for ThemePicker widget.
    struct ThemePickerConfig {
        const char* label = u8"Theme";                   ///< Combo label.
        float combo_width = 0.0f;                        ///< Forced combo width.
        const std::vector<ThemeItem>* items = nullptr;   ///< Custom theme list.
    };

    /// \brief Theme selection combo with persistent storage.
    /// \param id Unique widget identifier.
    /// \param ctrl Controller that owns ThemeManager and OptionsStore.
    /// \param cfg Widget configuration.
    /// \return True if selection changed.
    inline bool ThemePicker(const char* id,
                            Controller* ctrl,
                            const ThemePickerConfig& cfg = {}) {
        if (!id || !ctrl) return false;

        const std::vector<ThemeItem>& list =
            (cfg.items && !cfg.items->empty()) ? *cfg.items : DefaultThemes();
        if (list.empty()) return false;

        auto& opts = ctrl->options();
        std::string cur_id = opts.getStrOr(IMGUIX_THEME_STORAGE_KEY, list[0].id);

        int cur_index = 0;
        for (int i = 0; i < static_cast<int>(list.size()); ++i) {
            if (cur_id == list[i].id) { cur_index = i; break; }
        }

        float combo_w = cfg.combo_width;
        if (combo_w <= 0.0f) {
            float maxw = 0.0f;
            for (const auto& t : list) {
                float w = ImGuiX::Extensions::CalcComboWidthForPreview(t.label);
                if (w > maxw) maxw = w;
            }
            combo_w = maxw;
        }

        bool changed = false;
        ImGui::PushID(id);
        ImGui::SetNextItemWidth(combo_w);
        const char* preview = list[cur_index].label;
        if (ImGui::BeginCombo(cfg.label ? cfg.label : u8"Theme", preview)) {
            for (int i = 0; i < static_cast<int>(list.size()); ++i) {
                bool sel = (i == cur_index);
                if (ImGui::Selectable(list[i].label, sel)) {
                    cur_index = i;
                    cur_id = list[i].id;
                    opts.setStr(IMGUIX_THEME_STORAGE_KEY, cur_id);
                    ctrl->setTheme(cur_id);
                    changed = true;
                }
                if (sel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::PopID();
        return changed;
    }

    /// \brief Apply stored theme to window if present in options.
    /// \param win Window to update.
    inline void ApplyStoredTheme(WindowInstance* win) {
        if (!win) return;
        const auto id = win->options().getStrOr(IMGUIX_THEME_STORAGE_KEY, "");
        if (!id.empty()) win->setTheme(id);
    }

    /// \brief Apply stored theme to controller if present in options.
    /// \param ctrl Controller to update.
    inline void ApplyStoredTheme(Controller* ctrl) {
        if (!ctrl) return;
        const auto id = ctrl->options().getStrOr(IMGUIX_THEME_STORAGE_KEY, "");
        if (!id.empty()) ctrl->setTheme(id);
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_MISC_THEME_PICKER_HPP_INCLUDED
