#include <imgui.h>
#include <string>

#include <imguix/config/icons.hpp>
#include <imguix/config/colors.hpp>

namespace ImGuiX::Widgets {

void TooltipWrapped(const char* desc, float wrap_cols) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * wrap_cols);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
}

void IconMarker(
        const char* icon_utf8,
        const ImVec4& color,
        const char* desc,
        MarkerMode mode,
        float wrap_cols) {
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TextUnformatted(icon_utf8);
    ImGui::PopStyleColor();

    if (mode == MarkerMode::InlineText) {
        ImGui::SameLine();
        ImGui::TextWrapped("%s", desc);
    } else if (ImGui::IsItemHovered()) {
        TooltipWrapped(desc, wrap_cols);
    }
}

void ColoredMarker(
        const char* label,
        const char* desc,
        const ImVec4& color) {
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TextUnformatted(label);
    ImGui::PopStyleColor();
    if (ImGui::IsItemHovered())
        TooltipWrapped(desc);
}

bool SelectableMarker(const std::string& text) {
    bool clicked = ImGui::Selectable(text.c_str());
    if (ImGui::IsItemHovered())
        TooltipWrapped(text.c_str());
    return clicked;
}

void HelpMarker(
        const char* desc,
        MarkerMode mode,
        const char* icon_utf8) {
    ImGui::TextDisabled("%s", icon_utf8);
    if (mode == MarkerMode::InlineText) {
        ImGui::SameLine();
        ImGui::TextWrapped("%s", desc);
    } else if (ImGui::IsItemHovered()) {
        TooltipWrapped(desc);
    }
}

void WarningMarker(
        const char* desc,
        MarkerMode mode,
        const ImVec4& color,
        const char* icon_utf8) {
    IconMarker(icon_utf8, color, desc, mode);
}

void InfoMarker(
        const char* desc,
        MarkerMode mode,
        const ImVec4& color,
        const char* icon_utf8) {
    IconMarker(icon_utf8, color, desc, mode);
}

void SuccessMarker(
        const char* desc,
        MarkerMode mode,
        const ImVec4& color,
        const char* icon_utf8) {
    IconMarker(icon_utf8, color, desc, mode);
}

} // namespace ImGuiX::Widgets
