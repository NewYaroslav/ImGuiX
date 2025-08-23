#pragma once
#ifndef _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED
#define _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED

/// \file markers.hpp
/// \brief Assorted marker widgets for colored labels and tooltips.

#include <string>

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Draw label in custom color with tooltip.
    /// \param label Text displayed on screen.
    /// \param desc Tooltip text shown when hovered.
    /// \param color Text color.
    inline void ColoredMarker(const char* label,
                               const char* desc,
                               const ImVec4& color) {
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    /// \brief Selectable label with tooltip echoing its text.
    /// \param text Text shown and displayed in tooltip.
    /// \return True if selected.
    inline bool SelectableMarker(const std::string& text) {
        bool clicked = ImGui::Selectable(text.c_str());
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(text.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        return clicked;
    }

    /// \brief Help marker using a question icon with tooltip.
    /// \param desc Help text.
    inline void HelpMarker(const char* desc) {
        ImGui::TextDisabled(u8"\uE887");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    /// \brief Warning marker with yellow icon and text.
    /// \param desc Warning message.
    inline void WarningMarker(const char* desc) {
        const ImVec4 kYellow{1.0f, 1.0f, 0.0f, 1.0f};
        ImGui::PushStyleColor(ImGuiCol_Text, kYellow);
        ImGui::TextUnformatted(u8"\u26A0");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextWrapped("%s", desc);
    }

    /// \brief Info marker with blue icon and text.
    /// \param desc Information message.
    inline void InfoMarker(const char* desc) {
        const ImVec4 kBlue{0.1f, 0.45f, 0.95f, 1.0f};
        ImGui::PushStyleColor(ImGuiCol_Text, kBlue);
        ImGui::TextUnformatted(u8"\uE88E");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextWrapped("%s", desc);
    }

    /// \brief Success marker with green icon and text.
    /// \param desc Success message.
    inline void SuccessMarker(const char* desc) {
        const ImVec4 kGreen{0.0f, 0.6f, 0.0f, 1.0f};
        ImGui::PushStyleColor(ImGuiCol_Text, kGreen);
        ImGui::TextUnformatted(u8"\uE86C");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextWrapped("%s", desc);
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_MARKERS_HPP_INCLUDED
