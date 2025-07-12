#pragma once
#ifndef _IMGUIX_EXTENSIONS_FORCE_OPAQUE_STYLE_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_FORCE_OPAQUE_STYLE_HPP_INCLUDED

/// \file force_opaque_style.hpp
/// \brief Adds utility functions to enforce fully opaque ImGui style (alpha = 1.0f).

#include <imgui.h>

namespace ImGui {

    /// \brief Sets ImGui style to be fully opaque.
    ///
    /// Applies `style.Alpha = 1.0f` and forces the alpha component of all style colors
    /// (`Colors[i].w`) to be 1.0f, disabling any transparency.
    /// Useful when using DWM transparency or to prevent unwanted blending with the background.
    inline void ForceOpaqueStyle() {
        ImGuiStyle& style = GetStyle();
        style.Alpha = 1.0f;
        for (int i = 0; i < ImGuiCol_COUNT; ++i) {
            style.Colors[i].w = 1.0f;
        }
    }

    /// \brief RAII-style scope to temporarily enforce a fully opaque ImGui style.
    ///
    /// Restores all style colors and alpha after scope ends.
    class ScopedOpaqueStyle {
    public:
        /// \brief Saves the current style and applies a fully opaque one.
        ScopedOpaqueStyle() {
            ImGuiStyle& style = GetStyle();
            m_old_alpha = style.Alpha;
            for (int i = 0; i < ImGuiCol_COUNT; ++i) {
                m_old_colors[i] = style.Colors[i];
                style.Colors[i].w = 1.0f;
            }
            style.Alpha = 1.0f;
        }

        /// \brief Restores the previous style upon destruction.
        ~ScopedOpaqueStyle() {
            ImGuiStyle& style = GetStyle();
            style.Alpha = m_old_alpha;
            for (int i = 0; i < ImGuiCol_COUNT; ++i) {
                style.Colors[i] = m_old_colors[i];
            }
        }

    private:
        ImVec4 m_old_colors[ImGuiCol_COUNT];
        float m_old_alpha = 1.0f;
    };

} // namespace ImGui

#endif // _IMGUIX_EXTENSIONS_FORCE_OPAQUE_STYLE_HPP_INCLUDED