#pragma once
#ifndef _IMGUIX_EXTENSIONS_BLEND_COLORS_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_BLEND_COLORS_HPP_INCLUDED

/// \file blend_colors.hpp
/// \brief Provides alpha-blending utility for combining ImVec4 colors.

#include <imgui.h>

namespace ImGuiX::Extensions {

    /// \brief Blend two colors using source-over alpha compositing.
    /// \param fg Foreground color.
    /// \param bg Background color.
    /// \return Blended color.
    /// \note Result equals foreground over background.
    inline ImVec4 BlendColors(const ImVec4& fg, const ImVec4& bg);

} // namespace ImGuiX::Extensions

#ifdef IMGUIX_HEADER_ONLY
#   include "blend_colors.ipp"
#endif

#endif // _IMGUIX_EXTENSIONS_BLEND_COLORS_HPP_INCLUDED
