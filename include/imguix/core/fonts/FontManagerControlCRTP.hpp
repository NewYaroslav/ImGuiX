#pragma once
#ifndef _IMGUIX_FONTS_FONT_MANAGER_CONTROL_CRTP_HPP_INCLUDED
#define _IMGUIX_FONTS_FONT_MANAGER_CONTROL_CRTP_HPP_INCLUDED

/// \file FontManagerControlCRTP.hpp
/// \brief CRTP facade for safe runtime font management.
/// \note All methods must be called on the GUI thread between frames.

namespace ImGuiX::Fonts {

    template<class Impl>
    struct FontManagerControlCRTP {
        // --- Mutation safe at runtime ---

        /// \brief Change active locale.
        /// \param s Locale identifier.
        void setLocale(std::string s) {
                static_cast<Impl*>(this)->setLocale(std::move(s));
        }

        /// \brief Set Markdown sizes in pixels at 96 DPI.
        /// \param body Body text size in pixels.
        /// \param h1 Header level 1 size in pixels.
        /// \param h2 Header level 2 size in pixels.
        /// \param h3 Header level 3 size in pixels.
        void setMarkdownSizes(float body, float h1, float h2, float h3) {
                static_cast<Impl*>(this)->setMarkdownSizes(body, h1, h2, h3);
        }

        /// \brief Set logical DPI for display scaling.
        /// \param dpi Display DPI where 96 equals 1.0 scale.
        void setDpi(float dpi) {
                static_cast<Impl*>(this)->setDpi(dpi);
        }

        /// \brief Set global UI scale.
        /// \param ui_scale Additional scaling multiplier.
        void setUiScale(float ui_scale) {
                static_cast<Impl*>(this)->setUiScale(ui_scale);
        }

        /// \brief Rebuild atlas if marked dirty.
        /// \warning Call between frames.
        /// \return Result summary of the rebuild.
        BuildResult rebuildIfNeeded() {
                return static_cast<Impl*>(this)->rebuildIfNeeded();
        }

        // --- Convenient getters (read-only) ---

        /// \brief Get font by role.
        /// \param role Logical font role.
        /// \return Pointer to font or nullptr if unavailable.
        [[nodiscard]] ImFont* getFont(FontRole role) const noexcept {
                return static_cast<const Impl*>(this)->getFont(role);
        }

        /// \brief Get currently active locale.
        /// \return Locale identifier string.
        [[nodiscard]] const std::string& activeLocale() const noexcept {
                return static_cast<const Impl*>(this)->activeLocale();
        }

        /// \brief Access current build parameters.
        /// \return Build parameters including dpi, scale and base directory.
        [[nodiscard]] const BuildParams& params() const noexcept {
                return static_cast<const Impl*>(this)->params();
        }
    };

} // namespace ImGuiX::Fonts

#endif // _IMGUIX_FONTS_FONT_MANAGER_CONTROL_CRTP_HPP_INCLUDED
