#pragma once
#ifndef _IMGUIX_EXTENSIONS_COLOR_UTILS_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_COLOR_UTILS_HPP_INCLUDED

/// \file color_utils.hpp
/// \brief Utilities for color conversions between ImGui and SFML.

#include <imgui.h>
#include <cstdint>

#if defined(IMGUIX_USE_SFML_BACKEND)
    #include <SFML/Graphics/Color.hpp>
#endif

namespace ImGuiX::Extensions {

#if defined(IMGUIX_USE_SFML_BACKEND)

    /// \brief Convert ImVec4 color to sf::Color.
    /// \param color Color with components in [0,1].
    /// \return 8-bit RGBA color.
    inline sf::Color ColorToSfml(const ImVec4& color);

#endif // IMGUIX_USE_SFML_BACKEND

    /// \brief Clamp value to [0,1].
    /// \param v Value to clamp.
    /// \return Clamped value.
    inline float Clamp01(float v) {
        return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
    }

    /// \brief Convert 0..255 byte to normalized float 0..1.
    /// \param x 0..255 value.
    /// \return Normalized float.
    inline float ByteToFloat(std::uint8_t x) {
        return static_cast<float>(x) / 255.0f;
    }

    /// \brief Build ImVec4 from 8-bit channels. Alpha defaults to 255.
    /// \param r Red channel 0..255.
    /// \param g Green channel 0..255.
    /// \param b Blue channel 0..255.
    /// \param a_alpha Alpha channel 0..255.
    /// \return Resulting color.
    inline ImVec4 RgbU8(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a_alpha = 255) {
        return ImVec4(ByteToFloat(r), ByteToFloat(g), ByteToFloat(b), ByteToFloat(a_alpha));
    }

    /// \brief Build ImVec4 from 0xRRGGBB and float alpha.
    /// \param hex 0xRRGGBB value.
    /// \param a Alpha in [0,1].
    /// \return Resulting color.
    inline ImVec4 RgbHex(std::uint32_t hex, float a = 1.0f) {
        return ImVec4(
            ((hex >> 16) & 0xFF) / 255.0f,
            ((hex >>  8) & 0xFF) / 255.0f,
            ((hex      ) & 0xFF) / 255.0f,
            Clamp01(a)
        );
    }

    /// \brief Build ImVec4 from 0xRRGGBBAA (trailing AA).
    /// \param hex_rgba 0xRRGGBBAA value.
    /// \return Resulting color.
    inline ImVec4 RgbaHex(std::uint32_t hex_rgba) {
        const std::uint8_t r = (hex_rgba >> 24) & 0xFF;
        const std::uint8_t g = (hex_rgba >> 16) & 0xFF;
        const std::uint8_t b = (hex_rgba >>  8) & 0xFF;
        const std::uint8_t a = (hex_rgba      ) & 0xFF;
        return RgbU8(r, g, b, a);
    }

    /// \brief Replace alpha while preserving RGB.
    /// \param c Base color.
    /// \param a New alpha in [0,1].
    /// \return Color with replaced alpha.
    inline ImVec4 WithAlpha(ImVec4 c, float a) {
        c.w = Clamp01(a);
        return c;
    }

    /// \brief Multiply alpha.
    /// \param c Base color.
    /// \param k Alpha multiplier.
    /// \return Color with scaled alpha.
    inline ImVec4 MulAlpha(ImVec4 c, float k) {
        c.w = Clamp01(c.w * k);
        return c;
    }

    /// \brief Convert ImVec4 to packed U32 for ImDrawList.
    /// \param c Color to convert.
    /// \return Packed color.
    inline ImU32 ToU32(const ImVec4& c) {
        return ImGui::ColorConvertFloat4ToU32(c);
    }

    /// \brief Lighten or darken via HSV value multiplier.
    /// \param c Base color.
    /// \param k Value multiplier (>1 lighten, <1 darken).
    /// \return Color with scaled value.
    inline ImVec4 ScaleValue(ImVec4 c, float k) {
        float h, s, v;
        ImGui::ColorConvertRGBtoHSV(c.x, c.y, c.z, h, s, v);
        v = Clamp01(v * k);
        ImGui::ColorConvertHSVtoRGB(h, s, v, c.x, c.y, c.z);
        return c;
    }

    /// \brief Change saturation via HSV multiplier.
    /// \param c Base color.
    /// \param k Saturation multiplier.
    /// \return Color with scaled saturation.
    inline ImVec4 ScaleSaturation(ImVec4 c, float k) {
        float h, s, v;
        ImGui::ColorConvertRGBtoHSV(c.x, c.y, c.z, h, s, v);
        s = Clamp01(s * k);
        ImGui::ColorConvertHSVtoRGB(h, s, v, c.x, c.y, c.z);
        return c;
    }

} // namespace ImGuiX::Extensions

#ifdef IMGUIX_HEADER_ONLY
#   include "color_utils.ipp"
#endif

#endif // _IMGUIX_EXTENSIONS_COLOR_UTILS_HPP_INCLUDED
