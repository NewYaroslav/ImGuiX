#pragma once
#ifndef _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED
#define _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED

/// \file markers.hpp
/// \brief Assorted marker widgets for colored labels and tooltips.

#include <imgui.h>
#include <string>
#include <vector>

#include <imguix/config/icons.hpp>
#include <imguix/config/colors.hpp>

namespace ImGuiX::Widgets {

    /// \brief Marker display mode.
    enum class MarkerMode : int {
        TooltipOnly = 0,  ///< Icon/label with tooltip on hover.
        InlineText  = 1   ///< Icon/label with inline wrapped text.
    };

    /// \brief Show tooltip text with word wrapping.
    /// \param desc Tooltip text.
    /// \param wrap_cols Column width for wrapping.
    void TooltipWrapped(const char* desc, float wrap_cols = 35.0f);

    /// \brief Draw icon marker with tooltip or inline text.
    /// \param icon_utf8 Icon UTF-8 string.
    /// \param color Icon color.
    /// \param desc Tooltip or inline text.
    /// \param mode Display mode.
    /// \param wrap_cols Column width for wrapping.
    void IconMarker(
            const char* icon_utf8,
            const ImVec4& color,
            const char* desc,
            MarkerMode mode,
            float wrap_cols = 35.0f
        );

    /// \brief Draw label in custom color with tooltip.
    /// \param label Text displayed on screen.
    /// \param desc Tooltip text shown when hovered.
    /// \param color Text color.
    void ColoredMarker(
            const char* label,
            const char* desc,
            const ImVec4& color
        );

    /// \brief Selectable label with tooltip echoing its text.
    /// \param text Text shown and displayed in tooltip.
    /// \return True if selected.
    bool SelectableMarker(const std::string& text);

    /// \brief Help marker using a question icon with tooltip.
    /// \param desc Help text.
    /// \param mode Display mode.
    /// \param icon_utf8 Icon glyph.
    void HelpMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const char* icon_utf8 = IMGUIX_ICON_HELP
        );

    /// \brief Warning marker with yellow icon and text.
    /// \param desc Warning message.
    /// \param mode Display mode.
    /// \param color Icon and text color.
    /// \param icon_utf8 Icon glyph.
    void WarningMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const ImVec4& color = IMGUIX_COLOR_WARNING,
            const char* icon_utf8 = IMGUIX_ICON_WARNING
        );

    /// \brief Info marker with blue icon and text.
    /// \param desc Information message.
    /// \param mode Display mode.
    /// \param color Icon and text color.
    /// \param icon_utf8 Icon glyph.
    void InfoMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const ImVec4& color = IMGUIX_COLOR_INFO,
            const char* icon_utf8 = IMGUIX_ICON_INFO
        );

    /// \brief Success marker with green icon and text.
    /// \param desc Success message.
    /// \param mode Display mode.
    /// \param color Icon and text color.
    /// \param icon_utf8 Icon glyph.
    void SuccessMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const ImVec4& color = IMGUIX_COLOR_SUCCESS,
            const char* icon_utf8 = IMGUIX_ICON_SUCCESS
        );

#ifdef IMGUIX_DEMO
    /// \brief Render demo for marker widgets.
    inline void DemoMarkers() {
        ImGui::TextDisabled("Colored markers:");
        ColoredMarker("OK",         "All good",             ImVec4(0.10f, 0.75f, 0.30f, 1.0f));
        ImGui::SameLine();
        ColoredMarker("WARNING",    "Spread high",          ImVec4(0.95f, 0.75f, 0.10f, 1.0f));
        ImGui::SameLine();
        ColoredMarker("ERROR",      "Disconnected",         ImVec4(0.95f, 0.25f, 0.25f, 1.0f));
        ImGui::SameLine();
        ColoredMarker("SIMULATION", "Demo / paper trading", ImVec4(0.70f, 0.70f, 0.80f, 1.0f));

        ImGui::Separator();

        ImGui::TextDisabled("Message markers:");
        ImGui::TextUnformatted("Help:");
        ImGui::SameLine();
        HelpMarker("Trading server to place real orders.\nUse with care.");

        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 2);
        ImGui::TextUnformatted("Info:");
        ImGui::SameLine();
        InfoMarker("New version available: v1.4.2. Update when idle.");

        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 2);
        ImGui::TextUnformatted("Warn:");
        ImGui::SameLine();
        WarningMarker("Funding API rate-limit almost reached; consider backoff.");

        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 2);
        ImGui::TextUnformatted("Success:");
        ImGui::SameLine();
        SuccessMarker("License validated. All features enabled.");
        SuccessMarker("License validated.", MarkerMode::InlineText);

        ImGui::Separator();

        ImGui::TextDisabled("Selectable markers:");
        static std::vector<std::string> presets = {
            "London Session", "New York Session", "Asia Session", "Custom Range"
        };
        static int last_clicked = -1;
        for (int i = 0; i < (int)presets.size(); ++i) {
            if (SelectableMarker(presets[i])) {
                last_clicked = i;
            }
        }
        ImGui::Separator();
        ImGui::Text("Last clicked: %s", (last_clicked >= 0 ? presets[last_clicked].c_str() : "none"));
    }
#endif

} // namespace ImGuiX::Widgets
#ifdef IMGUIX_HEADER_ONLY
#   include "markers.ipp"
#endif

#endif // _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED
