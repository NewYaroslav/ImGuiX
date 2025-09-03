#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL.h>
#include <SDL_opengles2.h>

namespace ImGuiX {

    WindowInstance::~WindowInstance() noexcept  {
        saveIniNow();

#       ifdef IMGUI_ENABLE_IMPLOT
        if (m_implot_ctx) {
            ImPlot::SetCurrentContext(m_implot_ctx);
            ImPlot::DestroyContext(m_implot_ctx);
            m_implot_ctx = nullptr;
        }
#       endif

#       ifdef IMGUI_ENABLE_IMPLOT3D
        if (m_implot3d_ctx) {
            ImPlot3D::SetCurrentContext(m_implot3d_ctx);
            ImPlot3D::DestroyContext(m_implot3d_ctx);
            m_implot3d_ctx = nullptr;
        }
#       endif

        if (m_imgui_ctx) {
            ImGui::SetCurrentContext(m_imgui_ctx);
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext(m_imgui_ctx);
            m_imgui_ctx = nullptr;
        }

        if (m_gl_context) { 
            SDL_GL_DeleteContext(m_gl_context); 
            m_gl_context = nullptr; 
        }
        if (m_window)     { 
            SDL_DestroyWindow(m_window);        
            m_window = nullptr;     
        }
    }

    bool WindowInstance::create() {
        if (m_window || m_is_open) return true;
        if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
#ifdef __EMSCRIPTEN__
        SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR, u8"#canvas");
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, u8"0");
        SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, u8"1");
#endif
        m_window = SDL_CreateWindow(name().c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   width(), height(), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        if (!m_window) return false;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        
        m_gl_context = SDL_GL_CreateContext(m_window);
        SDL_GL_MakeCurrent(m_window, m_gl_context);
        
        IMGUI_CHECKVERSION();

        m_imgui_ctx = ImGui::CreateContext();
        ImGui::SetCurrentContext(m_imgui_ctx);
        
#       ifdef IMGUI_ENABLE_IMPLOT
        m_implot_ctx = ImPlot::CreateContext();
        ImPlot::SetCurrentContext(m_implot_ctx);
#       endif

#       ifdef IMGUI_ENABLE_IMPLOT3D
        m_implot3d_ctx = ImPlot3D::CreateContext();
        ImPlot3D::SetCurrentContext(m_implot3d_ctx);
#       endif
        
        ImGui_ImplSDL2_InitForOpenGL(m_window, m_gl_context);
        ImGui_ImplOpenGL3_Init(selectGlslForSdl(m_window));
        
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        int window_w, window_h, drawable_w, drawable_h;
        SDL_GetWindowSize(m_window, &window_w, &window_h);
        SDL_GL_GetDrawableSize(m_window, &drawable_w, &drawable_h);
        io.DisplayFramebufferScale = ImVec2(
            (float)drawable_w / window_w,
            (float)drawable_h / window_h
        );
        m_is_open = true;
        return true;
    }

    bool WindowInstance::create(int w, int h) {
        m_width = w;
        m_height = h;
        return create();
    }
    
    const char* WindowInstance::selectGlslForSdl(SDL_Window* w) noexcept {
#   if !defined(IMGUIX_USE_SDL2_BACKEND)
        (void)w; return IMGUIX_GLSL_VERSION;
#   else
        if (!w) return IMGUIX_GLSL_VERSION;

        int major=0, minor=0, profile=0;
        if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major) != 0) return IMGUIX_GLSL_VERSION;
        if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor) != 0) return IMGUIX_GLSL_VERSION;
        if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile) != 0) return IMGUIX_GLSL_VERSION;

        if (profile & SDL_GL_CONTEXT_PROFILE_ES)
            return (major >= 3) ? u8"#version 300 es" : u8"#version 100";

#       if defined(__APPLE__)
        return u8"#version 150";
#       else
        if (major > 3 || (major == 3 && minor >= 2) || (profile & SDL_GL_CONTEXT_PROFILE_CORE))
            return u8"#version 150";
        return u8"#version 130";
#       endif
#   endif
    }

    void WindowInstance::handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                Events::WindowClosedEvent evt(id(), name());
                notify(evt);
                SDL_GL_DeleteContext(m_gl_context);
                SDL_DestroyWindow(m_window);
                m_gl_context = nullptr;
                m_window = nullptr;
                m_is_open = false;
            }
        }
    }

    void WindowInstance::tick() {
        if (!m_window) return;
        setCurrentWindow();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        updateCurrentTheme();
    }

    void WindowInstance::present() {
        if (!m_window) return;
        setCurrentWindow();
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

    bool WindowInstance::isMaximized() const {
        if (m_window)
            return SDL_GetWindowFlags(m_window) & SDL_WINDOW_MAXIMIZED;
        return false;
    }

    void WindowInstance::toggleMaximizeRestore() {
        if (!m_window) return;
        if (isMaximized())
            SDL_RestoreWindow(m_window);
        else
            SDL_MaximizeWindow(m_window);
    }

    void WindowInstance::close() {
        m_is_open = false;
        if (m_window) {
            Events::WindowClosedEvent evt(id(), name());
            notify(evt);
            SDL_GL_DeleteContext(m_gl_context);
            SDL_DestroyWindow(m_window);
            m_gl_context = nullptr;
            m_window = nullptr;
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
    
    void WindowInstance::setCurrentWindow() {
        if (!m_window || !m_gl_context || !m_imgui_ctx) return;
#       ifdef IMGUI_ENABLE_IMPLOT
        if (!m_implot_ctx) return;
#       endif
#       ifdef IMGUI_ENABLE_IMPLOT3D
        if (!m_implot3d_ctx) return;
#       endif
        SDL_GL_MakeCurrent(m_window, m_gl_context);
        ImGui::SetCurrentContext(m_imgui_ctx);
#       ifdef IMGUI_ENABLE_IMPLOT
        ImPlot::SetCurrentContext(m_implot_ctx);
#       endif
#       ifdef IMGUI_ENABLE_IMPLOT3D
        ImPlot3D::SetCurrentContext(m_implot3d_ctx);
#       endif
    }

} // namespace ImGuiX


