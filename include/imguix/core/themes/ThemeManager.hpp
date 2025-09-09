#pragma once
#ifndef _IMGUIX_CORE_THEME_MANAGER_HPP_INCLUDED
#define _IMGUIX_CORE_THEME_MANAGER_HPP_INCLUDED

/// \file ThemeManager.hpp 
/// \brief Manages ImGui style themes.

#include <imgui.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#ifdef IMGUIX_ENABLE_IMPLOT
  #include <implot.h>
#endif
#ifdef IMGUIX_ENABLE_IMPLOT3D
  #include <implot3d.h>
#endif

#include "Theme.hpp"

namespace ImGuiX::Themes {

    /// \brief Manage registration and application of themes.
    class ThemeManager {
    public:

        /// \brief Construct manager with built-in themes.
        ThemeManager() {
            registerTheme("classic", std::make_unique<ClassicTheme>());
            registerTheme("light",   std::make_unique<LightTheme>());
            registerTheme("dark",    std::make_unique<DarkTheme>());
        }

        /// \brief Register theme under identifier.
        /// \param id Theme identifier.
        /// \param theme Theme instance.
        /// \return True if existing theme was replaced.
        bool registerTheme(std::string id, std::unique_ptr<Theme> theme) {
            bool replaced = (m_themes.find(id) != m_themes.end());
            m_themes.insert_or_assign(id, std::move(theme));
            if (id == m_current) m_dirty = true;
            return replaced;
        }

        /// \brief Check whether theme is registered.
        /// \param id Theme identifier.
        /// \return True if theme exists.
        bool hasTheme(const std::string& id) const {
            return m_themes.find(id) != m_themes.end();
        }

        /// \brief Remove theme by identifier.
        /// \param id Theme identifier.
        /// \return Removed theme or null.
        std::unique_ptr<Theme> unregisterTheme(const std::string& id) {
            auto it = m_themes.find(id);
            if (it == m_themes.end()) return {};
            auto old = std::move(it->second);
            m_themes.erase(it);
            if (id == m_current) m_dirty = false;
            return old;
        }

        /// \brief Set active theme identifier.
        /// \param id Identifier of registered theme.
        void setTheme(std::string id) {
            m_current = std::move(id);
            m_dirty = true;
        }

        /// \brief Apply currently selected theme to style.
        void updateCurrentTheme() {
            if (!m_dirty || m_current.empty()) return;
            if (auto it = m_themes.find(m_current); it != m_themes.end() && it->second) {
                it->second->apply(ImGui::GetStyle());
#               ifdef IMGUIX_ENABLE_IMPLOT
                it->second->apply(ImPlot::GetStyle());
#               endif
#               ifdef IMGUIX_ENABLE_IMPLOT3D
                it->second->apply(ImPlot3D::GetStyle());
#               endif
            }
            m_dirty = false;
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<Theme>> m_themes;
        std::string m_current;
        bool m_dirty = false;
    };

} // namespace ImGuiX::Themes

#endif // _IMGUIX_CORE_THEME_MANAGER_HPP_INCLUDED

