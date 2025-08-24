#pragma once
#ifndef _IMGUIX_EXTENSIONS_VALIDATION_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_VALIDATION_HPP_INCLUDED

/// \file validation.hpp
/// \brief Lightweight helpers for input validation and scoped styling.

#include <imgui.h>
#include <regex>
#include <string>

namespace ImGuiX::Extensions {

    /// \brief Safe regex match. Returns true if matches or if regex is invalid (fail-open).
    /// \param s       Input string to validate.
    /// \param pattern Regex pattern (std::string). If empty or invalid, returns true.
    inline bool RegexMatchSafe(const std::string& s, const std::string& pattern) {
        if (pattern.empty()) return true;
        try { std::regex re(pattern); return std::regex_match(s, re); }
        catch (...) { return true; } // invalid regex -> don't block UI
    }

    /// \brief Begin invalid styling if condition is true.
    /// \param is_invalid Whether to push error color.
    /// \param color      Color to use when invalid.
    /// \param target     ImGuiCol to tint (default: Text).
    inline void BeginInvalid(bool is_invalid,
                             ImVec4 color = ImVec4(0.9f, 0.5f, 0.5f, 1.0f),
                             ImGuiCol target = ImGuiCol_Text) {
        if (is_invalid) ImGui::PushStyleColor(target, color);
    }

    /// \brief End invalid styling if condition is true.
    /// \param is_invalid Whether BeginInvalid was used.
    /// \param count      Number of colors to pop (usually 1).
    inline void EndInvalid(bool is_invalid, int count = 1) {
        if (is_invalid) ImGui::PopStyleColor(count);
    }

    /// \brief RAII scope for invalid styling (push on ctor, pop on dtor).
    class ScopedInvalid {
    public:
        /// \brief Construct and optionally push error color.
        /// \param is_invalid If true, pushes style color.
        /// \param color      Color to use (default: red-ish).
        /// \param target     ImGuiCol to affect (default: Text).
        explicit ScopedInvalid(bool is_invalid,
                               ImVec4 color = ImVec4(0.9f, 0.5f, 0.5f, 1.0f),
                               ImGuiCol target = ImGuiCol_Text)
            : m_active(is_invalid) {
            if (m_active) ImGui::PushStyleColor(target, color);
        }

        /// \brief Pop style color if still active.
        ~ScopedInvalid() { end(); }

        ScopedInvalid(const ScopedInvalid&) = delete;
        ScopedInvalid& operator=(const ScopedInvalid&) = delete;

        /// \brief End invalid tinting early (idempotent).
        /// \details Useful to stop tinting before drawing adjacent widgets (e.g., buttons).
        void end() {
            if (m_active) {
                ImGui::PopStyleColor();
                m_active = false;
            }
        }
    private:
        bool m_active = false;
    };

} // namespace ImGuiX::Extensions

#endif // _IMGUIX_EXTENSIONS_VALIDATION_HPP_INCLUDED
