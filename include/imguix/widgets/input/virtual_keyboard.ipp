#include <iostream>
#include <imgui.h>
#include <string>
#include <vector>
#include <cctype>
#include <cstring>
#include <algorithm>
#include <regex>

#include <imguix/widgets/controls/icon_button.hpp>
#include <imguix/config/icons.hpp>

namespace ImGuiX::Widgets {

bool VirtualKeyboard(
        const char* id,
        std::string& text,
        VirtualKeyboardConfig cfg
    ) {
            const char* id, 
            std::string& text, 
            const VirtualKeyboardConfig& cfg,
            VirtualKeyboardState* state = nullptr) {
        bool modified = false;

        ImGui::PushID(id);
        ImVec2 spacing = (cfg.spacing > 0.0f) ? ImVec2(cfg.spacing, cfg.spacing)
                                      : ImGui::GetStyle().ItemSpacing; // keep style
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);

        const ImGuiID key_vk_locale = ImGui::GetID(u8"vk_locale");
        const ImGuiID key_vk_shift_state = ImGui::GetID(u8"vk_shift_state");
        const ImGuiID key_vk_pane_mode   = ImGui::GetID(u8"vk_pane_mode");
        ImGuiStorage* st = ImGui::GetStateStorage();

        if (state) state->submitted = false;
        bool did_submit_this_frame = false;
        
        // helper to trigger submit once
        auto do_submit = [&](){
            if (did_submit_this_frame) return;         // prevent duplicates in the same frame
            did_submit_this_frame = true;
            if (state) state->submitted = true;
            if (cfg.on_submit) cfg.on_submit(text);    // user callback
        };
        
        auto get_locale = [&](){
            if (state) return std::clamp(state->locale_index, 0, (int)cfg.locales.size() - 1);
            int locale_index = st->GetInt(key_vk_locale, std::clamp(cfg.start_locale_index, 0, (int)cfg.locales.size() - 1));
            return std::clamp(locale_index, 0, (int)cfg.locales.size() - 1);
        };
        
        auto set_locale = [&](int i){
            if (state) state->locale_index = i;
            else st->SetInt(key_vk_locale, i);
        };

        auto get_shift = [&](){
            if (state) return state->shift;
            return st->GetInt(key_vk_shift_state, 0) != 0;
        };
        
        auto set_shift = [&](bool v){
            if (state) state->shift = v;
            else st->SetInt(key_vk_shift_state, v ? 1 : 0);
        };

        auto get_pane = [&](){
            if (state) return state->pane;
            int v = cfg.remember_pane ? st->GetInt(key_vk_pane_mode, 0) : 0;
            return v == 1 ? VKSpecialPane::Accents : VKSpecialPane::Symbols;
        };
        
        auto set_pane = [&](VKSpecialPane p){
            if (state) state->pane = p;
            else if (cfg.remember_pane) st->SetInt(key_vk_pane_mode, p==VKSpecialPane::Accents ? 1:0);
        };
        
        auto do_copy = [&](){
            if (!cfg.allow_copy_from_text) return;
            ImGui::SetClipboardText(text.c_str());
        };

        auto do_paste = [&](){
            if (!cfg.allow_paste_into_text) return;
            const char* src = ImGui::GetClipboardText();
            if (!src) return;
            std::string s(src);
            if (cfg.sanitize_paste_newlines) {
                // CRLF/CR -> \n
                std::string out; out.reserve(s.size());
                for (size_t i=0;i<s.size();++i){
                    char c = s[i];
                    if (c=='\r') { if (i+1<s.size() && s[i+1]=='\n') ++i; out.push_back('\n'); }
                    else out.push_back(c);
                }
                s.swap(out);
            }
            if (cfg.max_text_len_on_paste>0) {
                size_t allowed = (text.size()>= (size_t)cfg.max_text_len_on_paste) ? 0 :
                                 (size_t)cfg.max_text_len_on_paste - text.size();
                if (s.size() > allowed) s.resize(allowed);
            }
            if (s.empty()) return;
            if (cfg.paste_replace_all) text = std::move(s);
            else                       text += s;
            modified = true;
        };

        // --- раскладка / shift состояние ---
        int locale_index = get_locale();
        const char* locale_name = cfg.locales.empty() ? u8"en" : cfg.locales[locale_index];
        const KBLayout& L = get_layout(locale_name);
        bool shift = get_shift();

        const ImGuiStyle& style = ImGui::GetStyle();
        const float pad_x = style.WindowPadding.x;
        const float pad_y = style.WindowPadding.y;
        const float child_b = style.ChildBorderSize;
        const float row_gap  = style.ItemSpacing.y;

        // --- авто ширина/высота (с учётом паддингов Child) ---
        // внутренние размеры контента:
        const int left_cols_max = std::max(10, std::max(L.row2_n, std::max(L.row3_n, L.row4_n)));
        const int right_cols    = 6;
        const int left_rows     = (cfg.show_digits ? 4 : 3);
        const int right_rows    = 5;
        const float preview_h = cfg.show_top_preview
            ? ((cfg.preview_height > 0.0f) ? cfg.preview_height : ImGui::GetFrameHeight() + 2.0f * style.FramePadding.y)
            : 0.0f;

        float inner_w = (cfg.size.x > 0.0f)
            ? (cfg.size.x - style.WindowPadding.x * 2.0f)
            : calc_min_inner_width(cfg, left_cols_max, right_cols);

        float inner_h = (cfg.size.y > 0.0f)
            ? (cfg.size.y - style.WindowPadding.y * 2.0f)
            : calc_min_inner_height(cfg, left_rows, right_rows);
            
        if (cfg.size.y <= 0.0f && cfg.show_top_preview)
            inner_h += row_gap + preview_h;

        // внешние размеры Child
        float child_w = inner_w + 2.0f * pad_x + (cfg.border ? 2.0f * child_b : 0.0f);
        float child_h = inner_h + 2.0f * pad_y + (cfg.border ? 2.0f * child_b : 0.0f);
        child_h += cfg.height_bias_px;
        //child_h += 2.0f * pad_y;

        ImGui::BeginChild(u8"##VirtualKeyboard", ImVec2(child_w, child_h), cfg.border, ImGuiWindowFlags_NoScrollbar);
        const float base_x = ImGui::GetCursorPosX();

        // 
        bool is_valid = true; // default ok
        if (cfg.validate_with_regex && cfg.validate_regex) {
            try {
                std::regex re(cfg.validate_regex);
                is_valid = std::regex_match(text, re);
            } catch (...) {
                is_valid = true; // invalid regex => do not block
            }
        }

        if (cfg.show_top_preview) {
            // background (Child) + colored text inside
            ImGui::PushStyleColor(ImGuiCol_ChildBg, cfg.preview_bg_col);
            ImGui::BeginChild(u8"##vk_preview",
                ImVec2(inner_w, preview_h),
                cfg.preview_border,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::PushStyleColor(ImGuiCol_Text, is_valid ? cfg.preview_text_col : cfg.preview_text_invalid);
            // Single-line display; clip if long
            ImGui::TextUnformatted(text.c_str());
            ImGui::PopStyleColor();

            ImGui::EndChild();
            ImGui::PopStyleColor();

            // spacing between preview and panels
            ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y));
        }

        auto press = [&](const char* label, bool ascii_single = false) {
            if (std::strcmp(label, u8"Bksp") == 0) {
                if (!text.empty()) { 
                    text.pop_back(); 
                    modified = true; 
                }
                return;
            }
            if (std::strcmp(label, u8"Shift") == 0) {
                shift = !shift; 
                set_shift(shift);
                return;
            }
            if (std::strcmp(label, u8"Space") == 0) { 
                text.push_back(' '); 
                modified = true; 
                return; 
            }
            if (std::strcmp(label, u8"Enter") == 0) { 
                if (cfg.submit_on_enter) {
                    // если нужно — поддержать Shift+Enter = перенос строки
                    if (cfg.shift_enter_newline && 
                        shift && 
                        cfg.allow_enter) {
                        text.push_back('\n'); 
                        modified = true;
                    } else {
                        do_submit();
                    }
                } else {
                    if (cfg.allow_enter) { 
                        text.push_back('\n'); 
                        modified = true; 
                    }
                }
                return; 
            }
            if (ascii_single) append_char_ascii(text, label, shift, modified);
            else append_utf8(text, label, modified);
        };
        
        auto show_tooltip = [&](const std::string& key) {
            if (ImGui::IsItemHovered()) {
                auto it = cfg.tooltips.find(key);
                if (it != cfg.tooltips.end()) {
                    ImGui::SetTooltip("%s", it->second);
                }
            }
        };

        // --- верхняя область: левая/правая панели ---
        ImVec2 key_size = cfg.key_size;
        if (key_size.x <= 0.0f || key_size.y <= 0.0f) {
            const float h = ImGui::GetFrameHeight(); // respects font & global scale
            key_size = ImVec2(h, h);                 // square keys by default
        }
        
        auto calc_btn_w = [&](const char* s) -> float {
            // ширина текста + горизонтальные паддинги кнопки
            ImVec2 ts = ImGui::CalcTextSize(s, nullptr, false);
            return ts.x + 2.0f * ImGui::GetStyle().FramePadding.x;
        };

        const float frame_h  = ImGui::GetFrameHeight();
        float wide_w = key_size.x * cfg.min_wide_key_factor; // нижняя граница
        
        if (cfg.auto_bottom_width) {
            wide_w = std::max(wide_w, calc_btn_w(u8"Shift"));
            wide_w = std::max(wide_w, calc_btn_w(u8"Bksp"));
            if (cfg.allow_enter)
                wide_w = std::max(wide_w, calc_btn_w(u8"Enter"));
        }
        ImVec2 wide_size (wide_w,                        key_size.y);
        ImVec2 space_size(wide_w * cfg.space_width_multiplier, key_size.y);

        const char* ICON_COPY  = IMGUIX_ICON_COPY;   // content_copy
        const char* ICON_PASTE = IMGUIX_ICON_PASTE;  // content_paste
        ImVec2 clip_size(frame_h, frame_h);

        const float btn_h    = cfg.show_bottom ? key_size.y : 0.0f;
        const float combo_h  = (cfg.show_locale_combo && cfg.locale_bottom_right) ? frame_h : 0.0f;
        const float row_h    = (btn_h > combo_h ? btn_h : combo_h);
        const float bottom_h_used = (cfg.show_bottom || (cfg.show_locale_combo && cfg.locale_bottom_right))
                                    ? (row_gap + row_h) : 0.0f;

        const float top_h = inner_h - bottom_h_used - (cfg.show_top_preview ? (row_gap + preview_h) : 0.0f);

        // ширины панелей по КОНТЕНТУ:
        float right_content_w = 0.0f;
        if (cfg.show_special)
            right_content_w = right_cols * key_size.x + (right_cols - 1) * cfg.spacing;

        // учтём паддинги каждого саб-чилда:
        float right_w_outer = cfg.show_special ? (right_content_w + 2.0f * pad_x + 2.0f * child_b) : 0.0f;
        float left_w_outer  = inner_w - right_w_outer - (cfg.show_special ? cfg.pane_gap : 0.0f);
        const float right_anchor_offset = left_w_outer + (cfg.show_special ? cfg.pane_gap : 0.0f);

        // Панель слева
        ImGui::BeginChild(u8"##vk_left", ImVec2(left_w_outer, top_h), true, ImGuiWindowFlags_NoScrollbar);
        {
            if (cfg.show_digits) {
                const char* row1[] = {u8"1",u8"2",u8"3",u8"4",u8"5",u8"6",u8"7",u8"8",u8"9",u8"0"};
                for (int i = 0; i < 10; ++i) {
                    if (ImGui::Button(row1[i], key_size)) press(row1[i], /*ascii=*/true);
                    if (i < 9) ImGui::SameLine();
                }
            }

            auto draw_row = [&](const char* const* row, int n, bool ascii) {
                for (int i = 0; i < n; ++i) {
                    const char* lbl = row[i];
                    if (ascii && shift && std::isalpha((unsigned char)lbl[0])) {
                        char up[2] = { (char)std::toupper((unsigned char)lbl[0]), 0 };
                        if (ImGui::Button(up, key_size)) press(up, /*ascii=*/true);
                    } else {
                        if (ImGui::Button(lbl, key_size)) {
                            if (ascii) press(lbl, /*ascii=*/true);
                            else       press(lbl, /*ascii=*/false);
                        }
                    }
                    if (i < (n - 1)) ImGui::SameLine();
                }
            };

            bool ascii_letters = (std::strcmp(L.name, u8"en") == 0);

            draw_row(shift && !ascii_letters ? L.row2U : L.row2, L.row2_n, ascii_letters);
            draw_row(shift && !ascii_letters ? L.row3U : L.row3, L.row3_n, ascii_letters);
            draw_row(shift && !ascii_letters ? L.row4U : L.row4, L.row4_n, ascii_letters);
        }
        ImGui::EndChild();

        // Панель справа
        if (cfg.show_special) {
            ImGui::SameLine(0.0f, cfg.pane_gap);
            ImGui::BeginChild(u8"##vk_right", ImVec2(right_w_outer, top_h), true, ImGuiWindowFlags_NoScrollbar);
            {
                // --- small mode switch (top)
                VKSpecialPane pane = get_pane();
                bool has_accents = cfg.show_accents && (get_accents_for_locale(locale_name).count > 0);

                if (pane == VKSpecialPane::Accents && has_accents) {
                    // --- ACCENTS GRID ---
                    AccentsSet acc = get_accents_for_locale(locale_name);
                    constexpr int cols = 6; // compact grid
                    int idx = 0;
                    for (int i = 0; i < acc.count; ++i) {
                        const char* s = acc.items[i];
                        if (ImGui::Button(s, key_size)) press(s, /*ascii=*/false);
                        ++idx;
                        if ((idx % cols) != 0 && i < acc.count - 1)
                            ImGui::SameLine();
                    }
                } else {
                    // --- SYMBOLS GRID (as before) ---
                    const char* spec[] = {
                        u8"!",u8"@",u8"#",u8"$",u8"%",u8"^",
                        u8"&",u8"*",u8"(",u8")",u8"-",u8"_",
                        u8"+",u8"=",u8"{",u8"}",u8"[",u8"]",
                        u8"|",u8"\\",u8":",u8";",u8"'",u8"\"",
                        u8"<",u8">",u8",",u8".",u8"?",u8"/"
                    };
                    constexpr int cols = 6, rows = 5;
                    int idx = 0;
                    for (int r = 0; r < rows; ++r) {
                        for (int c = 0; c < cols; ++c, ++idx) {
                            if (ImGui::Button(spec[idx], key_size)) press(spec[idx], /*ascii=*/true);
                            if (c < cols - 1) ImGui::SameLine();
                        }
                    }
                }
            }
            ImGui::EndChild();
        }

        // реакция на ФИЗИЧЕСКИЙ Enter
        if (cfg.submit_on_enter && cfg.capture_hw_enter) {
            const bool wnd_focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
            if (wnd_focused) {
                const bool key_enter  = ImGui::IsKeyPressed(ImGuiKey_Enter, false);
                const bool key_kp_ent = ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false);
                if (key_enter || key_kp_ent) {
                    if (cfg.shift_enter_newline && (ImGui::GetIO().KeyShift || get_shift())) {
                        if (cfg.allow_enter) { 
                            text.push_back('\n'); 
                            modified = true; 
                        }
                    } else {
                        do_submit();
                    }
                }
            }
        }

        // Универсальный рендер кнопки (иконка или текст)
        auto ButtonOrIcon = [&](const char* label, const ImVec2& size, bool ascii_single = false) -> bool {
            // если иконка настроена — рисуем иконку
            auto it = cfg.labels.find(label);
            if (it != cfg.labels.end() && it->second && it->second[0] != '\0') {
                unsigned int cp = static_cast<unsigned char>(it->second[0]);
                if ((cp & 0xF0) == 0xE0) {
                    ImGuiX::Widgets::IconButtonConfig ib = cfg.icon_btn;
                    if (cfg.icon_font) ib.font = cfg.icon_font;
                    return ImGuiX::Widgets::IconButtonCentered(label, it->second, ib);
                }
                return ImGui::Button(label, size);
            }
            // обычная кнопка
            return ImGui::Button(label, size);
        };

        // --- нижняя строка + селектор локали (справа) ---
        if (cfg.show_bottom || 
            (cfg.show_locale_combo && cfg.locale_bottom_right) || 
            (cfg.show_accents && cfg.show_special)) {
            ImGui::Dummy(ImVec2(0.0f, style.ItemSpacing.y));
            
            const float line_start_x = ImGui::GetCursorPosX();

            if (cfg.show_bottom) {
                if (ButtonOrIcon(u8"Shift", wide_size)) press(u8"Shift");
                show_tooltip(u8"Shift");
                ImGui::SameLine();
                if (ButtonOrIcon(u8"Space", space_size)) press(u8"Space");
                show_tooltip(u8"Space");
                ImGui::SameLine();
                if (ButtonOrIcon(u8"Bksp", wide_size)) press(u8"Bksp");
                show_tooltip(u8"Bksp");
                if (cfg.allow_enter) { 
                    ImGui::SameLine(); 
                    if (ButtonOrIcon(u8"Enter", wide_size)) { 
                        press(u8"Enter"); 
                        show_tooltip(u8"Enter");
                    }
                }
                // Copy/Paste
                if (cfg.show_clipboard_buttons) {
                    if (cfg.allow_copy_from_text) {
                        ImGui::SameLine(); 
                        if (ButtonOrIcon(u8"Copy",  clip_size)) do_copy();
                        show_tooltip(u8"Copy");
                    }
                    if (cfg.allow_paste_into_text) {
                        ImGui::SameLine(); 
                        if (ButtonOrIcon(u8"Paste", clip_size)) do_paste();
                        show_tooltip(u8"Paste");
                    }
                }
            }

            // по началу правой части — переключатель режимов
            if (cfg.show_accents && cfg.show_special) {
                bool has_accents = get_accents_for_locale(locale_name).count > 0;
                
                if (has_accents) {
                    float offset = right_anchor_offset;                    // смещение от начала строки
                    if (offset < 0.0f) offset = 0.0f;                      // на всякий случай
                    //ImGui::SameLine(offset, style.ItemSpacing.x);          // позиционируем "по X от начала"
                    ImGui::SameLine(offset, 0.0f);
                    const char* label = (get_pane() == VKSpecialPane::Symbols) ? u8"Accents" : u8"Symbols";
                    const float btn_w = 64.0f;
                    if (ButtonOrIcon(label, ImVec2(btn_w, 0))) {
                        set_pane(get_pane() == VKSpecialPane::Symbols ? 
                            VKSpecialPane::Accents : 
                            VKSpecialPane::Symbols);
                    }
                    show_tooltip(label);
                }
            }

            // комбо локали справа
            if (cfg.show_locale_combo && 
                cfg.locale_bottom_right && 
                !cfg.locales.empty()) {
                // прижать вправо
                const float line_x0 = line_start_x;
                float w = std::max(64.0f, cfg.locale_width);
                float x = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - w;

                // если есть правая панель — выравниваем по её правому краю
                float target_x;
                if (cfg.show_special) {
                    // base_x — X-старт контента главного Child в начале рендера
                    target_x = base_x + right_anchor_offset + right_w_outer - w;
                    // переведём в координату строки: SameLine сбивает относительный X,
                    // поэтому ставим курсор жёстко от line_x0.
                    ImGui::SameLine(0.0f, 0.0f);
                    ImGui::SetCursorPosX(target_x);
                } else {
                    // fallback: правый край всего child-контента
                    target_x = line_x0 + ImGui::GetContentRegionAvail().x - w;
                    ImGui::SameLine(0.0f, 0.0f);
                    ImGui::SetCursorPosX(target_x);
                }

                ImGui::SetNextItemWidth(w);
                const char* cur = cfg.locales[locale_index];
                if (ImGui::BeginCombo(u8"##vk_locale_br", cur, ImGuiComboFlags_HeightSmall)) {
                    for (int i = 0; i < (int)cfg.locales.size(); ++i) {
                        bool sel = (i == locale_index);
                        if (ImGui::Selectable(cfg.locales[i], sel)) {
                            set_locale(i);
                            locale_index = i;
                        }
                        if (sel) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
        } else 
        if (cfg.show_locale_combo && 
            !cfg.locale_bottom_right && 
            !cfg.locales.empty()) {
            // старое размещение сверху (компактно)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (cfg.spacing + key_size.y)); // поднять к верху
            ImGui::SetNextItemWidth(std::max(64.0f, cfg.locale_width));
            const char* cur = cfg.locales[locale_index];
            if (ImGui::BeginCombo(u8"##vk_locale_top", cur, ImGuiComboFlags_HeightSmall)) {
                for (int i = 0; i < (int)cfg.locales.size(); ++i) {
                    bool sel = (i == locale_index);
                    if (ImGui::Selectable(cfg.locales[i], sel)) {
                        set_locale(i);
                        locale_index = i;
                    }
                    if (sel) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopID();

        return modified;
    }


} // namespace ImGuiX::Widgets

