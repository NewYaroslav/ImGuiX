#include <algorithm>
#include <cstdarg>
#include <cstdio>

namespace ImGuiX::Widgets {

    void TextCenteredFmt(const char* fmt, ...) {
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

    void TextUnformattedCentered(const char* text) {
        if (!text) return;
        const float avail_w = ImGui::GetContentRegionAvail().x;
        const float text_w  = ImGui::CalcTextSize(text).x;

        const float x0 = ImGui::GetCursorPosX();
        const float x  = (text_w < avail_w) ? x0 + (avail_w - text_w) * 0.5f : x0;
        ImGui::SetCursorPosX(x);
        ImGui::TextUnformatted(text);
    }

    void TextWrappedCentered(const char* text, float wrap_width) {
        if (!text) return;

        const float avail_w = ImGui::GetContentRegionAvail().x;
        const float block_w = (wrap_width > 0.0f) ? std::min(wrap_width, avail_w) : avail_w;

        const float x0 = ImGui::GetCursorPosX();
        const float x  = x0 + (avail_w - block_w) * 0.5f; // центрируем «контейнер»

        ImGui::SetCursorPosX(x);
        // wrap позиция задаётся в ЛОКАЛЬНЫХ координатах текущего окна:
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + block_w);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
    }

} // namespace ImGuiX::Widgets
