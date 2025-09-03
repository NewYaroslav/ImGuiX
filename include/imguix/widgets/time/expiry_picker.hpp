#pragma once
#ifndef _IMGUIX_WIDGETS_EXPIRY_PICKER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_EXPIRY_PICKER_HPP_INCLUDED

/// \file expiry_picker.hpp
/// \brief Expiry picker with preset grid and H/M/S steppers.

#include <imgui.h>

#include <vector>

#include <imguix/widgets/controls/icon_combo.hpp>

namespace ImGuiX::Widgets {

    /// \brief Preset entry for expiry picker.
    struct ExpiryPreset {
        const char* label;   ///< Display label (e.g. "S5", "M1").
        int         seconds; ///< Value in seconds.
    };
    
    /// Inclusive seconds range [from, to].
    struct SecRange {
        int from = 0;
        int to   = 0;
    };

    /// \brief Default presets similar to the screenshot.
    /// \return Reference to built-in presets.
    const std::vector<ExpiryPreset>& DefaultExpiryPresets();

    /// \brief Configuration for ExpiryPicker.
    struct ExpiryPickerConfig {
        const char* label             = u8"Expiry"; ///< Combo label.
        float       combo_width       = 0.0f;       ///< Preview width (0 -> default).
        float       field_width       = 0.0f;       ///< H/M/S field width (0 -> auto).
        float       hms_gap           = -1.0f;      ///< Extra horizontal gap around ':' (px). <0 -> use style.ItemInnerSpacing.x
        float       cell_rounding     = 0.0f;       ///< 0 -> square. (<0 -> use style).
        bool        show_cell_borders = true;       ///< Draw cell borders.
        ImVec2      cell_size         = ImVec2(0, 0); ///< Preset cell size (0 -> frame height).
        int         rows              = 3;          ///< Preset grid rows.
        int         cols              = 3;          ///< Preset grid columns.
        
        bool        preset_highlight_selected = true; ///< If true, highlight currently selected preset button.

        const char* icon_text         = IMGUIX_ICON_TIMER; ///< Icon text for combo.
        float       icon_slot_width   = 0.0f;       ///< Width reserved for icon (0 -> auto).

        int min_seconds = 0;                        ///< Minimum allowed seconds.
        int max_seconds = 24 * 3600 - 1;            ///< Maximum allowed seconds.
        int step_seconds = 1;                       ///< Minimal step (quantization), e.g. 60
        
        // Forbidden inclusive ranges (e.g. {120,120} forbids 120s exactly).
        // Intervals are interpreted in seconds BEFORE quantization; picker will
        // snap to the nearest allowed quantized value per direction rules.
        const std::vector<SecRange>* forbidden = nullptr;

        const std::vector<ExpiryPreset>* presets = nullptr; ///< Optional custom presets.
    };

    /// \brief Expiry picker with H/M/S steppers and preset grid.
    /// \param id Unique widget identifier.
    /// \param seconds In/out expiry in seconds (clamped to [min,max]).
    /// \param cfg Widget configuration.
    /// \return True if value changed.
    bool ExpiryPicker(
            const char* id,
            int& seconds,
            const ExpiryPickerConfig& cfg = {});

#ifdef IMGUIX_DEMO
    /// \brief Render demo for expiry picker.
    void DemoExpiryPicker();
#endif

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "expiry_picker.ipp"
#endif

#endif // _IMGUIX_WIDGETS_EXPIRY_PICKER_HPP_INCLUDED
