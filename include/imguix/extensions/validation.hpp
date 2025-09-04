#pragma once
#ifndef _IMGUIX_EXTENSIONS_VALIDATION_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_VALIDATION_HPP_INCLUDED

/// \file validation.hpp
/// \brief Lightweight helpers for input validation and scoped styling.

#include <imgui.h>
#include <string>

namespace ImGuiX::Extensions {

    /// \brief Safe regex match. Returns true if matches or if regex is invalid (fail-open).
    inline bool RegexMatchSafe(const std::string& s, const std::string& pattern);

    /// \brief Tint mode for invalid state.
    enum class InvalidTintMode {
        Frame,  ///< Tint FrameBg/FrameBgHovered/FrameBgActive (и опционально Border).
        Text    ///< Tint Text.
    };

    // --- Functional helpers ---

    /// \brief Begin invalid styling (mode-based). Returns number of pushed colors.
    /// \param is_invalid   Whether to push.
    /// \param color        Tint color.
    /// \param mode         Frame (default) or Text.
    /// \param include_border Add Border tint for Frame mode (default: false).
    /// \return Count of colors pushed (pass to EndInvalid).
    inline int BeginInvalid(bool is_invalid,
                            ImVec4 color = ImVec4(0.9f, 0.5f, 0.5f, 1.0f),
                            InvalidTintMode mode = InvalidTintMode::Frame,
                            bool include_border = false);

    /// \brief Legacy variant: tint a single ImGuiCol (kept for compatibility).
    inline void BeginInvalid(bool is_invalid,
                             ImVec4 color,
                             ImGuiCol target);

    /// \brief End invalid styling with given count (from BeginInvalid).
    inline void EndInvalid(bool is_invalid, int count);

    /// \brief Legacy variant: end with fixed count=1 (kept for compatibility).
    inline void EndInvalid(bool is_invalid);

    // --- RAII helper ---

    class ScopedInvalid {
    public:
        /// \brief Construct and push styling in \p mode if \p is_invalid.
        explicit ScopedInvalid(bool is_invalid,
                               ImVec4 color = ImVec4(0.9f, 0.5f, 0.5f, 1.0f),
                               InvalidTintMode mode = InvalidTintMode::Frame,
                               bool include_border = false);

        /// \brief Legacy ctor: tint a single ImGuiCol (compatibility).
        explicit ScopedInvalid(bool is_invalid,
                               ImVec4 color,
                               ImGuiCol target);

        ~ScopedInvalid();

        ScopedInvalid(const ScopedInvalid&) = delete;
        ScopedInvalid& operator=(const ScopedInvalid&) = delete;

        /// \brief End invalid tinting early (idempotent).
        void end();

    private:
        bool m_active = false;
        int  m_count  = 0;   // how many colors were pushed
    };

} // namespace ImGuiX::Extensions

#ifdef IMGUIX_HEADER_ONLY
#   include "validation.ipp"
#endif

#endif // _IMGUIX_EXTENSIONS_VALIDATION_HPP_INCLUDED
