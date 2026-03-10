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
#           ifdef IMGUIX_ENABLE_IMPLOT
            m_implot_ctx = ImPlot::CreateContext();
            ImPlot::SetCurrentContext(m_implot_ctx);
#           endif
#           ifdef IMGUIX_ENABLE_IMPLOT3D
            m_implot3d_ctx = ImPlot3D::CreateContext();
            ImPlot3D::SetCurrentContext(m_implot3d_ctx);
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
        Pubsub::SyncNotifier notifier{eventBus()};
        for (auto& ctrl : m_controllers) {
            ctrl->processFeatures(notifier);
        }
    }

    void ImGuiFramedWindow::drawUi() {
#       ifdef IMGUIX_ENABLE_IMPLOT
        ImPlot::SetCurrentContext(m_implot_ctx);
#       endif
#       ifdef IMGUIX_ENABLE_IMPLOT3D
        ImPlot3D::SetCurrentContext(m_implot3d_ctx);
#       endif
        ImGui::PushID(id());
        const ImGuiStyle& style = ImGui::GetStyle();

        int fb_w = 0, fb_h = 0;
        glfwGetFramebufferSize(m_window, &fb_w, &fb_h);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)fb_w, (float)fb_h));

        const float title_padding_x = 0.0f;
        const float menu_bar_height = ImGui::GetFrameHeight();

        const ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        if (hasFlag(m_flags, WindowFlags::DisableBackground) || m_disable_background) {
            ImVec4 border_color = style.Colors[ImGuiCol_Border];
            ImVec4 background_color = style.Colors[ImGuiCol_WindowBg];
            ImVec4 new_color = Extensions::BlendColors(border_color, background_color);
            ImGui::PushStyleColor(ImGuiCol_Border, new_color);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        }

        ImGui::Begin(u8"##imguix_framed_window", nullptr, flags);
        const float inset = style.WindowBorderSize;
        const ImVec2 padded_start = ImGui::GetCursorPos();

        if (hasFlag(m_flags, WindowFlags::DisableBackground) || m_disable_background) {
            ImGui::PopStyleColor(2);
        }

        ImGui::SetCursorPos(ImVec2(inset, inset));
        const float title_w = ImMax(0.0f, ImGui::GetWindowSize().x - 2.0f * inset);
        ImGui::BeginChild(u8"##imguix_title_bar", ImVec2(title_w, m_config.title_bar_height), false,
                          ImGuiWindowFlags_NoScrollbar |
                          ImGuiWindowFlags_NoDecoration |
                          ImGuiWindowFlags_NoBackground);

        {
            ImVec2 p_min = ImGui::GetWindowPos();
            ImVec2 p_max = ImVec2(p_min.x + ImGui::GetWindowWidth(), p_min.y + m_config.title_bar_height);
            ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, ImGui::GetColorU32(ImGuiCol_TitleBgActive));
        }

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

        const ImVec2 body_start(inset, padded_start.y + m_config.title_bar_height);
        const float body_width = ImMax(0.0f, ImGui::GetWindowSize().x - 2.0f * inset);
        const float requested_side_panel_width =
            m_config.side_panel_width > 0 ? static_cast<float>(m_config.side_panel_width) : 0.0f;
        const float side_panel_width = ImMin(requested_side_panel_width, body_width);
        const float main_region_width = ImMax(0.0f, body_width - side_panel_width);

        if (side_panel_width <= 0.0f) {
            ImGui::SetCursorPos(ImVec2(padded_start.x, padded_start.y + m_config.title_bar_height));

            if (hasFlag(m_flags, WindowFlags::HasMenuBar)) {
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
                ImGui::SetCursorPosY(padded_start.y + m_config.title_bar_height);
                if (ImGui::BeginChild(
                    u8"##imguix_menu_bar",
                    ImVec2(0.0f, menu_bar_height),
                    ImGuiChildFlags_None,
                    ImGuiWindowFlags_MenuBar |
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoDecoration
                )) {
                    drawMenuBar();
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();
            }

            for (auto& ctrl : m_controllers) {
                ctrl->drawUi();
            }
        } else {
            ImGui::SetCursorPos(body_start);
            if (ImGui::BeginChild(
                    u8"##imguix_side_panel",
                    ImVec2(side_panel_width, 0.0f),
                    ImGuiChildFlags_None,
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_NoBackground
                )) {
                ImVec2 p_min = ImGui::GetWindowPos();
                ImVec2 p_max = ImVec2(p_min.x + ImGui::GetWindowWidth(), p_min.y + ImGui::GetWindowHeight());
                ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, ImGui::GetColorU32(ImGuiCol_TitleBgActive));
                drawSidePanel();
            }
            ImGui::EndChild();
            ImGui::SameLine(0.0f, 0.0f);

            if (ImGui::BeginChild(
                    u8"##imguix_main_region",
                    ImVec2(main_region_width, 0.0f),
                    ImGuiChildFlags_AlwaysUseWindowPadding,
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_NoBackground
                )) {
                if (hasFlag(m_flags, WindowFlags::HasMenuBar)) {
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
                    if (ImGui::BeginChild(
                            u8"##imguix_menu_bar",
                            ImVec2(0.0f, menu_bar_height),
                            ImGuiChildFlags_None,
                            ImGuiWindowFlags_MenuBar |
                            ImGuiWindowFlags_NoScrollbar |
                            ImGuiWindowFlags_NoDecoration
                        )) {
                        drawMenuBar();
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                }

                for (auto& ctrl : m_controllers) {
                    ctrl->drawUi();
                }
            }
            ImGui::EndChild();
        }

        ImGui::End();
        ImGui::PopID();
        notifications().render();
    }

} // namespace ImGuiX::Windows
