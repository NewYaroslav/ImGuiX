#include <imgui.h>

namespace ImGuiX::Windows {
    
    ImGuiFramedWindow::ImGuiFramedWindow(
            int id, 
            ApplicationControl& app, 
            std::string name,
            std::string title,
            WindowFlags flags, 
            ImGuiFramedWindowConfig config)
        : WindowInstance(id, app, std::move(name)), 
          m_title(std::move(title)), 
          m_flags(flags), 
          m_config(std::move(config)) {}
          
    bool ImGuiFramedWindow::create(int w, int h) {
        m_width = w;
        m_height = h;
        return create();
    }
    
    void ImGuiFramedWindow::drawTitleBarText() {
        float padding_y = (m_config.title_bar_height - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::SetCursorPosY(padding_y);
        ImGui::Text("%s", m_title.c_str());
    }

    void ImGuiFramedWindow::drawControlButtons(float title_padding_x) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        ImVec2 char_size = ImGui::CalcTextSize("W");
        float btn_width = char_size.x * 3.0f + ImGui::GetStyle().FramePadding.x * 2.0f;
        float btn_height = char_size.y;
        float btn_padding = ImGui::GetStyle().ItemSpacing.x * 2.0f + btn_width * 3.0f + title_padding_x;
        ImVec2 btn_size(btn_width, btn_height);

        ImVec2 btn_min, btn_max;
        float center_y = (m_config.title_bar_height - btn_size.y) * 0.5f;
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - btn_padding, center_y));
        if (ImGui::Button(m_config.minimize_button_text, btn_size)) {
            minimize();
        }

        btn_min = ImGui::GetItemRectMin();
        btn_max = ImGui::GetItemRectMax();
#       ifdef _WIN32
        m_minimize_btn_rect = {
            static_cast<LONG>(btn_min.x),
            static_cast<LONG>(btn_min.y),
            static_cast<LONG>(btn_max.x),
            static_cast<LONG>(btn_max.y)
        };
#       endif
        
        ImGui::SameLine();
        if (ImGui::Button(m_config.maximize_button_text, btn_size)) {
            toggleMaximizeRestore();
        }
        
        btn_min = ImGui::GetItemRectMin();
        btn_max = ImGui::GetItemRectMax();
#       ifdef _WIN32
        m_maximize_btn_rect = {
            static_cast<LONG>(btn_min.x),
            static_cast<LONG>(btn_min.y),
            static_cast<LONG>(btn_max.x),
            static_cast<LONG>(btn_max.y)
        };
#       endif

        ImGui::SameLine();
        if (ImGui::Button(m_config.close_button_text, btn_size)) {
            close();
        }

        btn_min = ImGui::GetItemRectMin();
        btn_max = ImGui::GetItemRectMax();
#       ifdef _WIN32
        m_close_btn_rect = {
            static_cast<LONG>(btn_min.x),
            static_cast<LONG>(btn_min.y),
            static_cast<LONG>(btn_max.x),
            static_cast<LONG>(btn_max.y)
        };
#       endif

        ImGui::PopStyleVar();
    }
    
    void ImGuiFramedWindow::drawMacStyledControlButtons(float title_padding_x) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        float btn_diameter = ImGui::GetTextLineHeight() * ImGui::GetStyle().FramePadding.y * 2.0f;
        float btn_padding = ImGui::GetStyle().ItemSpacing.x * 2.0f + btn_diameter * 3.0f + title_padding_x;
        ImVec2 btn_size(btn_diameter, btn_diameter);

        ImVec2 btn_min, btn_max;
        float center_y = (m_config.title_bar_height - btn_size.y) * 0.5f;
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - btn_padding, center_y));
        if (drawCircleButton("##imguix_btn_minimize", btn_diameter, ImVec4(1.0f, 0.74f, 0.18f, 1.0f))) {
            minimize();
        }

        btn_min = ImGui::GetItemRectMin();
        btn_max = ImGui::GetItemRectMax();
#       ifdef _WIN32
        m_minimize_btn_rect = {
            static_cast<LONG>(btn_min.x),
            static_cast<LONG>(btn_min.y),
            static_cast<LONG>(btn_max.x),
            static_cast<LONG>(btn_max.y)
        };
#       endif
        
        ImGui::SameLine();
        if (drawCircleButton("##imguix_btn_maximize", btn_diameter, ImVec4(0.15f, 0.79f, 0.25f, 1.0f))) {
            toggleMaximizeRestore();
        }

        btn_min = ImGui::GetItemRectMin();
        btn_max = ImGui::GetItemRectMax();
#       ifdef _WIN32
        m_maximize_btn_rect = {
            static_cast<LONG>(btn_min.x),
            static_cast<LONG>(btn_min.y),
            static_cast<LONG>(btn_max.x),
            static_cast<LONG>(btn_max.y)
        };
#       endif

        ImGui::SameLine();
        if (drawCircleButton("##imguix_btn_close", btn_diameter, ImVec4(1.0f, 0.0f, 0.0f, 1.0f))) {
            close();
        }
 
        btn_min = ImGui::GetItemRectMin();
        btn_max = ImGui::GetItemRectMax();
#       ifdef _WIN32
        m_close_btn_rect = {
            static_cast<LONG>(btn_min.x),
            static_cast<LONG>(btn_min.y),
            static_cast<LONG>(btn_max.x),
            static_cast<LONG>(btn_max.y)
        };
#       endif

        ImGui::PopStyleVar();
    }
    
    void ImGuiFramedWindow::drawImGuiStyledControlButtons(float title_padding_x) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        float btn_height = m_config.title_bar_height - ImGui::GetStyle().FramePadding.y * 2.0f;
        float btn_padding = ImGui::GetStyle().ItemSpacing.x * 2.0f + btn_height * 3.0f + title_padding_x;
        ImVec2 btn_size(btn_height, btn_height);

        ImVec2 btn_min, btn_max;
        float center_y = (m_config.title_bar_height - btn_size.y) * 0.5f;
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - btn_padding, center_y));
        if (drawSystemButton("##imguix_btn_minimize", SystemButtonType::Minimize, btn_size)) {
            minimize();
        }

        btn_min = ImGui::GetItemRectMin();
        btn_max = ImGui::GetItemRectMax();
#       ifdef _WIN32
        m_minimize_btn_rect = {
            static_cast<LONG>(btn_min.x),
            static_cast<LONG>(btn_min.y),
            static_cast<LONG>(btn_max.x),
            static_cast<LONG>(btn_max.y)
        };
#       endif
        
        ImGui::SameLine();
        if (drawSystemButton("##imguix_btn_maximize", SystemButtonType::Maximize, btn_size)) {
            toggleMaximizeRestore();
        }

        btn_min = ImGui::GetItemRectMin();
        btn_max = ImGui::GetItemRectMax();
#       ifdef _WIN32
        m_maximize_btn_rect = {
            static_cast<LONG>(btn_min.x),
            static_cast<LONG>(btn_min.y),
            static_cast<LONG>(btn_max.x),
            static_cast<LONG>(btn_max.y)
        };
#       endif

        ImGui::SameLine();
        if (drawSystemButton("##imguix_btn_close", SystemButtonType::Close, btn_size)) {
            close();
        }
 
        btn_min = ImGui::GetItemRectMin();
        btn_max = ImGui::GetItemRectMax();
#       ifdef _WIN32
        m_close_btn_rect = {
            static_cast<LONG>(btn_min.x),
            static_cast<LONG>(btn_min.y),
            static_cast<LONG>(btn_max.x),
            static_cast<LONG>(btn_max.y)
        };
#       endif

        ImGui::PopStyleVar();
    }

    bool ImGuiFramedWindow::drawCircleButton(const char* id, float diameter, const ImVec4& color) {
        ImVec2 size = ImVec2(diameter, diameter);
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton(id, size);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 center = ImVec2(pos.x + diameter * 0.5f, pos.y + diameter * 0.5f);

        ImVec4 final_col = color;
        if (ImGui::IsItemHovered()) final_col = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        if (ImGui::IsItemActive())  final_col = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);

        draw_list->AddCircleFilled(center, diameter * 0.5f, ImGui::ColorConvertFloat4ToU32(final_col), 16);

        return ImGui::IsItemClicked();
    }
    
    bool ImGuiFramedWindow::drawSystemButton(const char* id, SystemButtonType type, ImVec2 size) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton(id, size);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        const bool is_hovered = ImGui::IsItemHovered();
        const bool is_active  = ImGui::IsItemActive();

        // Используем прямоугольник только при наведении или нажатии
        if (is_hovered || is_active) {
            ImVec4 bg_col = ImGui::GetStyleColorVec4(is_active ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
            draw_list->AddRectFilled(pos,
                                     ImVec2(pos.x + size.x, pos.y + size.y),
                                     ImGui::ColorConvertFloat4ToU32(bg_col),
                                     2.0f);
        }

        // Цвет символа
        ImVec4 fg_col = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        ImU32 col_u32 = ImGui::ColorConvertFloat4ToU32(fg_col);

        // Центр кнопки
        ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);
        float cross_extent = size.x * 0.25f;
        float line_thickness = 1.0f;

        if (type == SystemButtonType::Close) {
            ImVec2 a1 = ImVec2(center.x - cross_extent, center.y - cross_extent);
            ImVec2 b1 = ImVec2(center.x + cross_extent, center.y + cross_extent);
            ImVec2 a2 = ImVec2(center.x - cross_extent, center.y + cross_extent);
            ImVec2 b2 = ImVec2(center.x + cross_extent, center.y - cross_extent);
            draw_list->AddLine(a1, b1, col_u32, line_thickness);
            draw_list->AddLine(a2, b2, col_u32, line_thickness);
        }
        else if (type == SystemButtonType::Minimize) {
            ImVec2 a = ImVec2(center.x - cross_extent, center.y);
            ImVec2 b = ImVec2(center.x + cross_extent, center.y);
            draw_list->AddLine(a, b, col_u32, line_thickness);
        }
        else if (type == SystemButtonType::Maximize) {
            float half = cross_extent * 0.85f;
            ImVec2 tl = ImVec2(center.x - half, center.y - half);
            ImVec2 br = ImVec2(center.x + half, center.y + half);
            draw_list->AddRect(tl, br, col_u32, 0.0f, 0, line_thickness);
        }

        return ImGui::IsItemClicked();
    }

    void ImGuiFramedWindow::renderFrameManually() {
#       ifdef _WIN32
        if (!m_in_manual_sizing) return;
#       endif
        tick();
        drawUi();
        drawContent();
        present();
    }

} // namespace ImGuiX::Windows