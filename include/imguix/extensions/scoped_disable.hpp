#pragma once
#ifndef _IMGUIX_EXTENSIONS_SCOPED_DISABLE_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_SCOPED_DISABLE_HPP_INCLUDED

/// \file scoped_disable.hpp
/// \brief Helpers to temporarily disable ImGui widgets.

#include <imgui.h>

namespace ImGuiX::Extensions {

    /// \brief Begin disabled block with optional alpha fade.
    /// \param disable When true, subsequent widgets are disabled.
    inline void BeginDisable(bool disable) {
        if (!disable)
            return;
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    /// \brief End disabled block started with BeginDisable.
    /// \param disable When true, pops style changes.
    inline void EndDisable(bool disable) {
        if (!disable)
            return;
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }

    /// \brief RAII scope disabling contained widgets.
    class ScopedDisable {
    public:
        /// \brief Construct scope and optionally disable widgets.
        /// \param disable True to disable widgets within scope.
        explicit ScopedDisable(bool disable) : m_is_disabled(disable) {
            BeginDisable(m_is_disabled);
        }

        /// \brief Restore previous widget state.
        ~ScopedDisable() {
            EndDisable(m_is_disabled);
        }

    private:
        bool m_is_disabled = false;
    };

} // namespace ImGuiX::Extensions

#endif // _IMGUIX_EXTENSIONS_SCOPED_DISABLE_HPP_INCLUDED
