#include <imgui.h>
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include <imguix/widgets/controls/icon_combo.hpp>
#include <imguix/utils/time_utils.hpp>

namespace ImGuiX::Widgets {

    inline const std::vector<ExpiryPreset>& DefaultExpiryPresets() {
        static const std::vector<ExpiryPreset> p = {
            {"S5",   5},   {"S15",   15}, {"S30",   30},
            {"M1",   60},  {"M3",   180}, {"M5",   300},
            {"M30",  1800},{"H1",   3600},{"H4",  14400},
        };
        return p;
    }

    inline bool ExpiryPicker(
            const char* id,
            int& seconds,
            const ExpiryPickerConfig& cfg
    ) {
        const auto& presets = cfg.presets ? *cfg.presets : DefaultExpiryPresets();

        seconds = std::clamp(seconds, cfg.min_seconds, cfg.max_seconds);
        std::string preview = Utils::format_hms_u32(seconds);

        if (cfg.combo_width > 0.0f) {
            ImGui::SetNextItemWidth(cfg.combo_width);
        }

        bool changed = false;
        ImGui::PushID(id);

        bool open = BeginIconCombo(
                cfg.label ? cfg.label : u8"Expiry",
                preview.c_str(),
                cfg.icon_text,
                cfg.icon_slot_width);

        if (open) {
            int h = seconds / 3600;
            int m = (seconds / 60) % 60;
            int s = seconds % 60;

            const float frame_h = ImGui::GetFrameHeight();
            const float fw = cfg.field_width > 0.0f ? cfg.field_width : frame_h * 2.0f;

            auto plus = [&](int& v, int max, int step) { v = (v + step) % (max + 1); };
            auto minus = [&](int& v, int max, int step) {
                v -= step;
                if (v < 0) {
                    v += (max + 1);
                }
            };

            ImGui::BeginGroup();
            ImGui::PushID("plus");
            if (ImGui::Button("+", ImVec2(fw, 0))) { plus(h, 23, 1); changed = true; }
            ImGui::SameLine();
            if (ImGui::Button("+", ImVec2(fw, 0))) { plus(m, 59, 1); changed = true; }
            ImGui::SameLine();
            if (ImGui::Button("+", ImVec2(fw, 0))) { plus(s, 59, 1); changed = true; }
            ImGui::PopID();

            auto centered_text_box = [&](const char* txt) {
                ImVec2 tsz = ImGui::CalcTextSize(txt);
                float extra = std::max(0.0f, fw - tsz.x);
                ImGui::Dummy(ImVec2(extra * 0.5f, 0));
                ImGui::SameLine(0, 0);
                ImGui::TextUnformatted(txt);
                ImGui::SameLine(0, 0);
                ImGui::Dummy(ImVec2(extra * 0.5f, 0));
            };
            {
                char buf[8];
                std::snprintf(buf, sizeof(buf), "%02d", h); centered_text_box(buf);
                ImGui::SameLine(); ImGui::TextUnformatted(":"); ImGui::SameLine();
                std::snprintf(buf, sizeof(buf), "%02d", m); centered_text_box(buf);
                ImGui::SameLine(); ImGui::TextUnformatted(":"); ImGui::SameLine();
                std::snprintf(buf, sizeof(buf), "%02d", s); centered_text_box(buf);
            }

            ImGui::PushID("minus");
            if (ImGui::Button("-", ImVec2(fw, 0))) { minus(h, 23, 1); changed = true; }
            ImGui::SameLine();
            if (ImGui::Button("-", ImVec2(fw, 0))) { minus(m, 59, 1); changed = true; }
            ImGui::SameLine();
            if (ImGui::Button("-", ImVec2(fw, 0))) { minus(s, 59, 1); changed = true; }
            ImGui::PopID();

            ImGui::EndGroup();

            if (changed) {
                int v = h * 3600 + m * 60 + s;
                v = std::clamp(v, cfg.min_seconds, cfg.max_seconds);
                if (v != seconds) {
                    seconds = v;
                }
            }

            const ImGuiStyle& st = ImGui::GetStyle();
            ImVec2 cell = cfg.cell_size;
            if (cell.x <= 0.0f || cell.y <= 0.0f) {
                float fh = ImGui::GetFrameHeight();
                cell = ImVec2(fh * 1.5f, fh * 1.2f);
            }

            int rows = std::max(1, cfg.rows);
            int cols = std::max(1, cfg.cols);

            ImGui::Dummy(ImVec2(0.0f, st.ItemSpacing.y * 0.5f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,
                                cfg.cell_rounding >= 0.0f ? cfg.cell_rounding : 0.0f);

            int pi = 0;
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    if (c) {
                        ImGui::SameLine();
                    }
                    ImGui::PushID(r * cols + c);
                    if (pi < static_cast<int>(presets.size())) {
                        bool sel = seconds == presets[pi].seconds;
                        if (ImGui::Selectable(
                                presets[pi].label,
                                sel,
                                ImGuiSelectableFlags_DontClosePopups,
                                cell)) {
                            int clamped = std::clamp(
                                    presets[pi].seconds,
                                    cfg.min_seconds,
                                    cfg.max_seconds);
                            if (seconds != clamped) {
                                seconds = clamped;
                                changed = true;
                            }
                        }
                        if (cfg.show_cell_borders) {
                            ImDrawList* dl = ImGui::GetWindowDrawList();
                            ImVec2 a = ImGui::GetItemRectMin();
                            ImVec2 b = ImGui::GetItemRectMax();
                            a.x = std::floor(a.x) + 0.5f;
                            a.y = std::floor(a.y) + 0.5f;
                            b.x = std::floor(b.x) - 0.5f;
                            b.y = std::floor(b.y) - 0.5f;
                            ImU32 col = sel
                                ? ImGui::GetColorU32(ImGuiCol_Header)
                                : (ImGui::IsItemHovered()
                                        ? ImGui::GetColorU32(ImGuiCol_HeaderHovered)
                                        : ImGui::GetColorU32(ImGuiCol_Border));
                            dl->AddRect(a, b, col, 0.0f, 0, 1.0f);
                        }
                        ++pi;
                    } else {
                        ImGui::Dummy(cell);
                    }
                    ImGui::PopID();
                }
            }
            ImGui::PopStyleVar();

            EndIconCombo();
        }

        ImGui::PopID();
        return changed;
    }

#ifdef IMGUIX_DEMO
    inline void DemoExpiryPicker() {
        static int expiry_sec = 5;
        ExpiryPickerConfig cfg;
        cfg.label = u8"Expiration";
        cfg.icon_text = u8"🕒";
        cfg.combo_width = 180.0f;
        ExpiryPicker("expiry", expiry_sec, cfg);
    }
#endif

} // namespace ImGuiX::Widgets
