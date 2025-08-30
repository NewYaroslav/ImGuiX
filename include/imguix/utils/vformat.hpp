#pragma once
#ifndef _IMGUIX_UTILS_VFORMAT_HPP_INCLUDED
#define _IMGUIX_UTILS_VFORMAT_HPP_INCLUDED

/// \file vformat.hpp
/// \brief String formatting helpers using printf semantics.

#include <cstdarg>
#include <vector>
#include <string>

namespace ImGuiX::Utils {

    /// \brief Format string using a va_list.
    /// \param fmt printf-like format string.
    /// \param args Argument list copied internally.
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
    
    /// \brief Format string using variable arguments.
    /// \param fmt printf-like format string.
    /// \param ... Arguments to format.
    /// \return Formatted string.
    inline std::string vformat(const char* fmt, ...) {
        if (!fmt) return {};
        va_list args; va_start(args, fmt);
        std::string s = vformat_va(fmt, args);
        va_end(args);
        return s;
    }

}; // namespace ImGuiX::Utils

#endif // _IMGUIX_UTILS_VFORMAT_HPP_INCLUDED
