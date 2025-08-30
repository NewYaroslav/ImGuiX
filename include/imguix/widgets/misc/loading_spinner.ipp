#include <algorithm>
#include <cmath>

namespace ImGuiX::Widgets {
    
    ImU32 DefaultSpinnerColor() {
        const ImGuiStyle& s = ImGui::GetStyle();
        // Heuristic: pick a stronger accent on light UIs, softer on dark
        const ImVec4 bg = s.Colors[ImGuiCol_WindowBg];
        const float lum = 0.2126f*bg.x + 0.7152f*bg.y + 0.0722f*bg.z; // relative luminance
        const ImGuiCol accent = (lum > 0.5f) ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab;
        return ImGui::GetColorU32(s.Colors[accent]);
    }

    bool LoadingSpinner(const char* id, const LoadingSpinnerConfig& cfg) {
        // Validate / clamp
        const float radius    = std::max(0.0f, cfg.radius);
        const float thickness = std::max(1.0f, cfg.thickness);
        const int   segments  = std::max(8, cfg.segments);
        const float sweep_rad = std::clamp(cfg.sweep_ratio, 0.05f, 1.0f) * 6.28318530717958647692f; // TAU
        const float phase     = static_cast<float>(ImGui::GetTime()) * std::max(0.0f, cfg.angular_speed);

        // Reserve item rect using public API
        // Make item square (2R x 2R) + optional extra top padding (keeps spacing similar to old code)
        const ImVec2 item_size(2.0f * radius, 2.0f * radius + cfg.extra_top_padding);
        if (!ImGui::InvisibleButton(id, item_size)) {
            // Even if not clicked/hovered, the item exists and we can draw (unless clipped).
            // If it was clipped, drawing won't be visible anyway. Continue to draw for consistency.
        }

        // Compute geometry from item rect
        const ImVec2 pos_min = ImGui::GetItemRectMin();
        const ImVec2 center(pos_min.x + radius, pos_min.y + cfg.extra_top_padding * 0.5f + radius);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImU32 col = cfg.color ? cfg.color : DefaultSpinnerColor();

        const float a_min = phase;
        const float a_max = phase + sweep_rad;

        dl->PathClear();
        dl->PathArcTo(center, radius, a_min, a_max, segments);
        dl->PathStroke(col, 0 /*flags*/, thickness);
        return true;
    }

} // namespace ImGuiX::Widgets
