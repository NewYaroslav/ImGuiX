#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <imguix/extensions/blend_colors.hpp>

namespace ImGuiX::Windows {

    bool ImGuiFramedWindow::create() {
        if (m_window || m_is_open) return true;
        if (!glfwInit()) return false;
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        m_window = glfwCreateWindow(width(), height(), name().c_str(), nullptr, nullptr);
        if (!m_window) return false;
        glfwMakeContextCurrent(m_window);

        IMGUI_CHECKVERSION();
        if (!ImGui::GetCurrentContext()) { 
            ImGui::CreateContext();
#           ifdef IMGUI_ENABLE_IMPLOT
            m_implot_ctx = ImPlot::CreateContext();
            ImPlot::SetCurrentContext(m_implot_ctx);
#           endif
        }
        
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init();
        m_is_open = true;
        return true;
    }

    void ImGuiFramedWindow::tick() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (hasFlag(m_flags, WindowFlags::EnableTransparency)) {
            glClearColor(0.f, 0.f, 0.f, 0.f);
        } else {
            glClearColor(m_config.clear_color.x, m_config.clear_color.y,
                         m_config.clear_color.z, m_config.clear_color.w);
        }
        glClear(GL_COLOR_BUFFER_BIT);
        updateCurrentTheme();
    }

    void ImGuiFramedWindow::drawUi() {
#       ifdef IMGUI_ENABLE_IMPLOT
        ImPlot::SetCurrentContext(m_implot_ctx);
#       endif
        ImGui::PushID(id());

        int fb_w = 0, fb_h = 0;
        glfwGetFramebufferSize(m_window, &fb_w, &fb_h);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)fb_w, (float)fb_h));

        ImVec2 char_size = ImGui::CalcTextSize(u8"W");
        ImVec2 padding = ImGui::GetStyle().WindowPadding;
        float title_padding_x = padding.x + char_size.x * 2.0f;

        const ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        if (hasFlag(m_flags, WindowFlags::DisableBackground) || m_disable_background) {
            ImVec4 border_color = ImGui::GetStyle().Colors[ImGuiCol_Border];
            ImVec4 background_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
            ImVec4 new_color = Extensions::BlendColors(border_color, background_color);
            ImGui::PushStyleColor(ImGuiCol_Border, new_color);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(u8"##imguix_framed_window", nullptr, flags);
        ImGui::PopStyleVar();

        if (hasFlag(m_flags, WindowFlags::DisableBackground) || m_disable_background) {
            ImGui::PopStyleColor(2);
        }

        ImGui::BeginChild(u8"##imguix_title_bar", ImVec2(0, m_config.title_bar_height), false,
                          ImGuiWindowFlags_NoScrollbar |
                          ImGuiWindowFlags_NoDecoration);

        {
            ImVec2 p_min = ImGui::GetWindowPos();
            ImVec2 p_max = ImVec2(p_min.x + ImGui::GetWindowWidth(), p_min.y + m_config.title_bar_height);
            ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, ImGui::GetColorU32(ImGuiCol_TitleBgActive));
        }

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + title_padding_x);
        drawTitleBarText();

        if (hasFlag(m_flags, WindowFlags::ShowControlButtons)) {
            if (hasFlag(m_flags, WindowFlags::MacStyledControlButtons)) {
                drawMacStyledControlButtons(title_padding_x);
            } else if (hasFlag(m_flags, WindowFlags::ImGuiStyledControlButtons)) {
                drawImGuiStyledControlButtons(title_padding_x);
            } else {
                drawControlButtons(title_padding_x);
            }
        }

        ImGui::EndChild();

        if (hasFlag(m_flags, WindowFlags::HasMenuBar)) {
            ImGui::SetCursorPosY(m_config.title_bar_height);
            ImGui::BeginChild(u8"##imguix_menu_bar", ImVec2(0, 0), false,
                              ImGuiWindowFlags_MenuBar |
                              ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoDecoration |
                              ImGuiWindowFlags_AlwaysAutoResize);
            drawMenuBar();
            ImGui::EndChild();
        }

        for (auto& ctrl : m_controllers) {
            ctrl->drawUi();
        }

        ImGui::End();
        ImGui::PopID();
    }

} // namespace ImGuiX::Windows

