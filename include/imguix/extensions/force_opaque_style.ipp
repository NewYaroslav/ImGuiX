#include <imgui.h>

namespace ImGuiX::Extensions {

    inline void ForceOpaqueStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Alpha = 1.0f;
        for (int i = 0; i < ImGuiCol_COUNT; ++i) {
            style.Colors[i].w = 1.0f;
        }
    }

    inline ScopedOpaqueStyle::ScopedOpaqueStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        m_old_alpha = style.Alpha;
        for (int i = 0; i < ImGuiCol_COUNT; ++i) {
            m_old_colors[i] = style.Colors[i];
            style.Colors[i].w = 1.0f;
        }
        style.Alpha = 1.0f;
    }

    inline ScopedOpaqueStyle::~ScopedOpaqueStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Alpha = m_old_alpha;
        for (int i = 0; i < ImGuiCol_COUNT; ++i) {
            style.Colors[i] = m_old_colors[i];
        }
    }

} // namespace ImGuiX::Extensions

