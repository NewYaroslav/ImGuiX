#pragma once
#ifndef _IMGUIX_UTILS_ENCODING_UTILS_HPP_INCLUDED
#define _IMGUIX_UTILS_ENCODING_UTILS_HPP_INCLUDED

/// \file encoding_utils.hpp
/// \brief Utilities for working with character encodings and string transformations.

#ifdef _WIN32

#include <string>
#include <Windows.h>

namespace ImGuiX::Utils {

    /// \brief Convert UTF-8 string to ANSI string (Windows-specific).
    /// \param utf8 UTF-8 encoded string.
    /// \return Converted ANSI string.
    std::string Utf8ToAnsi(const std::string& utf8) noexcept;

    /// \brief Convert ANSI string to UTF-8 string (Windows-specific).
    /// \param ansi ANSI encoded string.
    /// \return Converted UTF-8 string.
    std::string AnsiToUtf8(const std::string& ansi) noexcept;

    /// \brief Convert UTF-8 string to CP866 string (DOS-specific, Windows).
    /// \param utf8 UTF-8 encoded string.
    /// \return Converted CP866 string.
    std::string Utf8ToCp866(const std::string& utf8) noexcept;

    /// \brief Validate UTF-8 string.
    /// \param message String to validate.
    /// \return True if string is valid UTF-8.
    bool IsValidUtf8(const char* message);

    /// \brief Convert CP1251 string to UTF-8.
    /// \param cp1251 CP1251 encoded string.
    /// \return Converted UTF-8 string.
    std::string Cp1251ToUtf8(const std::string& cp1251);

    /// \brief Convert UTF-16 string to UTF-8.
    /// \param utf16String Wide character string.
    /// \return Converted UTF-8 string.
    std::string Utf16ToUtf8(LPWSTR utf16String);

    /// \brief Convert UTF-8 string to UTF-16 wide string.
    /// \param utf8 UTF-8 encoded string.
    /// \return Converted UTF-16 string.
    std::wstring Utf8ToUtf16(const std::string& utf8) noexcept;

} // namespace ImGuiX::Utils
#else

#include <string>

#include "detail/common.hpp"

namespace ImGuiX::Utils {

    template<typename Dummy = void>
    std::string Utf8ToAnsi(const std::string&) {
        static_assert(detail::dependent_false_v<Dummy>, u8"Utf8ToAnsi is not supported on this platform");
        return {};
    }

    template<typename Dummy = void>
    std::string AnsiToUtf8(const std::string&) {
        static_assert(detail::dependent_false_v<Dummy>, u8"AnsiToUtf8 is not supported on this platform");
        return {};
    }

    template<typename Dummy = void>
    std::string Utf8ToCp866(const std::string&) {
        static_assert(detail::dependent_false_v<Dummy>, u8"Utf8ToCp866 is not supported on this platform");
        return {};
    }

    template<typename Dummy = void>
    bool IsValidUtf8(const char*) {
        static_assert(detail::dependent_false_v<Dummy>, u8"IsValidUtf8 is not supported on this platform");
        return false;
    }

    template<typename Dummy = void>
    std::string Cp1251ToUtf8(const std::string&) {
        static_assert(detail::dependent_false_v<Dummy>, u8"Cp1251ToUtf8 is not supported on this platform");
        return {};
    }

    template<typename Dummy = void>
    std::string Utf16ToUtf8(void*) {
        static_assert(detail::dependent_false_v<Dummy>, u8"Utf16ToUtf8 is not supported on this platform");
        return {};
    }

    template<typename Dummy = void>
    std::wstring Utf8ToUtf16(const std::string&) {
        static_assert(detail::dependent_false_v<Dummy>, u8"Utf8ToUtf16 is not supported on this platform");
        return {};
    }

} // namespace ImGuiX::Utils

#endif // ifdef _WIN32

#ifdef IMGUIX_HEADER_ONLY
#   include "encoding_utils.ipp"
#endif

#endif // _IMGUIX_UTILS_ENCODING_UTILS_HPP_INCLUDED
