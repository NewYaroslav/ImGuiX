#include <imgui.h>

namespace ImGuiX::Extensions {

    inline void BeginDisable(bool disable) {
        if (!disable)
            return;
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    inline void EndDisable(bool disable) {
        if (!disable)
            return;
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }

    inline ScopedDisable::ScopedDisable(bool disable) :
        m_is_disabled(disable) {
        BeginDisable(m_is_disabled);
    }

    inline ScopedDisable::~ScopedDisable() {
        EndDisable(m_is_disabled);
    }

} // namespace ImGuiX::Extensions

