#pragma once
#ifndef _IMGUIX_THEMES_THEME_MANAGER_HPP_INCLUDED
#define _IMGUIX_THEMES_THEME_MANAGER_HPP_INCLUDED

/// \file ThemeManager.hpp
/// \brief Manages ImGui style themes.

#include <imgui.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Theme.hpp"

namespace ImGuiX::Themes {

    class ThemeManager {
    public:
        /// \brief Register theme under identifier.
        /// \param id Theme identifier.
        /// \param theme Theme instance.
        void registerTheme(std::string id, std::unique_ptr<Theme> theme) {
            m_themes.emplace(std::move(id), std::move(theme));
        }

        /// \brief Set active theme identifier.
        /// \param id Identifier of registered theme.
        void setTheme(std::string_view id) {
            m_current.assign(id.begin(), id.end());
        }

        /// \brief Apply currently selected theme to style.
        /// \param style Target ImGui style.
        void updateCurrentTheme(ImGuiStyle& style) {
            auto it = m_themes.find(m_current);
            if (it != m_themes.end() && it->second) {
                it->second->apply(style);
            }
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<Theme>> m_themes;
        std::string m_current;
    };

} // namespace ImGuiX::Themes

#endif // _IMGUIX_THEMES_THEME_MANAGER_HPP_INCLUDED
