#pragma once
#ifndef _IMGUIX_CORE_WINDOW_INSTANCE_IWINDOW_INSTANCE_HPP_INCLUDED
#define _IMGUIX_CORE_WINDOW_INSTANCE_IWINDOW_INSTANCE_HPP_INCLUDED

/// \file WindowInstance.hpp
/// \brief Abstract base class for a window instance in ImGuiX.

#ifdef IMGUIX_USE_SFML_BACKEND
#   include "DeltaClockSfml.hpp"
#   include <imgui-SFML.h>
#   include <SFML/Graphics.hpp>
#endif
#ifdef IMGUIX_USE_GLFW_BACKEND
#   include <imgui_impl_glfw.h>
#   include <imgui_impl_opengl3.h>
#   include <GLFW/glfw3.h>
#endif
#ifdef IMGUIX_USE_SDL2_BACKEND
#   include <imgui_impl_sdl2.h>
#   include <imgui_impl_opengl3.h>
#   include <SDL.h>
#   include <SDL_opengles2.h>
#endif

namespace ImGuiX {

    /// \brief Abstract base class for a window instance.
    ///
    /// Combines event handling, rendering, and controller orchestration.
    /// Derive from this class to implement platform-specific windows.
    class WindowInstance : public WindowControl, public Pubsub::EventMediator {
    public:
        /// \brief Constructs the window with a unique ID and name.
        /// \param id Unique window identifier.
        /// \param app Reference to application control interface.
        /// \param name Window name (title).
        explicit WindowInstance(int id, ApplicationControl& app, std::string name);
        
        WindowInstance(const WindowInstance&) = delete;
        WindowInstance& operator=(WindowInstance) = delete;

        WindowInstance(WindowInstance&&) = delete;
        WindowInstance& operator=(WindowInstance&&) = delete;

        virtual ~WindowInstance() {
            unsubscribeAll();
        }
        
        /// \brief Optional callback invoked after the window is added to the manager.
        virtual void onInit() {}

        /// \brief Initializes the window (e.g., creates backend resources).
        virtual bool create();
        
        virtual bool create(int w, int h);

        /// \brief Processes input events and window messages.
        virtual void handleEvents();

        /// \brief Updates logic for the current frame.
        virtual void tick();

        /// \brief Renders scene content (e.g., world, game objects).
        virtual void drawContent();

        /// \brief Renders UI overlays, HUDs, debug panels.
        virtual void drawUi();

        /// \brief Finalizes frame and presents to screen.
        virtual void present();

        /// \brief Creates and registers a controller of given type.
        /// \tparam T Controller type derived from Controller.
        /// \tparam Args Arguments passed to the controller constructor.
        /// \return Reference to the created controller.
        template<typename T, typename... Args>
        T& createController(Args&&... args);

        // --- WindowControl interface ---

        int id() const override;
        const std::string& name() const override;

        int width() const override;
        int height() const override;
        void setSize(int w, int h) override;

        void close() override;
        void minimize() override;
        void maximize() override;
        void restore() override;
        bool isMaximized() const override;
        void toggleMaximizeRestore() override;

        bool setActive(bool active) override;
        bool isActive() const override;

        void setVisible(bool visible) override;
        bool isOpen() const override;

        Pubsub::EventBus& eventBus() override;
        ResourceRegistry& registry() override;
#       ifdef IMGUIX_USE_SFML_BACKEND
        sf::RenderWindow& getRenderTarget() override;
#       endif

    protected:
#       ifdef IMGUIX_USE_SFML_BACKEND
        sf::RenderWindow m_window;
#       elif defined(IMGUIX_USE_GLFW_BACKEND)
        GLFWwindow* m_window = nullptr;
#       elif defined(IMGUIX_USE_SDL2_BACKEND)
        SDL_Window* m_window = nullptr;
        SDL_GLContext m_gl_context = nullptr;
#       endif
        int m_window_id;
        std::string m_window_name;
        int m_width = 1280;
        int m_height = 720;
        bool m_is_active = true;
        bool m_is_open = false;
        bool m_is_visible = true;

        ApplicationControl& m_application;
        std::vector<std::unique_ptr<Controller>> m_controllers;
    };

} // namespace imguix

#ifdef IMGUIX_HEADER_ONLY
#   include "WindowInstance.ipp"
#   ifdef IMGUIX_USE_SFML_BACKEND
#       include "SfmlWindowInstance.ipp"
#   elif defined(IMGUIX_USE_GLFW_BACKEND)
#       include "GlfwWindowInstance.ipp"
#   elif defined(IMGUIX_USE_SDL2_BACKEND)
#       include "Sdl2WindowInstance.ipp"
#   endif
#endif

#endif // _IMGUIX_CORE_WINDOW_INSTANCE_IWINDOW_INSTANCE_HPP_INCLUDED