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

        /// \brief Returns the unique ID of this window.
        int id() const override;

        /// \brief Returns the window name used as title.
        const std::string& name() const override;

        /// \brief Current width of the window in pixels.
        int width() const override;

        /// \brief Current height of the window in pixels.
        int height() const override;

        /// \brief Sets window dimensions in pixels.
        void setSize(int w, int h) override;
        
        /// \brief Sets the window icon from an image file (currently SFML only).
        /// \param path Path to the icon image file (must be .png or .bmp, 32x32 or 64x64 recommended).
        /// \return True if the icon was loaded and applied successfully.
        bool setWindowIcon(const std::string& path) override;
		
        /// \brief Enables or disables clearing the background between frames.
                void setDisableBackground(bool disable) override {};

        /// \brief Requests the window to close.
        void close() override;

        /// \brief Minimizes the window.
        void minimize() override;

        /// \brief Maximizes the window.
        void maximize() override;

        /// \brief Restores the window from minimized or maximized state.
        void restore() override;

        /// \brief Checks whether the window is maximized.
        bool isMaximized() const override;

        /// \brief Toggles between maximized and restored states.
        void toggleMaximizeRestore() override;

        /// \brief Activates or deactivates the window.
        bool setActive(bool active) override;

        /// \brief Returns true if the window currently has focus.
        bool isActive() const override;

        /// \brief Shows or hides the window.
        void setVisible(bool visible) override;

        /// \brief Returns true if the window is open.
        bool isOpen() const override;

        /// \brief Access to the global event bus.
        Pubsub::EventBus& eventBus() override;

        /// \brief Access to the shared resource registry.
        ResourceRegistry& registry() override;

        /// \brief Reference to the owning application.
        ApplicationControl& application() override;
#       ifdef IMGUIX_USE_SFML_BACKEND
        sf::RenderWindow& getRenderTarget() override;
#       endif

    protected:
#       ifdef IMGUIX_USE_SFML_BACKEND
        sf::RenderWindow m_window; ///< Underlying SFML render window.
#       elif defined(IMGUIX_USE_GLFW_BACKEND)
        GLFWwindow* m_window = nullptr; ///< Pointer to the GLFW window.
#       elif defined(IMGUIX_USE_SDL2_BACKEND)
        SDL_Window* m_window = nullptr;   ///< SDL window handle.
        SDL_GLContext m_gl_context = nullptr; ///< Associated GL context.
#       endif
        int m_window_id;                    ///< Unique window identifier.
        std::string m_window_name;          ///< Internal window name.
        int m_width = 1280;                 ///< Current window width.
        int m_height = 720;                 ///< Current window height.
        bool m_is_active = true;            ///< Whether the window has focus.
        bool m_is_open = false;             ///< Whether the window is currently open.
        bool m_is_visible = true;           ///< Visibility flag.

        ApplicationControl& m_application;  ///< Reference to the owning application.
        std::vector<std::unique_ptr<Controller>> m_controllers; ///< Attached controllers.
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