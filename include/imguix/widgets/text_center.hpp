#pragma once
#ifndef _IMGUIX_WIDGETS_TEXT_CENTER_HPP_INCLUDED
#define _IMGUIX_WIDGETS_TEXT_CENTER_HPP_INCLUDED

// \file text_center.hpp
/// \brief Helpers to center text within the current content region (single-line, unformatted, wrapped).
///
/// ImGui 1.92.* compatible: uses only public API + <cstdio> for formatting.

#include <imgui.h>
#include <algorithm>
#include <string>
#include <cstdarg>
#include <cstdio>

namespace ImGuiX::Widgets {

    /// \brief Center a single-line formatted text (uses ImGui::Text).
    /// \note If text width > available width, falls back to left align.
    inline void TextCenteredFmt(const char* fmt, ...) {
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        std::vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);

        const float avail_w = ImGui::GetContentRegionAvail().x;
        const float text_w  = ImGui::CalcTextSize(buf).x;

        const float x0 = ImGui::GetCursorPosX();
        const float x  = (text_w < avail_w) ? x0 + (avail_w - text_w) * 0.5f : x0;
        ImGui::SetCursorPosX(x);
        ImGui::TextUnformatted(buf);
    }

    /// \brief Center an unformatted string (no printf parsing).
    inline void TextUnformattedCentered(const char* text) {
        if (!text) return;
        const float avail_w = ImGui::GetContentRegionAvail().x;
        const float text_w  = ImGui::CalcTextSize(text).x;

        const float x0 = ImGui::GetCursorPosX();
        const float x  = (text_w < avail_w) ? x0 + (avail_w - text_w) * 0.5f : x0;
        ImGui::SetCursorPosX(x);
        ImGui::TextUnformatted(text);
    }

    /// \brief Center a wrapped text block by placing it into a centered child with given width.
    /// \param text Text to render.
    /// \param wrap_width Desired wrap width in pixels (<= avail). If <= 0, uses avail width.
    ///
    /// Принцип: создаём «вкладыш»-child нужной ширины и центрируем его по горизонтали,
    /// а внутри используем обычный TextWrapped (левый край), что даёт визуально центрированный блок.
    inline void TextWrappedCentered(const char* text, float wrap_width = 0.0f) {
        if (!text) return;
        const float avail_w = ImGui::GetContentRegionAvail().x;
        float block_w = (wrap_width > 0.0f) ? std::min(wrap_width, avail_w) : avail_w;

        const float x0 = ImGui::GetCursorPosX();
        const float x  = x0 + (avail_w - block_w) * 0.5f;
        ImGui::SetCursorPosX(x);

        ImGui::BeginChild("##TextWrappedCentered", ImVec2(block_w, 0.0f),
                          false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextWrapped("%s", text);
        ImGui::PopTextWrapPos();
        ImGui::EndChild();
    }
    
    /// \brief Overload for std::string (unformatted).
    inline void TextUnformattedCentered(const std::string& s) {
        TextUnformattedCentered(s.c_str());
    }

    /// \brief Overload for std::string (wrapped).
    inline void TextWrappedCentered(const std::string& s, float wrap_width = 0.0f) {
        TextWrappedCentered(s.c_str(), wrap_width);
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_TEXT_CENTER_HPP_INCLUDED
