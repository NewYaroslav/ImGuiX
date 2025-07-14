#ifdef _WIN32
#   include <windows.h>
#endif
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace ImGuiX {

    bool WindowInstance::create() {
        if (m_window || m_is_open) return true;
        if (!glfwInit()) return false;
        m_window = glfwCreateWindow(width(), height(), name().c_str(), nullptr, nullptr);
        if (!m_window) return false;
        glfwMakeContextCurrent(m_window);
        IMGUI_CHECKVERSION();
        if (!ImGui::GetCurrentContext())
            ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init();
        m_is_open = true;
        return true;
    }

    bool WindowInstance::create(int w, int h) {
        m_width = w;
        m_height = h;
        return create();
    }

    void WindowInstance::handleEvents() {
        if (!m_window) return;
        glfwPollEvents();
        if (glfwWindowShouldClose(m_window)) {
            Events::WindowClosedEvent evt(id(), name());
            notify(evt);
            glfwDestroyWindow(m_window);
            m_window = nullptr;
            m_is_open = false;
        }
    }

    void WindowInstance::tick() {
        if (!m_window) return;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void WindowInstance::present() {
        if (!m_window) return;
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
    }

    void WindowInstance::setSize(int w, int h) {
        m_width = w;
        m_height = h;
        if (m_window) {
            glfwSetWindowSize(m_window, w, h);
        }
    }

    void WindowInstance::restore() {
        if (m_window) glfwRestoreWindow(m_window);
    }

    void WindowInstance::minimize() {
        if (m_window) glfwIconifyWindow(m_window);
    }

    void WindowInstance::maximize() {
        if (m_window) glfwMaximizeWindow(m_window);
    }

    void WindowInstance::close() {
        m_is_open = false;
        if (m_window) {
            Events::WindowClosedEvent evt(id(), name());
            notify(evt);
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
    }

    bool WindowInstance::setActive(bool active) {
        if (m_window) {
            glfwMakeContextCurrent(active ? m_window : nullptr);
            m_is_active = active;
        }
        return m_is_active;
    }

    bool WindowInstance::isActive() const {
        if (m_window)
            return glfwGetWindowAttrib(m_window, GLFW_FOCUSED);
        return m_is_active;
    }

    bool WindowInstance::isMaximized() const {
        if (m_window)
            return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED);
        return false;
    }

    void WindowInstance::toggleMaximizeRestore() {
        if (!m_window) return;
        if (glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED))
            glfwRestoreWindow(m_window);
        else
            glfwMaximizeWindow(m_window);
    }

    void WindowInstance::setVisible(bool visible) {
        m_is_visible = visible;
        if (m_window) {
            if (visible) glfwShowWindow(m_window);
            else glfwHideWindow(m_window);
        }
    }

    bool WindowInstance::isOpen() const {
        return m_is_open && m_window && !glfwWindowShouldClose(m_window);
    }

} // namespace ImGuiX

