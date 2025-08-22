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
    inline ImVec4 BlendColors(const ImVec4& fg, const ImVec4& bg) {
        float a = fg.w + bg.w * (1.0f - fg.w);
        if (a <= 0.0f)
            return ImVec4(0, 0, 0, 0);
        return ImVec4(
            (fg.x * fg.w + bg.x * bg.w * (1.0f - fg.w)) / a,
            (fg.y * fg.w + bg.y * bg.w * (1.0f - fg.w)) / a,
            (fg.z * fg.w + bg.z * bg.w * (1.0f - fg.w)) / a,
            a
        );
    }

} // namespace ImGuiX::Extensions

#endif // _IMGUIX_EXTENSIONS_BLEND_COLORS_HPP_INCLUDED