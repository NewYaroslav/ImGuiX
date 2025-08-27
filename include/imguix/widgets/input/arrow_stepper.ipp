#include <algorithm>
#include <cstdio>

#include <imgui.h>

namespace ImGuiX::Widgets {

bool ArrowStepper(
        const char* id,
        int& v,
        const ArrowStepperConfig& cfg,
        int* last_delta
) {
    bool changed = false;
    ImGui::PushID(id);
    ImGui::BeginGroup();

    // --- sanitize config
    int minv = cfg.min_value;
    int maxv = cfg.max_value;
    if (maxv < minv) std::swap(minv, maxv);
    int step = (cfg.step <= 0 ? 1 : cfg.step);
    // clamp current value once
    v = (v < minv ? minv : (v > maxv ? maxv : v));

    // --- text input (no built-in +/- buttons)
    ImGui::SetNextItemWidth(cfg.input_width);
    int prev = v;
    if (ImGui::InputInt(cfg.input_id, &v, 0, 0)) {
        if (v < minv) v = cfg.wrap ? maxv : minv;
        if (v > maxv) v = cfg.wrap ? minv : maxv;
        changed |= (v != prev);
    }

    // --- arrows with auto-repeat
    ImGui::SameLine(0.0f, cfg.same_line_spacing);
    ImGui::PushButtonRepeat(true);

    // Up
    bool at_up_edge = (!cfg.wrap && v >= maxv);
    if (cfg.disable_at_edges && at_up_edge) ImGui::BeginDisabled(true);
    if (ImGui::ArrowButton(u8"##up", ImGuiDir_Up)) {
        int nv = v + step;
        if (nv > maxv) {
            if (cfg.wrap) nv = minv;
            else { nv = v; /* keep current value */ }
        }
        if (last_delta) *last_delta = +step;   // always report intended delta
        changed |= (nv != v);
        v = nv;
    }
    if (cfg.disable_at_edges && at_up_edge) ImGui::EndDisabled();

    // Down
    ImGui::SameLine(0.0f, cfg.same_line_spacing);
    bool at_dn_edge = (!cfg.wrap && v <= minv);
    if (cfg.disable_at_edges && at_dn_edge) ImGui::BeginDisabled(true);
    if (ImGui::ArrowButton(u8"##dn", ImGuiDir_Down)) {
        int nv = v - step;
        if (nv < minv) {
            if (cfg.wrap) nv = maxv;
            else { nv = v; /* keep current value */ }
        }
        if (last_delta) *last_delta = -step;   // always report intended delta
        changed |= (nv != v);
        v = nv;
    }
    if (cfg.disable_at_edges && at_dn_edge) ImGui::EndDisabled();

    ImGui::PopButtonRepeat();

    // --- optional inline label
    if (cfg.fmt && *cfg.fmt) {
        ImGui::SameLine(0.0f, 4.0f);
        ImGui::Text(cfg.fmt, v);
    }

    ImGui::EndGroup();

    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    const ImGuiIO& io = ImGui::GetIO();

    // --- optional: mouse wheel over the whole group
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) &&
        ImGui::IsMouseHoveringRect(min, max))
    {
        if (io.MouseWheel != 0.0f) {
            int delta = (io.MouseWheel > 0.0f ? +step : -step);
            int nv = v + delta;

            if (nv > maxv) nv = cfg.wrap ? minv : v;  // at boundary: keep v but report delta
            if (nv < minv) nv = cfg.wrap ? maxv : v;

            changed |= (nv != v);
            if (last_delta) *last_delta = (io.MouseWheel > 0.0f ? +step : -step);
            v = nv;
        }
    }

    ImGui::PopID();
    return changed;
}

} // namespace ImGuiX::Widgets

