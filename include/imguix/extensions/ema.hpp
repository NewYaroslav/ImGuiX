#pragma once
#ifndef _IMGUIX_EXTENSIONS_EMA_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_EMA_HPP_INCLUDED

/// \file ema.hpp
/// \brief Exponential moving average helper with per-ID state in ImGui storage.

#include <imgui.h>

namespace ImGuiX::Extensions {

    /// \brief Exponential moving average stored in ImGui storage.
    class Ema {
    public:
        static constexpr float kDefaultAlpha = 0.1f;

        /// \brief Reset stored value.
        /// \param id Storage identifier.
        /// \param value Value to set as current EMA.
        static void reset(ImGuiID id, float value);

        /// \brief Update EMA with new sample.
        /// \param id Storage identifier.
        /// \param sample New data point.
        /// \param alpha Smoothing factor [0,1].
        /// \return Updated EMA value.
        static float update(
                ImGuiID id,
                float sample,
                float alpha = kDefaultAlpha);

        /// \brief Get current EMA value.
        /// \param id Storage identifier.
        /// \param default_value Fallback if EMA not yet initialized.
        /// \return Stored EMA value or default_value if unset.
        static float value(ImGuiID id, float default_value = 0.0f);
    };

} // namespace ImGuiX::Extensions

#ifdef IMGUIX_HEADER_ONLY
#include "ema.ipp"
#endif

#endif // _IMGUIX_EXTENSIONS_EMA_HPP_INCLUDED
