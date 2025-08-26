#pragma once
#ifndef _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED
#define _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED

/// \file markers.hpp
/// \brief Assorted marker widgets for colored labels and tooltips.

#include <imgui.h>
#include <string>
#include <cstddef>

namespace ImGuiX::Widgets {
    
    enum class MarkerMode : int {
        TooltipOnly = 0,  ///< icon/label + tooltip on hover
        InlineText  = 1   ///< icon/label + inline wrapped text
    };

    inline void TooltipWrapped(const char* desc, float wrap_cols = 35.0f) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * wrap_cols);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
    
    inline void IconMarker(
            const char* icon_utf8, 
            const ImVec4& color,
            const char* desc, 
            MarkerMode mode, 
            float wrap_cols = 35.0f
        ) {
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(icon_utf8);
        ImGui::PopStyleColor();

        if (mode == MarkerMode::InlineText) {
            ImGui::SameLine();
            ImGui::TextWrapped("%s", desc);
        } else 
        if (ImGui::IsItemHovered()) {
            TooltipWrapped(desc, wrap_cols);
        }
    }

    /// \brief Draw label in custom color with tooltip.
    /// \param label Text displayed on screen.
    /// \param desc Tooltip text shown when hovered.
    /// \param color Text color.
    inline void ColoredMarker(
            const char* label, 
            const char* desc, 
            const ImVec4& color
        ) {
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered()) TooltipWrapped(desc);
    }

    /// \brief Selectable label with tooltip echoing its text.
    /// \param text Text shown and displayed in tooltip.
    /// \return True if selected.
    inline bool SelectableMarker(const std::string& text) {
        bool clicked = ImGui::Selectable(text.c_str());
        if (ImGui::IsItemHovered()) TooltipWrapped(text.c_str());
        return clicked;
    }

    /// \brief Help marker using a question icon with tooltip.
    /// \param desc Help text.
    inline void HelpMarker(
            const char* desc, 
            MarkerMode mode = MarkerMode::TooltipOnly, 
            const char* icon_utf8 = u8"\uE887"
        ) {
        ImGui::TextDisabled(icon_utf8);
        if (mode == MarkerMode::InlineText) {
            ImGui::SameLine();
            ImGui::TextWrapped("%s", desc);
        } else if (ImGui::IsItemHovered()) {
            TooltipWrapped(desc);
        }
    }

    /// \brief Warning marker with yellow icon and text.
    /// \param desc Warning message.
    inline void WarningMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly, 
            const ImVec4& color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
            const char* icon_utf8 = u8"\uE002"
        ) {
        IconMarker(icon_utf8, color, desc, mode);
    }

    /// \brief Info marker with blue icon and text.
    /// \param desc Information message.
    inline void InfoMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const ImVec4& color = ImVec4(0.10f, 0.45f, 0.95f, 1.0f),
            const char* icon_utf8 = u8"\uE88E"
        ) {
        IconMarker(icon_utf8, color, desc, mode);
    }

    /// \brief Success marker with green icon and text.
    /// \param desc Success message.
    inline void SuccessMarker(
            const char* desc,
            MarkerMode mode = MarkerMode::TooltipOnly,
            const ImVec4& color = ImVec4(0.0f, 0.60f, 0.0f, 1.0f),
            const char* icon_utf8 = u8"\uE88E"
        ) {
        IconMarker(icon_utf8, color, desc, mode);
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED
