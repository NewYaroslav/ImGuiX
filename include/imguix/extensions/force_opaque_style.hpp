#pragma once
#ifndef _IMGUIX_EXTENSIONS_FORCE_OPAQUE_STYLE_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_FORCE_OPAQUE_STYLE_HPP_INCLUDED

/// \file force_opaque_style.hpp
/// \brief Adds utility functions to enforce fully opaque ImGui style (alpha = 1.0f).

#include <imgui.h>

namespace ImGuiX::Extensions {

    /// \brief Set ImGui style to fully opaque.
    /// \note Applies `style.Alpha = 1.0f` and sets all style colors' alpha to 1.0f.
    /// \note Useful when using DWM transparency or to prevent unwanted background blending.
    inline void ForceOpaqueStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Alpha = 1.0f;
        for (int i = 0; i < ImGuiCol_COUNT; ++i) {
            style.Colors[i].w = 1.0f;
        }
    }

    /// \brief RAII-style scope to enforce fully opaque ImGui style.
    /// \note Restores all style colors and alpha after scope ends.
    class ScopedOpaqueStyle {
    public:
        /// \brief Save current style and apply fully opaque style.
        ScopedOpaqueStyle() {
            ImGuiStyle& style = ImGui::GetStyle();
            m_old_alpha = style.Alpha;
            for (int i = 0; i < ImGuiCol_COUNT; ++i) {
                m_old_colors[i] = style.Colors[i];
                style.Colors[i].w = 1.0f;
            }
            style.Alpha = 1.0f;
        }

        /// \brief Restore previous style on destruction.
        ~ScopedOpaqueStyle() {
            ImGuiStyle& style = ImGui::GetStyle();
            style.Alpha = m_old_alpha;
            for (int i = 0; i < ImGuiCol_COUNT; ++i) {
                style.Colors[i] = m_old_colors[i];
            }
        }

    private:
        ImVec4 m_old_colors[ImGuiCol_COUNT];
        float m_old_alpha = 1.0f;
    };

} // namespace ImGuiX::Extensions

#endif // _IMGUIX_EXTENSIONS_FORCE_OPAQUE_STYLE_HPP_INCLUDED