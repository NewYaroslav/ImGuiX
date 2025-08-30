#pragma once
#ifndef _IMGUIX_UTILS_VFORMAT_HPP_INCLUDED
#define _IMGUIX_UTILS_VFORMAT_HPP_INCLUDED

/// \file format.hpp
/// \brief Function for formatting strings according to a specified format.

#include <cstdarg>
#include <vector>
#include <string>

namespace ImGuiX::Utils {

    /// \brief vsnprintf-based formatter using a va_list (robust, resizes buffer).
    ///
    /// This function formats a string using either the custom implementation based on `vsnprintf`).
    ///
    /// \param fmt The format string (similar to printf format).
    /// \param ... A variable number of arguments matching the format string.
    /// \see https://habr.com/ru/articles/318962/
    /// \return A formatted std::string.
    
    /// \brief vsnprintf-based formatter using a va_list (robust, resizes buffer).
    /// \param fmt printf-like format string.
    /// \param args argument list (will be copied internally).
    /// \return Formatted string.
    inline std::string vformat_va(const char *fmt, va_list args) {
        if (!fmt) return {};
        std::vector<char> buffer(1024);
        for (;;) {
            va_list args_copy;
            va_copy(args_copy, args); // Copy args to prevent modifying the original list.
            int res = std::vsnprintf(buffer.data(), buffer.size(), fmt, args_copy);
            va_end(args_copy); // Clean up the copied argument list.

            if ((res >= 0) && (res < static_cast<int>(buffer.size()))) {
                return std::string(buffer.data(), static_cast<size_t>(res));
            }

            // If the buffer was too small, resize it.
            const size_t size = res < 0 ? buffer.size() * 2 : static_cast<size_t>(res) + 1;
            buffer.clear();
            buffer.resize(size);
        }
    }
    
    /// \brief printf-style formatter (varargs) built on top of vformat_va.
    inline std::string vformat(const char* fmt, ...) {
        if (!fmt) return {};
        va_list args; va_start(args, fmt);
        std::string s = vformat_va(fmt, args);
        va_end(args);
        return s;
    }

}; // namespace ImGuiX::Utils

#endif // _IMGUIX_UTILS_VFORMAT_HPP_INCLUDED
