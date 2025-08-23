#pragma once
#ifndef _IMGUIX_WIDGETS_ICON_BUTTON_HPP_INCLUDED
#define _IMGUIX_WIDGETS_ICON_BUTTON_HPP_INCLUDED

#include <imgui.h>

namespace ImGuiX::Widgets {

    /// \brief Configuration for IconButtonCentered.
    struct IconButtonConfig {
        ImVec2  size{0, 0};           ///< (0,0) => square with height = GetFrameHeight()
        float   rounding{-1.0f};      ///< Background rounding, -1 to use style
        ImFont* font{nullptr};        ///< Font for text/icons; null uses current
        ImVec2  text_offset{0, 0};    ///< Baseline adjustment for text
        bool    draw_border{false};   ///< Draw border around button
        ImU32   border_col{0};        ///< Border color, 0 => ImGuiCol_Border
        float   border_thickness{1.0f}; ///< Border thickness in pixels
    };

    /// \brief Button with centered text or icon.
    /// \param id Unique widget identifier.
    /// \param text UTF-8 text or icon string.
    /// \param cfg Button appearance options.
    /// \return True if clicked with left mouse button.
    inline bool IconButtonCentered(const char* id,
                                   const char* text,
                                   const IconButtonConfig& cfg = {}) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 p0 = ImGui::GetCursorScreenPos();

        ImVec2 sz = cfg.size;
        if (sz.x <= 0.0f || sz.y <= 0.0f) {
            const float h = ImGui::GetFrameHeight();
            sz = ImVec2(h, h); // square sized to current frame height
        }

        ImGui::InvisibleButton(id, sz);
        const bool hovered = ImGui::IsItemHovered();
        const bool held    = ImGui::IsItemActive();
        const bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);

        const ImGuiStyle& style = ImGui::GetStyle();
        float rounding = (cfg.rounding < 0.0f) ? style.FrameRounding : cfg.rounding;

        const ImU32 bg = ImGui::GetColorU32(
            held ? ImGuiCol_ButtonActive :
            hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        dl->AddRectFilled(p0, ImVec2(p0.x + sz.x, p0.y + sz.y), bg, rounding);

        if (cfg.draw_border) {
            const ImU32 bc = cfg.border_col ? cfg.border_col : ImGui::GetColorU32(ImGuiCol_Border);
            dl->AddRect(p0, ImVec2(p0.x + sz.x, p0.y + sz.y), bc, rounding, 0, cfg.border_thickness);
        }

        if (cfg.font) ImGui::PushFont(cfg.font);
        const ImVec2 ts = ImGui::CalcTextSize(text);

        auto snap = [](float v) { return std::floor(v) + 0.5f; };
        const float tx = snap(p0.x + (sz.x - ts.x) * 0.5f + cfg.text_offset.x);
        const float ty = snap(p0.y + (sz.y - ts.y) * 0.5f + cfg.text_offset.y);
        dl->AddText(ImVec2(tx, ty), ImGui::GetColorU32(ImGuiCol_Text), text);
        if (cfg.font) ImGui::PopFont();

        return clicked;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_ICON_BUTTON_HPP_INCLUDED
