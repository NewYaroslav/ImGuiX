#pragma once
#ifndef _IMGUIX_EXTENSIONS_SCOPED_DISABLE_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_SCOPED_DISABLE_HPP_INCLUDED

/// \file scoped_disable.hpp
/// \brief Helpers to temporarily disable ImGui widgets.

#include <imgui.h>

namespace ImGuiX::Extensions {

    /// \brief Begin disabled block with optional alpha fade.
    /// \param disable When true, subsequent widgets are disabled.
    inline void BeginDisable(bool disable);

    /// \brief End disabled block started with BeginDisable.
    /// \param disable When true, pops style changes.
    inline void EndDisable(bool disable);

    /// \brief RAII scope disabling contained widgets.
    class ScopedDisable {
    public:
        /// \brief Construct scope and optionally disable widgets.
        /// \param disable True to disable widgets within scope.
        explicit ScopedDisable(bool disable);

        /// \brief Restore previous widget state.
        ~ScopedDisable();

    private:
        bool m_is_disabled = false;
    };

} // namespace ImGuiX::Extensions

#ifdef IMGUIX_HEADER_ONLY
#   include "scoped_disable.ipp"
#endif

#endif // _IMGUIX_EXTENSIONS_SCOPED_DISABLE_HPP_INCLUDED
