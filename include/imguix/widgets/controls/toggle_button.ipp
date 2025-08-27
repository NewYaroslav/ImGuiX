#include <algorithm>

namespace ImGuiX::Widgets {

    void ToggleButton(const char* id, bool* state) {
        // Geometry
        const float h = ImGui::GetFrameHeight();
        const float w = h * 1.55f;
        const float r = h * 0.50f;

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        // Click area
        ImGui::InvisibleButton(id, ImVec2(w, h));
        if (ImGui::IsItemClicked() && state) *state = !*state;

        // --- Animation (public API) -----------------------------------------
        // Store per-widget animation progress in window storage.
        ImGuiStorage* st = ImGui::GetStateStorage();
        ImGui::PushID(id);
        const ImGuiID key_anim = ImGui::GetID("anim");
        ImGui::PopID();

        float anim = st->GetFloat(key_anim, (*state ? 1.0f : 0.0f));
        const float target = (*state ? 1.0f : 0.0f);

        // Linear approach to target with ~0.08s time to settle (like old kAnimSpeed)
        const float dt = ImGui::GetIO().DeltaTime;
        const float step = (dt > 0 ? dt / 0.08f : 1.0f); // 0.08s to travel 0->1
        if (target > anim) anim = std::min(1.0f, anim + step);
        else               anim = std::max(0.0f, anim - step);

        st->SetFloat(key_anim, anim);
        const float t = anim; // 0..1

        // Small helpers (no ImLerp/ImSaturate from internals)
        auto lerp4 = [](const ImVec4& a, const ImVec4& b, float k) {
            if (k < 0.0f) k = 0.0f; else if (k > 1.0f) k = 1.0f;
            return ImVec4(a.x + (b.x - a.x) * k,
                          a.y + (b.y - a.y) * k,
                          a.z + (b.z - a.z) * k,
                          a.w + (b.w - a.w) * k);
        };

        // Colors
        const bool hovered = ImGui::IsItemHovered();
        const ImVec4 off_hov = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
        const ImVec4 on_hov  = ImVec4(0.64f, 0.83f, 0.34f, 1.0f);
        const ImVec4 off     = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
        const ImVec4 on      = ImVec4(0.56f, 0.83f, 0.26f, 1.0f);

        const ImU32 col_bg = ImGui::GetColorU32( hovered ? lerp4(off_hov, on_hov, t)
                                                          : lerp4(off,     on,     t) );

        // Track background
        dl->AddRectFilled(
            pos,
            ImVec2(pos.x + w, pos.y + h),
            col_bg,
            h * 0.5f
        );

        // Knob
        const float x0 = pos.x + r;
        const float x1 = pos.x + (w - r);
        const float x  = x0 + (x1 - x0) * t;

        dl->AddCircleFilled(
            ImVec2(x, pos.y + r),
            r - 1.5f,
            IM_COL32(255,255,255,255)
        );
    }

} // namespace ImGuiX::Widgets

