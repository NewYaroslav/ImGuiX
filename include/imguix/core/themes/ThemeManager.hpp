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
            if (const Theme* theme = currentTheme()) {
                theme->apply(ImGui::GetStyle());
#               ifdef IMGUIX_ENABLE_IMPLOT
                theme->apply(ImPlot::GetStyle());
#               endif
#               ifdef IMGUIX_ENABLE_IMPLOT3D
                theme->apply(ImPlot3D::GetStyle());
#               endif
            }
            m_dirty = false;
        }

        /// \brief Get custom color from the active theme by string key.
        /// \param key Custom color key.
        /// \param out Resolved color value.
        /// \return True when value exists.
        bool tryGetCurrentThemeCustomColor(std::string_view key, ImVec4& out) const {
            const Theme* theme = currentTheme();
            return theme != nullptr && theme->tryGetCustomColor(key, out);
        }

        /// \brief Get custom color from the active theme by numeric key.
        /// \param key Numeric custom color key.
        /// \param out Resolved color value.
        /// \return True when value exists.
        bool tryGetCurrentThemeCustomColor(std::uint32_t key, ImVec4& out) const {
            const Theme* theme = currentTheme();
            return theme != nullptr && theme->tryGetCustomColor(key, out);
        }

        /// \brief Get custom color from the active theme with string-first fallback.
        /// \param key Primary string key.
        /// \param fallback_id Numeric fallback key.
        /// \param out Resolved color value.
        /// \return True when value exists.
        bool tryGetCurrentThemeCustomColor(std::string_view key, std::uint32_t fallback_id, ImVec4& out) const {
            const Theme* theme = currentTheme();
            return theme != nullptr && theme->tryGetCustomColor(key, fallback_id, out);
        }

        /// \brief Get custom value from the active theme by string key.
        /// \param key Custom value key.
        /// \param out Resolved value.
        /// \return True when value exists.
        bool tryGetCurrentThemeCustomValue(std::string_view key, ThemeCustomValue& out) const {
            const Theme* theme = currentTheme();
            return theme != nullptr && theme->tryGetCustomValue(key, out);
        }

        /// \brief Get custom value from the active theme by numeric key.
        /// \param key Numeric custom value key.
        /// \param out Resolved value.
        /// \return True when value exists.
        bool tryGetCurrentThemeCustomValue(std::uint32_t key, ThemeCustomValue& out) const {
            const Theme* theme = currentTheme();
            return theme != nullptr && theme->tryGetCustomValue(key, out);
        }

        /// \brief Get custom value from the active theme with string-first fallback.
        /// \param key Primary string key.
        /// \param fallback_id Numeric fallback key.
        /// \param out Resolved value.
        /// \return True when value exists.
        bool tryGetCurrentThemeCustomValue(std::string_view key, std::uint32_t fallback_id, ThemeCustomValue& out) const {
            const Theme* theme = currentTheme();
            return theme != nullptr && theme->tryGetCustomValue(key, fallback_id, out);
        }

    private:
        const Theme* currentTheme() const {
            if (m_current.empty()) {
                return nullptr;
            }
            auto it = m_themes.find(m_current);
            if (it == m_themes.end() || !it->second) {
                return nullptr;
            }
            return it->second.get();
        }

        std::unordered_map<std::string, std::unique_ptr<Theme>> m_themes;
        std::string m_current;
        bool m_dirty = false;
    };

} // namespace ImGuiX::Themes

#endif // _IMGUIX_CORE_THEME_MANAGER_HPP_INCLUDED
