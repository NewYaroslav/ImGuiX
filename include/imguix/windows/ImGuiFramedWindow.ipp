#include <imgui.h>
#include <imguix/widgets/controls/circle_button.hpp>
#include <imguix/widgets/controls/system_button.hpp>

namespace ImGuiX::Windows {
    
    ImGuiFramedWindow::ImGuiFramedWindow(
            int id, 
            ApplicationContext& app,
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
    
    void ImGuiFramedWindow::setDisableBackground(bool disable) {
        m_disable_background = disable;
    }
    
    void ImGuiFramedWindow::drawTitleBarText() {
        float padding_y = (m_config.title_bar_height - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::SetCursorPosY(padding_y);
        ImGui::Text(u8"%s", m_title.c_str());
    }

    void ImGuiFramedWindow::drawControlButtons(float title_padding_x) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        ImVec2 char_size = ImGui::CalcTextSize(u8"W");
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
        if (Widgets::CircleButton(u8"##imguix_btn_minimize", btn_diameter, ImVec4(1.0f, 0.74f, 0.18f, 1.0f))) {
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
        if (Widgets::CircleButton(u8"##imguix_btn_maximize", btn_diameter, ImVec4(0.15f, 0.79f, 0.25f, 1.0f))) {
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
        if (Widgets::CircleButton(u8"##imguix_btn_close", btn_diameter, ImVec4(1.0f, 0.0f, 0.0f, 1.0f))) {
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
        if (Widgets::SystemButton(u8"##imguix_btn_minimize", Widgets::SystemButtonType::Minimize, btn_size)) {
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
        if (Widgets::SystemButton(u8"##imguix_btn_maximize", Widgets::SystemButtonType::Maximize, btn_size)) {
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
        if (Widgets::SystemButton(u8"##imguix_btn_close", Widgets::SystemButtonType::Close, btn_size)) {
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