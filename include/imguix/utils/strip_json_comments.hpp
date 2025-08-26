#pragma once
#ifndef _IMGUIX_UTILS_STRIP_JSON_COMMENTS_HPP_INCLUDED
#define _IMGUIX_UTILS_STRIP_JSON_COMMENTS_HPP_INCLUDED

/// \file strip_json_comments.hpp
/// \brief Remove //, /* ... */, and # line comments from JSON-like text outside strings.
/// \note JSON standard does not allow comments; this acts as a preprocessor.
/// \note '#' comments apply only at line start or after whitespace.
/// \note Inside string literals nothing is stripped; escaped quotes \" are honored.
/// \note When with_whitespace is true, removed characters become spaces while newlines stay.

#include <string>

namespace ImGuiX::Utils {

    /// \brief Strip comments from JSON-like string.
    /// \param json_string Input string (can be empty).
    /// \param with_whitespace If true, replaced comment regions are filled with spaces (newlines preserved).
    /// \return String with comments removed.
    std::string strip_json_comments(
            const std::string& json_string,
            const bool with_whitespace = false
        ) noexcept;

} // namespace ImGuiX::Utils

#ifdef IMGUIX_HEADER_ONLY
#   include "strip_json_comments.ipp"
#endif

#endif // _IMGUIX_UTILS_STRIP_JSON_COMMENTS_HPP_INCLUDED
