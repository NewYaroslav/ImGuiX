#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL.h>
#include <SDL_opengles2.h>

namespace ImGuiX {

    bool WindowInstance::create() {
        if (m_window || m_is_open) return true;
        if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
        m_window = SDL_CreateWindow(name().c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   width(), height(), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (!m_window) return false;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        m_gl_context = SDL_GL_CreateContext(m_window);
        SDL_GL_MakeCurrent(m_window, m_gl_context);
        IMGUI_CHECKVERSION();
        if (!ImGui::GetCurrentContext())
            ImGui::CreateContext();
        ImGui_ImplSDL2_InitForOpenGL(m_window, m_gl_context);
        ImGui_ImplOpenGL3_Init("#version 100");
        m_is_open = true;
        return true;
    }

    bool WindowInstance::create(int w, int h) {
        m_width = w;
        m_height = h;
        return create();
    }

    void WindowInstance::handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                Events::WindowClosedEvent evt(id(), name());
                notify(evt);
                SDL_DestroyWindow(m_window);
                SDL_GL_DeleteContext(m_gl_context);
                m_window = nullptr;
                m_gl_context = nullptr;
                m_is_open = false;
            }
        }
    }

    void WindowInstance::tick() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void WindowInstance::present() {
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(m_window);
    }

    void WindowInstance::setSize(int w, int h) {
        m_width = w;
        m_height = h;
        if (m_window) SDL_SetWindowSize(m_window, w, h);
    }

    void WindowInstance::restore() {
        if (m_window) SDL_RestoreWindow(m_window);
    }

    void WindowInstance::minimize() {
        if (m_window) SDL_MinimizeWindow(m_window);
    }

    void WindowInstance::maximize() {
        if (m_window) SDL_MaximizeWindow(m_window);
    }

    void WindowInstance::close() {
        m_is_open = false;
        if (m_window) {
            Events::WindowClosedEvent evt(id(), name());
            notify(evt);
            SDL_DestroyWindow(m_window);
            SDL_GL_DeleteContext(m_gl_context);
            m_window = nullptr;
            m_gl_context = nullptr;
        }
    }

    bool WindowInstance::setActive(bool active) {
        if (m_window) {
            if (active)
                SDL_GL_MakeCurrent(m_window, m_gl_context);
            else
                SDL_GL_MakeCurrent(m_window, nullptr);
            m_is_active = active;
        }
        return m_is_active;
    }

    bool WindowInstance::isActive() const {
        if (m_window)
            return SDL_GetWindowFlags(m_window) & SDL_WINDOW_INPUT_FOCUS;
        return m_is_active;
    }

    void WindowInstance::setVisible(bool visible) {
        m_is_visible = visible;
        if (m_window) {
            if (visible) SDL_ShowWindow(m_window);
            else SDL_HideWindow(m_window);
        }
    }

    bool WindowInstance::isOpen() const {
        return m_is_open && m_window != nullptr;
    }

} // namespace ImGuiX


