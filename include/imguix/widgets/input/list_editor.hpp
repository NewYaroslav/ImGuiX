#pragma once
#ifndef _IMGUIX_WIDGETS_LIST_EDITOR_HPP_INCLUDED
#define _IMGUIX_WIDGETS_LIST_EDITOR_HPP_INCLUDED

/// \file list_editor.hpp
/// \brief Combo-like list editor (strings/ints) with popup child. No CSV.
/// \note ImGui 1.89+. Uses public API only.

#include <imgui.h>
#include <string>
#include <vector>
#include <set>
#include <cstdio>
#include <cstring>
#include <algorithm>

namespace ImGuiX::Widgets {

    /// \brief Config for ListEditor.
    struct ListEditorConfig {
        float      combo_width  = 0.0f;                   ///< Width of combo preview button.
        ImVec2     popup_size   = ImVec2(0.0f, 0.0f);     ///< Size of popup child.
        const char* empty_desc  = u8"(empty)";            ///< Preview when list is empty.

        // Default to Material Icons PUA. Make sure the font is loaded with PUA ranges.
        const char* icon_add    = u8"\uE148";             ///< 'add'
        const char* icon_remove = u8"\uE15D";             ///< 'remove_circle'

        bool       delete_on_right = true;                ///< Remove button placed at right side.
        bool       deduplicate     = false;               ///< Keep unique values on add.
    };

    // ------------------------------- strings -------------------------------

    /// \brief Combo-like list editor for strings. One input -> one element. No CSV.
    inline bool ListEditor(
            const char* id,
            const char* label,
            std::vector<std::string>& items,
            const ListEditorConfig& cfg = {}
        ) {
        bool changed = false;

        // Precise UTF-8 safe preview by pixel width
        auto build_preview_px = [&](const std::vector<std::string>& elems, float px_max) -> std::string {
            if (elems.empty() || px_max <= 0.0f)
                return cfg.empty_desc ? std::string(cfg.empty_desc) : std::string();

            const float ell_w = ImGui::CalcTextSize("...").x;
            const float limit = std::max(0.0f, px_max - ell_w); // оставляем место под "..."

            std::string out;
            float w = 0.0f;
            bool truncated = false;

            // minimal UTF-8 decoder: returns bytes consumed (0 on error)
            auto utf8_next = [](const char* p, const char* e, unsigned& cp) -> int {
                if (p >= e) return 0;
                const unsigned char b0 = (unsigned char)p[0];
                if (b0 < 0x80) { cp = b0; return 1; }
                if ((b0 >> 5) == 0x6) { // 110xxxxx 10xxxxxx
                    if (p + 1 >= e) return 0;
                    const unsigned char b1 = (unsigned char)p[1];
                    if ((b1 & 0xC0) != 0x80) return 0;
                    cp = ((b0 & 0x1F) << 6) | (b1 & 0x3F);
                    return 2;
                }
                if ((b0 >> 4) == 0xE) { // 1110xxxx 10xxxxxx 10xxxxxx
                    if (p + 2 >= e) return 0;
                    const unsigned char b1 = (unsigned char)p[1], b2 = (unsigned char)p[2];
                    if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80) return 0;
                    cp = ((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
                    return 3;
                }
                if ((b0 >> 3) == 0x1E) { // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                    if (p + 3 >= e) return 0;
                    const unsigned char b1 = (unsigned char)p[1], b2 = (unsigned char)p[2], b3 = (unsigned char)p[3];
                    if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) return 0;
                    cp = ((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
                    return 4;
                }
                return 0; // invalid lead
            };

            auto try_append = [&](const char* s, int len) -> bool {
                std::string tmp = out; tmp.append(s, s + len);
                float new_w = ImGui::CalcTextSize(tmp.c_str()).x;
                if (new_w <= limit) { out.swap(tmp); w = new_w; return true; }
                return false;
            };

            for (size_t i = 0; i < elems.size(); ++i) {
                if (!out.empty()) {
                    if (!try_append(",", 1)) { truncated = true; break; }
                }
                const char* p = elems[i].c_str();
                const char* e = p + std::strlen(p);
                while (p < e) {
                    unsigned cp = 0;
                    const char* p0 = p;
                    int n = utf8_next(p, e, cp);
                    if (n <= 0) break; // invalid sequence -> прекращаем добавление
                    p += n;
                    if (!try_append(p0, n)) { truncated = true; break; }
                }
                if (truncated) break;
            }

            if (truncated) out += "...";
            if (out.empty()) return cfg.empty_desc ? std::string(cfg.empty_desc) : std::string();
            return out;
        };

        auto dedup = [&](std::vector<std::string>& v) {
            std::set<std::string> s(v.begin(), v.end());
            v.assign(s.begin(), s.end());
        };

        const float item_w = ImGui::CalcItemWidth();
        float combo_width = cfg.combo_width > 0.0f ?
            cfg.combo_width : item_w;

        const ImGuiStyle& st = ImGui::GetStyle();
        const float arrow_w = ImGui::GetFrameHeight();
        const float text_max_px = std::max(0.0f, combo_width - (2.0f * st.FramePadding.x + st.ItemInnerSpacing.x + arrow_w));
            
        ImGui::PushID(id);
        ImGui::SetNextItemWidth(combo_width);
        const std::string preview = build_preview_px(items, text_max_px);
        if (!ImGui::BeginCombo(label, preview.c_str(), ImGuiComboFlags_HeightLargest)) {
            ImGui::PopID();
            return false;
        }
        
        ImVec2 popup_size = cfg.popup_size;
        if (popup_size.y <= 0.0f) {
            popup_size.y = ImGui::GetFrameHeightWithSpacing() * 8.0f;
        }

        // Popup content
        ImGui::BeginChild("##list_editor_child", popup_size, true);
        const float btn_w   = ImGui::GetFrameHeight(); // square
        const float inner_w = 
            (popup_size.x > 0.0f ? popup_size.x : ImGui::CalcItemWidth()) - 
            (btn_w + ImGui::GetStyle().ItemSpacing.x);

        // Existing items
        for (size_t i = 0; i < items.size(); /* ++i in body */) {
            ImGui::PushID(static_cast<int>(i));

            if (!cfg.delete_on_right) {
                if (ImGui::Button(cfg.icon_remove)) {
                    items.erase(items.begin() + static_cast<long>(i));
                    changed = true;
                    ImGui::PopID();
                    continue;
                }
                ImGui::SameLine();
            }

            if (popup_size.x > 0.0f) ImGui::SetNextItemWidth(inner_w);
            char buf[512];
            std::snprintf(buf, sizeof(buf), "%s", items[i].c_str());
            if (ImGui::InputText("##item", buf, sizeof(buf))) {
                items[i] = buf;
                changed = true;
            }

            if (cfg.delete_on_right) {
                ImGui::SameLine();
                if (ImGui::Button(cfg.icon_remove)) {
                    items.erase(items.begin() + static_cast<long>(i));
                    changed = true;
                    ImGui::PopID();
                    continue;
                }
            }

            ImGui::PopID();
            ++i;
        }

        // Add one new item
        static char add_buf[512] = {0}; // local-static: shared per TU; fine for demo
        if (popup_size.x > 0.0f) ImGui::SetNextItemWidth(inner_w);
        ImGui::InputText("##add_item", add_buf, sizeof(add_buf));
        ImGui::SameLine();
        if (ImGui::Button(cfg.icon_add)) {
            if (add_buf[0]) {
                items.emplace_back(add_buf);
                if (cfg.deduplicate) dedup(items);
                add_buf[0] = '\0';
                changed = true;
            }
        }

        ImGui::EndChild();
        ImGui::EndCombo();

        ImGui::PopID();
        return changed;
    }

    // ------------------------------- integers -------------------------------

    /// \brief Combo-like list editor for integers. One input -> one element. No CSV.
    inline bool ListEditor(
            const char* id,
            const char* label,
            std::vector<int>& items,
            const ListEditorConfig& cfg = {}
        ) {
        ImGui::PushID(id);
        bool changed = false;
        // Preview from ints
        auto build_preview = [&](float preview_px) -> std::string {
            const float avg_w = ImGui::CalcTextSize("Hamburgefontsiv").x / 15.0f;
            const size_t max_chars = (preview_px > 16.0f && avg_w > 0.0f)
                                     ? static_cast<size_t>((preview_px - 16.0f) / avg_w)
                                     : size_t(0);
            if (items.empty() || max_chars == 0)
                return cfg.empty_desc ? std::string(cfg.empty_desc) : std::string();

            std::string out;
            for (size_t i = 0; i < items.size(); ++i) {
                if (!out.empty()) out.push_back(',');
                char tmp[64];
                std::snprintf(tmp, sizeof(tmp), "%d", items[i]);
                out += tmp;
                if (out.size() >= max_chars) break;
            }
            if (out.size() > max_chars && max_chars >= 3) {
                out.resize(max_chars - 3);
                out += "...";
            }
            return out;
        };

        auto dedup = [&](std::vector<int>& v) {
            std::set<int> s(v.begin(), v.end());
            v.assign(s.begin(), s.end());
        };
        
        const float item_w = ImGui::CalcItemWidth();
        float combo_width = cfg.combo_width > 0.0f ?
            cfg.combo_width : item_w;

        const ImGuiStyle& st = ImGui::GetStyle();
        const float arrow_w = ImGui::GetFrameHeight();
        const float text_max_px = std::max(0.0f, combo_width - (2.0f * st.FramePadding.x + st.ItemInnerSpacing.x + arrow_w));

        ImGui::SetNextItemWidth(combo_width);
        const std::string preview = build_preview(text_max_px);
        if (!ImGui::BeginCombo(label, preview.c_str(), ImGuiComboFlags_HeightLargest)) {
            ImGui::PopID();
            return false;
        }

        ImVec2 popup_size = cfg.popup_size;
        if (popup_size.y <= 0.0f) {
            popup_size.y = ImGui::GetFrameHeightWithSpacing() * 8.0f;
        }

        ImGui::BeginChild("##list_items", popup_size, true);
        const float btn_w = ImGui::GetFrameHeight();
        const float inner_w = 
            (popup_size.x > 0.0f ? popup_size.x : ImGui::CalcItemWidth()) - 
            (btn_w + ImGui::GetStyle().ItemSpacing.x);

        for (size_t i = 0; i < items.size(); /* ++i in body */) {
            ImGui::PushID(static_cast<int>(i));

            if (!cfg.delete_on_right) {
                if (ImGui::Button(cfg.icon_remove)) {
                    items.erase(items.begin() + static_cast<long>(i));
                    changed = true;
                    ImGui::PopID();
                    continue;
                }
                ImGui::SameLine();
            }

            if (popup_size.x > 0.0f) ImGui::SetNextItemWidth(inner_w);
            if (ImGui::InputInt("##item", &items[i], 0, 0)) {
                changed = true;
            }

            if (cfg.delete_on_right) {
                ImGui::SameLine();
                if (ImGui::Button(cfg.icon_remove)) {
                    items.erase(items.begin() + static_cast<long>(i));
                    changed = true;
                    ImGui::PopID();
                    continue;
                }
            }

            ImGui::PopID();
            ++i;
        }

        // Add one new integer
        static int add_value = 0;
        if (popup_size.x > 0.0f) ImGui::SetNextItemWidth(inner_w);
        ImGui::InputInt("##add_item", &add_value, 0, 0);
        ImGui::SameLine();
        if (ImGui::Button(cfg.icon_add)) {
            items.push_back(add_value);
            if (cfg.deduplicate) dedup(items);
            add_value = 0;
            changed = true;
        }

        ImGui::EndChild();
        ImGui::EndCombo();

        ImGui::PopID();
        return changed;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_LIST_EDITOR_HPP_INCLUDED

