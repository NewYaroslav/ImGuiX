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

        // --- helpers (внутренние лямбды) ---------------------------------------
        const int step = std::max(1, cfg.step_seconds);

        auto norm_forbidden = [&](const std::vector<SecRange>* src) {
            std::vector<SecRange> r;
            if (!src) return r;
            r.reserve(src->size());
            for (auto v : *src) {
                if (v.from > v.to) std::swap(v.from, v.to);
                v.from = std::clamp(v.from, cfg.min_seconds, cfg.max_seconds);
                v.to   = std::clamp(v.to,   cfg.min_seconds, cfg.max_seconds);
                if (v.from <= v.to) r.push_back(v);
            }
            std::sort(r.begin(), r.end(), [](const SecRange& a, const SecRange& b){ return a.from < b.from; });
            std::vector<SecRange> m;
            for (auto s : r) {
                if (m.empty() || s.from > m.back().to + 1) m.push_back(s);
                else m.back().to = std::max(m.back().to, s.to);
            }
            return m;
        };

        const std::vector<SecRange> F = norm_forbidden(cfg.forbidden);

        auto in_forbidden = [&](int x) -> int {
            for (int i = 0; i < (int)F.size(); ++i)
                if (x >= F[i].from && x <= F[i].to) return i;
            return -1;
        };

        auto ceil_div = [](int a, int b) {
            return (a >= 0) ? ( (a + b - 1) / b ) : (a / b);
        };

        auto quantize = [&](int v, int dir) {
            if (step <= 1) return v;
            if (dir > 0)       return ceil_div(v, step) * step;  // вверх
            else if (dir < 0)  return (v / step) * step;         // вниз
            else {
                int d = (v / step) * step;
                int u = ceil_div(v, step) * step;
                return (std::abs(v - d) <= std::abs(u - v)) ? d : u;
            }
        };

        // dir: +1 (увеличение), -1 (уменьшение), 0 (нейтрально/ввод/пресет)
        auto adjust_allowed = [&](int v, int dir) {
            v = std::clamp(v, cfg.min_seconds, cfg.max_seconds);
            v = quantize(v, dir);

            if (dir == 0) {
                // ближайшее допустимое (равноудалённость -> вниз)
                int down = quantize(v, -1);
                int up   = quantize(v, +1);

                auto walk_down = [&](){
                    while (down >= cfg.min_seconds) {
                        int idx = in_forbidden(down);
                        if (idx < 0) break;
                        down = quantize(F[idx].from - 1, -1);
                    }
                    down = std::clamp(down, cfg.min_seconds, cfg.max_seconds);
                };
                auto walk_up = [&](){
                    while (up <= cfg.max_seconds) {
                        int idx = in_forbidden(up);
                        if (idx < 0) break;
                        up = quantize(F[idx].to + 1, +1);
                    }
                    up = std::clamp(up, cfg.min_seconds, cfg.max_seconds);
                };
                walk_down(); walk_up();
                return (std::abs(v - down) <= std::abs(up - v)) ? down : up;
            }

            if (dir > 0) { // вверх: «после диапазона»
                while (v <= cfg.max_seconds) {
                    int idx = in_forbidden(v);
                    if (idx < 0) break;
                    v = quantize(F[idx].to + 1, +1);
                }
                return std::clamp(v, cfg.min_seconds, cfg.max_seconds);
            } else {        // вниз: «до диапазона»
                while (v >= cfg.min_seconds) {
                    int idx = in_forbidden(v);
                    if (idx < 0) break;
                    v = quantize(F[idx].from - 1, -1);
                }
                return std::clamp(v, cfg.min_seconds, cfg.max_seconds);
            }
        };

        auto can_move = [&](int cur, int delta, int dir) {
            int target = cur + (dir > 0 ? std::max(0, delta) : -std::max(0, delta));
            int adj = adjust_allowed(target, dir);
            return (dir > 0) ? (adj > cur) : (adj < cur);
        };
        // -----------------------------------------------------------------------

        std::string preview = Utils::format_hms_u32(seconds);
        bool changed = false;

        ImGui::PushID(id);

        if (cfg.combo_width > 0.0f)
            ImGui::SetNextItemWidth(cfg.combo_width);
        // попап без скролла
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
            const float sep_w   = colon_w + 2.0f * gap;     // визуальная ширина колонки с ':'
            const float top_w   = fw + sep_w + fw + sep_w + fw;

            auto CenterNextBlock = [](float w) {
                float avail = ImGui::GetContentRegionAvail().x;
                float off   = (avail - w) * 0.5f;
                if (off > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
            };

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

                // действия поверх seconds (с учётом шага и запретов)
                auto apply_inc = [&](int delta){ int v = adjust_allowed(seconds + delta, +1); if (v != seconds) { seconds = v; changed = true; } };
                auto apply_dec = [&](int delta){ int v = adjust_allowed(seconds - delta, -1); if (v != seconds) { seconds = v; changed = true; } };
                auto apply_direct = [&](int h_, int m_, int s_){
                    int v = h_*3600 + m_*60 + s_;
                    int dir = (v > seconds) ? +1 : (v < seconds ? -1 : 0);
                    v = adjust_allowed(v, dir);
                    if (v != seconds) { seconds = v; changed = true; }
                };

                const bool allow_sec = (step < 60);

                // can-move для дизейбла +/- (учитываем всё)
                const bool can_h_plus  = can_move(seconds, 3600, +1);
                const bool can_m_plus  = can_move(seconds,   60, +1);
                const bool can_s_plus  = allow_sec && can_move(seconds, 1, +1);
                const bool can_h_minus = can_move(seconds, 3600, -1);
                const bool can_m_minus = can_move(seconds,   60, -1);
                const bool can_s_minus = allow_sec && can_move(seconds, 1, -1);

                // Row 0: + + +
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::BeginDisabled(!can_h_plus);
                if (ImGui::Button("+##h", ImVec2(fw, 0))) apply_inc(3600);
                ImGui::EndDisabled();

                ImGui::TableSetColumnIndex(1); centered_text(" ", colon_w);

                ImGui::TableSetColumnIndex(2);
                ImGui::BeginDisabled(!can_m_plus);
                if (ImGui::Button("+##m", ImVec2(fw, 0))) apply_inc(60);
                ImGui::EndDisabled();

                ImGui::TableSetColumnIndex(3); centered_text(" ", colon_w);

                ImGui::TableSetColumnIndex(4);
                ImGui::BeginDisabled(!can_s_plus);
                if (ImGui::Button("+##s", ImVec2(fw, 0))) apply_inc(1);
                ImGui::EndDisabled();

                // синхронизируем локальные h/m/s
                h = seconds / 3600; m = (seconds / 60) % 60; s = seconds % 60;

                // Row 1: HH : MM : SS
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                {
                    int h_edit = h;
                    ImGui::SetNextItemWidth(fw);
                    if (ImGui::InputScalar("##H", ImGuiDataType_S32, &h_edit, nullptr, nullptr, "%02d",
                                           ImGuiInputTextFlags_CharsDecimal |
                                           ImGuiInputTextFlags_AutoSelectAll |
                                           ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        h_edit = std::clamp(h_edit, 0, 23);
                        apply_direct(h_edit, m, s);
                        h = seconds / 3600; m = (seconds / 60) % 60; s = seconds % 60;
                    }
                }

                ImGui::TableSetColumnIndex(1); centered_glyph(":", sep_w, frame_h);

                ImGui::TableSetColumnIndex(2);
                {
                    int m_edit = m;
                    ImGui::SetNextItemWidth(fw);
                    if (ImGui::InputScalar("##M", ImGuiDataType_S32, &m_edit, nullptr, nullptr, "%02d",
                                           ImGuiInputTextFlags_CharsDecimal |
                                           ImGuiInputTextFlags_AutoSelectAll |
                                           ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        m_edit = std::clamp(m_edit, 0, 59);
                        apply_direct(h, m_edit, s);
                        h = seconds / 3600; m = (seconds / 60) % 60; s = seconds % 60;
                    }
                }

                ImGui::TableSetColumnIndex(3); centered_glyph(":", sep_w, frame_h);

                ImGui::TableSetColumnIndex(4);
                ImGui::BeginDisabled(!allow_sec);
                {
                    int s_edit = s;
                    ImGui::SetNextItemWidth(fw);
                    if (ImGui::InputScalar("##S", ImGuiDataType_S32, &s_edit, nullptr, nullptr, "%02d",
                                           ImGuiInputTextFlags_CharsDecimal |
                                           ImGuiInputTextFlags_AutoSelectAll |
                                           ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        s_edit = std::clamp(s_edit, 0, 59);
                        apply_direct(h, m, s_edit);
                        h = seconds / 3600; m = (seconds / 60) % 60; s = seconds % 60;
                    }
                }
                ImGui::EndDisabled();

                // Row 2: - - -
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::BeginDisabled(!can_h_minus);
                if (ImGui::Button("-##h", ImVec2(fw, 0))) apply_dec(3600);
                ImGui::EndDisabled();

                ImGui::TableSetColumnIndex(1); centered_text(" ", colon_w);

                ImGui::TableSetColumnIndex(2);
                ImGui::BeginDisabled(!can_m_minus);
                if (ImGui::Button("-##m", ImVec2(fw, 0))) apply_dec(60);
                ImGui::EndDisabled();

                ImGui::TableSetColumnIndex(3); centered_text(" ", colon_w);

                ImGui::TableSetColumnIndex(4);
                ImGui::BeginDisabled(!can_s_minus);
                if (ImGui::Button("-##s", ImVec2(fw, 0))) apply_dec(1);
                ImGui::EndDisabled();

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();
            ImGui::PopButtonRepeat();

            // seconds уже меняли напрямую — просто подстрахуемся
            if (changed) seconds = std::clamp(seconds, cfg.min_seconds, cfg.max_seconds);

            // ---------- Нижняя сетка пресетов (кнопки как обычные) ----------
            {
                const int rows = std::max(1, cfg.rows);
                const int cols = std::max(1, cfg.cols);
                const int ncols = cols * 2 - 1; // кнопка, spacer, кнопка...

                const float cell_h = (cfg.cell_size.y > 0.0f) ? cfg.cell_size.y : frame_h * 1.2f;
                const float grid_w = cols * fw + (cols - 1) * (colon_w); // визуально так же, как сверху

                ImGui::Dummy(ImVec2(0.0f, st.ItemSpacing.y * 0.5f));
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, st.CellPadding.y));

                CenterNextBlock(grid_w);
                if (ImGui::BeginTable("exp_presets_btns", ncols,
                        ImGuiTableFlags_SizingFixedFit |
                        ImGuiTableFlags_NoHostExtendX |
                        ImGuiTableFlags_NoBordersInBody |
                        ImGuiTableFlags_NoPadOuterX))
                {
                    for (int c = 0; c < ncols; ++c) {
                        float w = (c % 2 == 0) ? fw : colon_w;
                        ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, w);
                    }

                    const bool use_custom_round = (cfg.cell_rounding > 0.0f);

                    int pi = 0;
                    for (int r = 0; r < rows; ++r) {
                        ImGui::TableNextRow();
                        for (int c = 0; c < ncols; ++c) {
                            ImGui::TableSetColumnIndex(c);

                            if (c % 2 == 1) { ImGui::Dummy(ImVec2(colon_w, 0)); continue; }

                            ImGui::PushID(r * ncols + c);
                            if (pi < (int)P.size()) {
                                const auto& pr = P[pi];
                                const bool is_selected = (seconds == pr.seconds);

                                if (use_custom_round)
                                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, cfg.cell_rounding);

                                if (cfg.preset_highlight_selected && is_selected) {
                                    const ImGuiStyle& s = ImGui::GetStyle();
                                    ImGui::PushStyleColor(ImGuiCol_Button,        s.Colors[ImGuiCol_ButtonActive]);
                                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, s.Colors[ImGuiCol_ButtonActive]);
                                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  s.Colors[ImGuiCol_ButtonActive]);
                                }

                                if (ImGui::Button(pr.label, ImVec2(fw, cell_h))) {
                                    int v = adjust_allowed(pr.seconds, 0); // к ближайшему допустимому
                                    if (v != seconds) { seconds = v; changed = true; }
                                }

                                if (cfg.preset_highlight_selected && is_selected) ImGui::PopStyleColor(3);
                                if (use_custom_round) ImGui::PopStyleVar();

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
        static const std::vector<SecRange> bans = {
            {120,120}, {300,420} // запреты: ровно 2:00 и 5:00..7:00
        };

        ExpiryPickerConfig cfg;
        cfg.label = u8"Expiration";
        cfg.icon_text = u8"\ue8b5";
        cfg.combo_width = 180.0f;
        cfg.min_seconds  = 60;
        cfg.step_seconds = 60;     // квантуем по 1 минуте -> секунды отключены
        cfg.forbidden = &bans;
        ExpiryPicker("expiry", expiry_sec, cfg);
    }
#   endif

} // namespace ImGuiX::Widgets

