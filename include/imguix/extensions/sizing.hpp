#pragma once
#ifndef _IMGUIX_EXTENSIONS_SIZING_HPP_INCLUDED
#define _IMGUIX_EXTENSIONS_SIZING_HPP_INCLUDED

/// \file sizing.hpp
/// \brief Helpers to compute widths for Combo preview, fields, and time widgets.

#include <imgui.h>
#include <imguix/config/sizing.hpp>

namespace ImGuiX::Extensions {

    /// \brief Width of the combo arrow button for the current style.
    /// \return Arrow width in pixels.
    inline float ComboArrowWidth() {
        return ImGui::GetFrameHeight();
    }

    /// \brief Maximum usable width for preview text inside a Combo of given width.
    /// \param combo_width  Total width of the combo item (SetNextItemWidth / CalcItemWidth).
    /// \param flags        Combo flags (respects NoArrowButton/NoPreview).
    /// \param extra_slack  Small safety padding to avoid clipping on fractional pixels.
    /// \return Maximum preview text width.
    inline float CalcComboPreviewTextMax(
        float combo_width,
        ImGuiComboFlags flags = 0,
        float extra_slack = 1.0f
    ) {
        const ImGuiStyle& st = ImGui::GetStyle();
        const bool no_arrow   = (flags & ImGuiComboFlags_NoArrowButton) != 0;
        const bool no_preview = (flags & ImGuiComboFlags_NoPreview)     != 0;
        if (no_preview) return 0.0f;

        const float arrow_w   = no_arrow ? 0.0f : ComboArrowWidth();
        const float inner_gap = no_arrow ? 0.0f : st.ItemInnerSpacing.x;

        float w = combo_width - (2.0f * st.FramePadding.x + inner_gap + arrow_w) - extra_slack;
        return std::max(0.0f, w);
    }

    /// \brief Compute minimal combo width to fit a given preview string (with arrow, padding, spacing).
    /// \param preview_text  Text to fit (UTF-8).
    /// \param flags         Combo flags (respects NoArrowButton/NoPreview).
    /// \param extra_slack   Additional safety space (usually small, e.g. ItemSpacing.x * 0.5).
    /// \return Combo width in pixels.
    inline float CalcComboWidthForPreview(
            const char* preview_text,
            ImGuiComboFlags flags = 0,
            float extra_slack = -1.0f
        ) {
        const ImGuiStyle& st = ImGui::GetStyle();
        const bool no_arrow   = (flags & ImGuiComboFlags_NoArrowButton) != 0;
        const bool no_preview = (flags & ImGuiComboFlags_NoPreview)     != 0;

        const float arrow_w   = no_arrow ? 0.0f : ComboArrowWidth();
        const float inner_gap = no_arrow ? 0.0f : st.ItemInnerSpacing.x;
        if (extra_slack < 0.0f) extra_slack = st.ItemSpacing.x * 0.5f;

        const float text_w = no_preview ? 0.0f : ImGui::CalcTextSize(preview_text).x;
        return text_w + 2.0f * st.FramePadding.x + inner_gap + arrow_w + extra_slack;
    }

    /// \brief Compute width for a fixed-width numeric field (e.g., HH/MM/SS).
    /// \param sample_text  Text sample to fit (e.g., "+88" or "88").
    /// \param extra_slack  Small safety space (default: ItemSpacing.x * 0.25).
    /// \return Field width in pixels.
    inline float CalcFieldWidthForSample(const char* sample_text, float extra_slack = -1.0f) {
        const ImGuiStyle& st = ImGui::GetStyle();
        if (extra_slack < 0.0f) extra_slack = st.ItemSpacing.x * 0.25f;
        return ImGui::CalcTextSize(sample_text).x + 2.0f * st.FramePadding.x + extra_slack;
    }

    /// \brief Recommended combo width for time-of-day preview ("23:59:59").
    /// \param signed_preview If true, includes '+' (use for offsets like "+23:59:59").
    /// \param flags          Combo flags (respects NoArrowButton/NoPreview).
    /// \return Combo width in pixels.
    inline float CalcTimeComboWidth(
            bool signed_preview = true,
            ImGuiComboFlags flags = 0
        ) {
        return CalcComboWidthForPreview(signed_preview ? IMGUIX_SIZING_TIME_SIGNED : IMGUIX_SIZING_TIME_UNSIGNED,
                                        flags, ImGui::GetStyle().ItemSpacing.x * 0.5f);
    }

    /// \brief Recommended field width for HH/MM/SS steppers (fits "+88").
    /// \return Field width in pixels.
    inline float CalcHMSFieldWidth() {
        return CalcFieldWidthForSample(IMGUIX_SIZING_HMS, ImGui::GetStyle().ItemSpacing.x * 0.25f);
    }

    /// \brief Recommended combo width for date preview like "Mon +8888-88-88".
    /// \param flags Combo flags (respects NoArrowButton/NoPreview).
    /// \return Combo width in pixels.
    inline float CalcDateComboWidth(
            ImGuiComboFlags flags = 0
        ) {
                return CalcComboWidthForPreview(
                        IMGUIX_SIZING_DATE, flags, ImGui::GetStyle().ItemSpacing.x * 0.5f
                );
	}

    /// \brief Recommended field width for a year edit (fits "+8888" or "8888").
    /// \param signed_year If true, reserves '+' for signed years (offsets).
    /// \return Field width in pixels.
    inline float CalcYearFieldWidth(bool signed_year = true) {
        return CalcFieldWidthForSample(
                        signed_year ? IMGUIX_SIZING_YEAR_SIGNED : IMGUIX_SIZING_YEAR_UNSIGNED,
                        ImGui::GetStyle().ItemSpacing.x * 0.25f
                );
    }
	
    /// \brief Recommended combo width for weekday name preview.
    /// \param flags Combo flags (respects NoArrowButton/NoPreview).
    /// \return Combo width in pixels.
    inline float CalcWeekdayComboWidth(ImGuiComboFlags flags = 0) {
        return CalcComboWidthForPreview(
                IMGUIX_SIZING_WEEKDAYS,
                flags,
                ImGui::GetStyle().ItemSpacing.x * 0.5f);
    }

} // namespace ImGuiX::Extensions

#endif // _IMGUIX_EXTENSIONS_SIZING_HPP_INCLUDED
