#include <imgui.h>
#include <regex>
#include <string>

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

    inline void BeginInvalid(bool is_invalid,
                             ImVec4 color,
                             ImGuiCol target) {
        if (is_invalid)
            ImGui::PushStyleColor(target, color);
    }

    inline void EndInvalid(bool is_invalid, int count) {
        if (is_invalid)
            ImGui::PopStyleColor(count);
    }

    inline ScopedInvalid::ScopedInvalid(bool is_invalid,
                                        ImVec4 color,
                                        ImGuiCol target) :
        m_active(is_invalid) {
        if (m_active)
            ImGui::PushStyleColor(target, color);
    }

    inline ScopedInvalid::~ScopedInvalid() {
        end();
    }

    inline void ScopedInvalid::end() {
        if (m_active) {
            ImGui::PopStyleColor();
            m_active = false;
        }
    }

} // namespace ImGuiX::Extensions

