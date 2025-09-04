#include <regex>

namespace ImGuiX::Extensions {

    inline bool RegexMatchSafe(const std::string& s, const std::string& pattern) {
        if (pattern.empty())
            return true;
        try {
            std::regex re(pattern);
            return std::regex_match(s, re);
        } catch (...) {
            return true; // invalid regex -> don't block UI
        }
    }

    inline int BeginInvalid(
            bool is_invalid,
            ImVec4 color,
            InvalidTintMode mode,
            bool include_border
        ) {
        if (!is_invalid) return 0;

        int pushed = 0;
        if (mode == InvalidTintMode::Frame) {
            ImGui::PushStyleColor(ImGuiCol_FrameBg,        color); ++pushed;
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, color); ++pushed;
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  color); ++pushed;
            if (include_border) {
                ImGui::PushStyleColor(ImGuiCol_Border, color);    ++pushed;
            }
        } else { // Text
            ImGui::PushStyleColor(ImGuiCol_Text, color); ++pushed;
        }
        return pushed;
    }

    inline void BeginInvalid(bool is_invalid,
                             ImVec4 color,
                             ImGuiCol target)
    {
        if (is_invalid)
            ImGui::PushStyleColor(target, color);
    }

    inline void EndInvalid(bool is_invalid, int count) {
        if (is_invalid && count > 0)
            ImGui::PopStyleColor(count);
    }

    inline void EndInvalid(bool is_invalid) {
        if (is_invalid)
            ImGui::PopStyleColor();
    }

    inline ScopedInvalid::ScopedInvalid(bool is_invalid,
                                        ImVec4 color,
                                        InvalidTintMode mode,
                                        bool include_border)
        : m_active(is_invalid)
    {
        if (m_active)
            m_count = BeginInvalid(true, color, mode, include_border);
    }

    inline ScopedInvalid::ScopedInvalid(bool is_invalid,
                                        ImVec4 color,
                                        ImGuiCol target)
        : m_active(is_invalid), m_count(0) {
        if (m_active) {
            ImGui::PushStyleColor(target, color);
            m_count = 1;
        }
    }

    inline ScopedInvalid::~ScopedInvalid() {
        end();
    }

    inline void ScopedInvalid::end() {
        if (m_active) {
            if (m_count > 0) ImGui::PopStyleColor(m_count);
            m_active = false;
            m_count  = 0;
        }
    }

} // namespace ImGuiX::Extensions

