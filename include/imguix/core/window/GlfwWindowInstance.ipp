#ifdef _WIN32
#   include <windows.h>
#endif
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace ImGuiX {

    WindowInstance::~WindowInstance() noexcept  {
        saveIniNow();
        if (m_imgui_ctx) {
            ImGui::SetCurrentContext(m_imgui_ctx);
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext(m_imgui_ctx);
            m_imgui_ctx = nullptr;
        }

        if (m_window) {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
    }

    bool WindowInstance::create() {
        if (m_window || m_is_open) return true;
        if (!glfwInit()) return false;
        m_window = glfwCreateWindow(width(), height(), name().c_str(), nullptr, nullptr);
        if (!m_window) return false;
        
        glfwMakeContextCurrent(m_window);
        
        IMGUI_CHECKVERSION();
        
        m_imgui_ctx = ImGui::CreateContext();
        ImGui::SetCurrentContext(m_imgui_ctx);

        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init(selectGlslForGlfw(m_window));
        m_is_open = true;
        return true;
    }

    bool WindowInstance::create(int w, int h) {
        m_width = w;
        m_height = h;
        return create();
    }
    
    const char* WindowInstance::selectGlslForGlfw(GLFWwindow* w) noexcept {
#   if !defined(IMGUIX_USE_GLFW_BACKEND)
        (void)w; return IMGUIX_GLSL_VERSION;
#   else
        if (!w) return IMGUIX_GLSL_VERSION;

        int client = glfwGetWindowAttrib(w, GLFW_CLIENT_API);
        if (client == GLFW_NO_API) return IMGUIX_GLSL_VERSION;

        // OpenGL ES?
        if (client == GLFW_OPENGL_ES_API) {
            int major = glfwGetWindowAttrib(w, GLFW_CONTEXT_VERSION_MAJOR);
            return (major >= 3) ? u8"#version 300 es" : u8"#version 100";
        }

#       if defined(__APPLE__)
        // macOS Core 3.2+
        return u8"#version 150";
#       else
        int major = glfwGetWindowAttrib(w, GLFW_CONTEXT_VERSION_MAJOR);
        int minor = glfwGetWindowAttrib(w, GLFW_CONTEXT_VERSION_MINOR);
        int profile = glfwGetWindowAttrib(w, GLFW_OPENGL_PROFILE);

        if (major > 3 || (major == 3 && minor >= 2) || profile == GLFW_OPENGL_CORE_PROFILE)
            return u8"#version 150";
        return u8"#version 130";
#       endif
#   endif
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
        setCurrentWindow();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void WindowInstance::present() {
        if (!m_window) return;
        setCurrentWindow();
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

    bool WindowInstance::isMaximized() const {
        if (m_window)
            return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED);
        return false;
    }

    void WindowInstance::toggleMaximizeRestore() {
        if (!m_window) return;
        if (isMaximized())
            glfwRestoreWindow(m_window);
        else
            glfwMaximizeWindow(m_window);
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
    
    void WindowInstance::setCurrentWindow() {
        if (!m_window || !m_imgui_ctx) return;
        glfwMakeContextCurrent(m_window);
        ImGui::SetCurrentContext(m_imgui_ctx);
    }

} // namespace ImGuiX

