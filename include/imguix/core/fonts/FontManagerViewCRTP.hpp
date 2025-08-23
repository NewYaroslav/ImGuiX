#pragma once
#ifndef _IMGUIX_FONTS_FONT_MANAGER_VIEW_CRTP_HPP_INCLUDED
#define _IMGUIX_FONTS_FONT_MANAGER_VIEW_CRTP_HPP_INCLUDED

/// \file FontManagerViewCRTP.hpp
/// \brief CRTP view facade exposing read-only font access.

namespace ImGuiX::Fonts {

    template<class Impl>
    struct FontManagerViewCRTP {

        /// \brief Get font by role.
        /// \param r Logical font role.
        /// \return Pointer to font or nullptr if unavailable.
        ImFont* getFont(FontRole r) const noexcept {
                return static_cast<const Impl*>(this)->getFont(r);
        }

        /// \brief Get currently active locale identifier.
        /// \return Locale identifier string.
        const std::string& activeLocale() const noexcept {
                return static_cast<const Impl*>(this)->activeLocale();
        }

        /// \brief Access current build parameters.
        /// \return Build parameters including dpi and scaling.
        const BuildParams& params() const noexcept {
                return static_cast<const Impl*>(this)->params();
        }
    };

} // namespace ImGuiX::Fonts

#endif // _IMGUIX_FONTS_FONT_MANAGER_VIEW_CRTP_HPP_INCLUDED
