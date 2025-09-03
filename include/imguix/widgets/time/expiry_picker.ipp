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

    inline bool ExpiryPicker(const char* id, int& seconds, const ExpiryPickerConfig& cfg) {
        const auto& P = cfg.presets ? *cfg.presets : DefaultExpiryPresets();
        seconds = std::clamp(seconds, cfg.min_seconds, cfg.max_seconds);

        std::string preview = Utils::format_hms_u32(seconds);
        bool changed = false;

        ImGui::PushID(id);

        if (cfg.combo_width > 0.0f)
            ImGui::SetNextItemWidth(cfg.combo_width);
        // уберём лимиты — попап без скролла
        ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX));

        if (BeginIconCombo(cfg.label ? cfg.label : u8"Expiration",
                           preview.c_str(),
                           cfg.icon_text))
        {
            const ImGuiStyle& st = ImGui::GetStyle();

            // ---------- Верхний блок (H : M : S) ----------
            int h = seconds / 3600;
            int m = (seconds / 60) % 60;
            int s = seconds % 60;

            const float frame_h = ImGui::GetFrameHeight();
            const float fw      = (cfg.field_width > 0.0f) ? cfg.field_width : (frame_h * 2.0f);
            const float colon_w = ImGui::CalcTextSize(":").x;
            const float gap     = (cfg.hms_gap >= 0.0f) ? cfg.hms_gap : st.ItemInnerSpacing.x;
            const float sep_w   = colon_w + 2.0f * gap;     // visual spacer width
            const float top_w   = fw + sep_w + fw + sep_w + fw;

            // хелпер центрирования блока шириной w
            auto CenterNextBlock = [](float w) {
                float avail = ImGui::GetContentRegionAvail().x;
                float off   = (avail - w) * 0.5f;
                if (off > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
            };

            //const float top_w = fw + colon_w + fw + colon_w + fw;

            ImGui::PushButtonRepeat(true);
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, st.CellPadding.y));

            CenterNextBlock(top_w);
            if (ImGui::BeginTable("exp_hms", 5,
                    ImGuiTableFlags_SizingFixedFit |
                    ImGuiTableFlags_NoHostExtendX |
                    ImGuiTableFlags_NoBordersInBody |
                    ImGuiTableFlags_NoPadOuterX))
            {
                ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, fw);
                ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, sep_w);
                ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, fw);
                ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, sep_w);
                ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, fw);
                
                auto centered_glyph = [](const char* glyph, float w, float h) {
                    ImVec2 pmin = ImGui::GetCursorScreenPos();
                    ImVec2 pmax = ImVec2(pmin.x + w, pmin.y + h);
                    ImGui::RenderTextClipped(pmin, pmax, glyph, nullptr, nullptr, ImVec2(0.5f, 0.5f));
                    ImGui::Dummy(ImVec2(w, h));
                };

                auto centered_text = [](const char* t, float w) {
                    ImVec2 ts = ImGui::CalcTextSize(t);
                    float dx = (w - ts.x) * 0.5f;
                    if (dx > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + dx);
                    ImGui::TextUnformatted(t);
                };

                auto input_2d = [&](const char* iid, int& v, int maxv) {
                    int before = v;
                    ImGui::SetNextItemWidth(fw);
                    ImGui::InputScalar(iid, ImGuiDataType_S32, &v, nullptr, nullptr, "%02d",
                        ImGuiInputTextFlags_CharsDecimal |
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_EnterReturnsTrue);
                    if (v < 0) v = 0; else if (v > maxv) v = maxv;
                    if (v != before) changed = true;
                };

                // Row 0: + + +
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); if (ImGui::Button("+##h", ImVec2(fw, 0))) { h = (h+1)%24; changed = true; }
                ImGui::TableSetColumnIndex(1); centered_text(" ", colon_w);
                ImGui::TableSetColumnIndex(2); if (ImGui::Button("+##m", ImVec2(fw, 0))) { m = (m+1)%60; changed = true; }
                ImGui::TableSetColumnIndex(3); centered_text(" ", colon_w);
                ImGui::TableSetColumnIndex(4); if (ImGui::Button("+##s", ImVec2(fw, 0))) { s = (s+1)%60; changed = true; }

                // Row 1: HH : MM : SS
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); input_2d("##H", h, 23);
                ImGui::TableSetColumnIndex(1); centered_glyph(":", sep_w, frame_h);
                ImGui::TableSetColumnIndex(2); input_2d("##M", m, 59);
                ImGui::TableSetColumnIndex(3); centered_glyph(":", sep_w, frame_h);
                ImGui::TableSetColumnIndex(4); input_2d("##S", s, 59);

                // Row 2: - - -
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); if (ImGui::Button("-##h", ImVec2(fw, 0))) { h = (h+23)%24; changed = true; }
                ImGui::TableSetColumnIndex(1); centered_text(" ", colon_w);
                ImGui::TableSetColumnIndex(2); if (ImGui::Button("-##m", ImVec2(fw, 0))) { m = (m+59)%60; changed = true; }
                ImGui::TableSetColumnIndex(3); centered_text(" ", colon_w);
                ImGui::TableSetColumnIndex(4); if (ImGui::Button("-##s", ImVec2(fw, 0))) { s = (s+59)%60; changed = true; }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();
            ImGui::PopButtonRepeat();

            if (changed) {
                int v = h * 3600 + m * 60 + s;
                std::clamp(v, cfg.min_seconds, cfg.max_seconds);
                if (v != seconds) seconds = v;
            }

            // ---------- Нижняя сетка пресетов (кнопки как обычные) ----------
            {
                const int rows = std::max(1, cfg.rows);
                const int cols = std::max(1, cfg.cols);
                const int ncols = cols * 2 - 1; // кнопка, spacer, кнопка...

                const float cell_h = (cfg.cell_size.y > 0.0f) ? cfg.cell_size.y : frame_h * 1.2f;
                const float grid_w = cols * fw + (cols - 1) * colon_w;

                ImGui::Dummy(ImVec2(0.0f, st.ItemSpacing.y * 0.5f));
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, st.CellPadding.y));

                CenterNextBlock(grid_w);
                if (ImGui::BeginTable("exp_presets_btns", ncols,
                        ImGuiTableFlags_SizingFixedFit |
                        ImGuiTableFlags_NoHostExtendX |
                        ImGuiTableFlags_NoBordersInBody |
                        ImGuiTableFlags_NoPadOuterX))
                {
                    // ширины колонок: кнопки = fw, прокладки = colon_w
                    for (int c = 0; c < ncols; ++c) {
                        float w = (c % 2 == 0) ? fw : colon_w;
                        ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, w);
                    }

                    // если пользователь попросил особое скругление — применим только к «кнопочным» колонкам
                    const bool use_custom_round = (cfg.cell_rounding > 0.0f);

                    int pi = 0;
                    for (int r = 0; r < rows; ++r) {
                        ImGui::TableNextRow();
                        for (int c = 0; c < ncols; ++c) {
                            ImGui::TableSetColumnIndex(c);

                            if (c % 2 == 1) { // spacer
                                ImGui::Dummy(ImVec2(colon_w, 0));
                                continue;
                            }

                            ImGui::PushID(r * ncols + c);
                            if (pi < (int)P.size()) {
                                const auto& pr = P[pi];
                                const bool is_selected = (seconds == pr.seconds);

                                if (use_custom_round)
                                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, cfg.cell_rounding);

                                // Optional sticky-like highlight using Button colors
                                if (cfg.preset_highlight_selected && is_selected) {
                                    const ImGuiStyle& s = ImGui::GetStyle();
                                    ImGui::PushStyleColor(ImGuiCol_Button,        s.Colors[ImGuiCol_ButtonActive]);
                                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, s.Colors[ImGuiCol_ButtonActive]);
                                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  s.Colors[ImGuiCol_ButtonActive]);
                                }

                                // Кнопка пресета: выглядит как обычная Button
                                bool pressed = ImGui::Button(pr.label, ImVec2(fw, cell_h));
                                
                                if (cfg.preset_highlight_selected && is_selected)
                                    ImGui::PopStyleColor(3);

                                if (use_custom_round)
                                    ImGui::PopStyleVar();

                                if (pressed) {
                                    int v = pr.seconds;
                                    std::clamp(v, cfg.min_seconds, cfg.max_seconds);
                                    if (v != seconds) { seconds = v; changed = true; }
                                }

                                if (cfg.show_cell_borders) {
                                    ImDrawList* dl = ImGui::GetWindowDrawList();
                                    ImVec2 a = ImGui::GetItemRectMin(), b = ImGui::GetItemRectMax();
                                    a.x = std::floor(a.x) + 0.5f; a.y = std::floor(a.y) + 0.5f;
                                    b.x = std::floor(b.x) - 0.5f; b.y = std::floor(b.y) - 0.5f;
                                    ImU32 col = ImGui::GetColorU32(ImGuiCol_Border);
                                    dl->AddRect(a, b, col, 0.0f, 0, 1.0f);
                                }

                                ++pi;
                            } else {
                                ImGui::Dummy(ImVec2(fw, cell_h));
                            }
                            ImGui::PopID();
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::PopStyleVar();
            }

            EndIconCombo();
        }

        ImGui::PopID();
        return changed;
    }

#   ifdef IMGUIX_DEMO
    inline void DemoExpiryPicker() {
        static int expiry_sec = 5;
        ExpiryPickerConfig cfg;
        cfg.label = u8"Expiration";
        cfg.icon_text = u8"\ue8b5";
        cfg.combo_width = 180.0f;
        ExpiryPicker("expiry", expiry_sec, cfg);
    }
#   endif

} // namespace ImGuiX::Widgets
