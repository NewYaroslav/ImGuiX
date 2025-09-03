#include <imgui_internal.h> // BeginComboPopup, RenderNavCursor, RenderFrameBorder

namespace ImGuiX::Widgets {

    inline bool BeginIconCombo(
        const char* label,
        const char* preview_value,
        const char* icon_text,
        ImGuiComboFlags flags,
        ImFont* icon_font
    ) {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.HasFlags;
        g.NextWindowData.ClearFlags(); // как в оригинале
        if (window->SkipItems)
            return false;

        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview));
        if (flags & ImGuiComboFlags_WidthFitPreview)
            IM_ASSERT((flags & (ImGuiComboFlags_NoPreview | (ImGuiComboFlags)ImGuiComboFlags_CustomPreview)) == 0);

        const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : ImGui::GetFrameHeight();
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
        const float preview_width = ((flags & ImGuiComboFlags_WidthFitPreview) && (preview_value != NULL)) ? ImGui::CalcTextSize(preview_value, NULL, true).x : 0.0f;
        const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : ((flags & ImGuiComboFlags_WidthFitPreview) ? (arrow_size + preview_width + style.FramePadding.x * 2.0f) : ImGui::CalcItemWidth());
        const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
        const ImRect total_bb(bb.Min, bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
        ImGui::ItemSize(total_bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, id, &bb))
            return false;

        // Открытие по клику
        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
        const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
        bool popup_open = ImGui::IsPopupOpen(popup_id, ImGuiPopupFlags_None);
        if (pressed && !popup_open)
        {
            ImGui::OpenPopupEx(popup_id, ImGuiPopupFlags_None);
            popup_open = true;
        }

        // Рамка + фон превью
        const ImU32 frame_col = ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        const float value_x2 = ImMax(bb.Min.x, bb.Max.x - arrow_size);
        ImGui::RenderNavCursor(bb, id);
        if (!(flags & ImGuiComboFlags_NoPreview))
            window->DrawList->AddRectFilled(bb.Min, ImVec2(value_x2, bb.Max.y), frame_col, style.FrameRounding,
                                            (flags & ImGuiComboFlags_NoArrowButton) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersLeft);

        // === ЗАМЕНА СТРЕЛКИ НА ИКОНКУ ===
        if (!(flags & ImGuiComboFlags_NoArrowButton)) {
            ImU32 bg_col   = ImGui::GetColorU32((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
            const bool round_all = (w <= arrow_size);

            // Правый квадрат (как у оригинальной стрелки)
            const ImVec2 a0(value_x2, bb.Min.y);
            const ImVec2 a1(bb.Max.x, bb.Max.y);
            window->DrawList->AddRectFilled(a0, a1, bg_col, style.FrameRounding,
                                            round_all ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersRight);

            // Центрируем и рисуем иконку
            const char* ic = icon_text ? icon_text : "";
            if (ic[0] && value_x2 + arrow_size - style.FramePadding.x <= bb.Max.x)
            {
                if (icon_font) ImGui::PushFont(icon_font);
                const ImVec2 isz = ImGui::CalcTextSize(ic);
                const float  sq  = arrow_size;
                const ImVec2 ip  = ImVec2(a0.x + (sq - isz.x) * 0.5f,
                                          a0.y + (bb.GetHeight() - isz.y) * 0.5f);
                window->DrawList->AddText(ip, text_col, ic);
                if (icon_font) ImGui::PopFont();
            }
        }
        ImGui::RenderFrameBorder(bb.Min, bb.Max, style.FrameRounding);
        // === /ЗАМЕНА СТРЕЛКИ ===

        // Кастом-превью (совместимость с ImGuiComboFlags_CustomPreview)
        if (flags & ImGuiComboFlags_CustomPreview) {
            g.ComboPreviewData.PreviewRect = ImRect(bb.Min.x, bb.Min.y, value_x2, bb.Max.y);
            IM_ASSERT(preview_value == NULL || preview_value[0] == 0);
            preview_value = NULL;
        }

        // Превью и лейбл — как в оригинале
        if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview)) {
            if (g.LogEnabled)
                ImGui::LogSetNextTextDecoration("{", "}");
            ImGui::RenderTextClipped(bb.Min + style.FramePadding, ImVec2(value_x2, bb.Max.y), preview_value, NULL, NULL);
        }
        if (label_size.x > 0)
            ImGui::RenderText(ImVec2(bb.Max.x + style.ItemInnerSpacing.x, bb.Min.y + style.FramePadding.y), label);

        if (!popup_open)
            return false;

        g.NextWindowData.HasFlags = backup_next_window_data_flags;
        // КРИТИЧНО: вызываем ОРИГИНАЛЬНЫЙ BeginComboPopup
        return ImGui::BeginComboPopup(popup_id, bb, flags);
    }

    inline void EndIconCombo() {
        ImGui::EndCombo();
    }

} // namespace ImGuiX::Widgets
