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

        /// \brief Change active locale; rebuilds atlas on next rebuildIfNeeded().
        void setLocale(std::string s) {
                static_cast<Impl*>(this)->setLocale(std::move(s));
        }

        /// \brief Set Markdown sizes (px at 96 DPI).
        void setMarkdownSizes(float body, float h1, float h2, float h3) {
                static_cast<Impl*>(this)->setMarkdownSizes(body, h1, h2, h3);
        }

        /// \brief Set logical DPI for display scaling.
        void setDpi(float dpi) {
                static_cast<Impl*>(this)->setDpi(dpi);
        }

        /// \brief Set global UI scale (additional multiplier).
        void setUiScale(float ui_scale) {
                static_cast<Impl*>(this)->setUiScale(ui_scale);
        }

        /// \brief Rebuild atlas if marked dirty.
        /// \warning Call between frames.
        BuildResult rebuildIfNeeded() {
                return static_cast<Impl*>(this)->rebuildIfNeeded();
        }

        // --- Convenient getters (read-only) ---

        /// \brief Get font by role; may return nullptr.
        [[nodiscard]] ImFont* getFont(FontRole role) const noexcept {
                return static_cast<const Impl*>(this)->getFont(role);
        }

        /// \brief Get currently active locale.
        [[nodiscard]] const std::string& activeLocale() const noexcept {
                return static_cast<const Impl*>(this)->activeLocale();
        }

        /// \brief Access current build parameters (dpi, ui_scale, base_dir, use_freetype).
        [[nodiscard]] const BuildParams& params() const noexcept {
                return static_cast<const Impl*>(this)->params();
        }
    };

} // namespace ImGuiX::Fonts

#endif // _IMGUIX_FONTS_FONT_MANAGER_CONTROL_CRTP_HPP_INCLUDED
