#pragma once
#ifndef _IMGUIX_CORE_WINDOW_INSTANCE_IWINDOW_INSTANCE_HPP_INCLUDED
#define _IMGUIX_CORE_WINDOW_INSTANCE_IWINDOW_INSTANCE_HPP_INCLUDED

/// \file WindowInstance.hpp
/// \brief Abstract base class for a window instance in ImGuiX.
/// \ingroup Core

#ifdef IMGUIX_USE_SFML_BACKEND
#    include "DeltaClockSfml.hpp"
#    include <imgui-SFML.h>
#    include <SFML/Graphics.hpp>
#endif
#ifdef IMGUIX_USE_GLFW_BACKEND
#    include <imgui_impl_glfw.h>
#    include <imgui_impl_opengl3.h>
#    include <GLFW/glfw3.h>
#    include "imgui_glsl_version.hpp"
#endif
#ifdef IMGUIX_USE_SDL2_BACKEND
#    include <imgui_impl_sdl2.h>
#    include <imgui_impl_opengl3.h>
#    include <SDL.h>
#    include <SDL_opengles2.h>
#    include "imgui_glsl_version.hpp"
#endif

#include "WindowInterface.hpp"
#include <imguix/config/paths.hpp>

namespace ImGuiX {

    /// \brief Abstract base class for a window instance.
    ///
    /// Combines event handling, rendering, and controller orchestration.
    /// Derive from this class to implement platform-specific windows.
    class WindowInstance :
        private WindowInterface,
        public Pubsub::EventMediator {
    public:
        /// \brief Constructs the window with a unique ID and name.
        /// \param id Unique window identifier.
        /// \param app Reference to application context interface.
        /// \param name Window name (title).
        explicit WindowInstance(int id, ApplicationContext& app, std::string name);
        
        WindowInstance(const WindowInstance&) = delete;
        WindowInstance& operator=(WindowInstance) = delete;

        WindowInstance(WindowInstance&&) = delete;
        WindowInstance& operator=(WindowInstance&&) = delete;

        virtual ~WindowInstance() noexcept;
        
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
        /// \tparam ControllerType Controller type derived from Controller.
        /// \tparam Args Arguments passed to the controller constructor.
        /// \return Reference to the created controller.
        template <typename ControllerType, typename... Args>
        ControllerType& createController(Args&&... args);

        // --- WindowInterface interface ---

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
        
        /// \brief Makes the window context current for rendering.
        /// Call only between frames before ImGui::NewFrame().
        virtual void setCurrentWindow();

        /// \brief Access to the global event bus.
        Pubsub::EventBus& eventBus() override;

        /// \brief Access to the shared resource registry.
        ResourceRegistry& registry() override;

        /// \brief Access to the global options store.
        OptionsStore::Control& options() override;

        /// \brief Read-only access to the global options store.
        const OptionsStore::View& options() const override;

        /// \brief Reference to the owning application.
        ApplicationContext& application() override;

#       ifdef IMGUIX_USE_SFML_BACKEND
        sf::RenderWindow& getRenderTarget() override;
#       endif

        // --- Lang and Fonts ---

        /// \brief Get language store.
        /// \return Language store.
        const ImGuiX::I18N::LangStore& langStore() const override;
        
        ImGuiX::Fonts::FontManager::View& fontsView() noexcept { return m_font_manager.view(); }
        
        ImGuiX::Fonts::FontManager::Control& fontsControl() noexcept { return m_font_manager.control(); }

        // ---

        /// \brief Compute file path for storing ImGui ini settings.
        /// \return Absolute path to ini file.
        /// \note Internal use.
        std::string iniPath() const;

        /// \brief Prepare ImGui to use the window-specific ini file.
        /// \note Internal use.
        void initIni();

        /// \brief Load ImGui settings from the ini file if not already loaded.
        /// \note Internal use.
        void loadIni();

        /// \brief Save ImGui ini settings to disk.
        /// \note Internal use.
        void saveIniNow();
        
        /// \brief Request window to switch its UI language.
        /// \param lang Language code.
        void requestLanguageChange(const std::string& lang);
        
        /// \brief Apply pending language change.
        /// \note Internal use.
        void applyPendingLanguageChange();
        
        /// \brief Start font initialization.
        /// \note Internal use.
        void fontsStartInit();

        /// \brief Build fonts atlas.
        /// \note Internal use.
        void buildFonts();

    protected:

        // --- onInit-фаза: ручная сборка атласа ---

        /// \brief Begin manual font configuration.
        void fontsBeginManual();

            void fontsSetLocale(std::string locale);

            void fontsSetRangesPreset(std::string preset);

            void fontsSetRangesExplicit(const std::vector<ImWchar>& pairs);

            void fontsClearRanges();

        /// \brief Add body font file.
        /// \param ff Font file.
        void fontsAddBody(const ImGuiX::Fonts::FontFile& ff);

        /// \brief Add headline font file.
        /// \param role Font role.
        /// \param ff Font file.
        void fontsAddHeadline(ImGuiX::Fonts::FontRole role, const ImGuiX::Fonts::FontFile& ff);

        /// \brief Add merge font file for role.
        /// \param role Font role.
        /// \param ff Font file.
        void fontsAddMerge(ImGuiX::Fonts::FontRole role, const ImGuiX::Fonts::FontFile& ff);
        
        /// \brief Add merge font file to body chain.
        /// \param ff Font file.
        void fontsAddMerge(const ImGuiX::Fonts::FontFile& ff);
        
        /// \brief Build fonts immediately.
        /// \return True on success.
        bool fontsBuildNow();

    protected:
#ifdef IMGUIX_USE_SFML_BACKEND
        sf::RenderWindow m_window; ///< Underlying SFML render window.
#elif defined(IMGUIX_USE_GLFW_BACKEND)
        GLFWwindow* m_window = nullptr; ///< Pointer to the GLFW window.
        ImGuiContext* m_imgui_ctx = nullptr;
        const char* selectGlslForGlfw(GLFWwindow* w) noexcept;
#elif defined(IMGUIX_USE_SDL2_BACKEND)
        SDL_Window* m_window = nullptr; ///< SDL window handle.
        SDL_GLContext m_gl_context = nullptr; ///< Associated GL context.
        SDL_Window* m_window = nullptr;
        ImGuiContext* m_imgui_ctx = nullptr;
        const char* selectGlslForSdl(SDL_Window* w) noexcept;
#endif
        int m_window_id;                    ///< Unique window identifier.
        std::string m_window_name;          ///< Internal window name.
        int m_width = 1280;                 ///< Current window width.
        int m_height = 720;                 ///< Current window height.
        bool m_is_active = true;            ///< Whether the window has focus.
        bool m_is_open = false;             ///< Whether the window is currently open.
        bool m_is_visible = true;           ///< Visibility flag.

        ApplicationContext& m_application;  ///< Reference to the owning application.
        std::vector<std::unique_ptr<Controller>> m_controllers; ///< Attached controllers.
        std::string m_ini_path;             ///< Path to the window-specific ImGui ini file.
        bool m_is_ini_once = false;         ///< Ensures imgui ini is saved only once.
        bool m_is_ini_loaded = false;       ///< Indicates whether ini settings have been loaded.
    
        bool m_is_fonts_manual = false;     ///< 
        bool m_in_init_phase = false;       ///< охранник фазы
        bool m_is_fonts_init = false;       ///< 
        ImGuiX::Fonts::FontManager m_font_manager; ///< 
        ImGuiX::I18N::LangStore    m_lang_store{}; ///< 
        std::string                m_pending_lang; ///< 


        /// \brief Requests the window to switch its UI language.
        /// \param lang Language code to apply.
        virtual void onBeforeLanguageApply(const std::string& /*lang*/) {};
    };

} // namespace ImGuiX

#ifdef IMGUIX_HEADER_ONLY
#    include "WindowInstance.ipp"
#    ifdef IMGUIX_USE_SFML_BACKEND
#        include "SfmlWindowInstance.ipp"
#    elif defined(IMGUIX_USE_GLFW_BACKEND)
#        include "GlfwWindowInstance.ipp"
#    elif defined(IMGUIX_USE_SDL2_BACKEND)
#        include "Sdl2WindowInstance.ipp"
#    endif
#endif

#endif // _IMGUIX_CORE_WINDOW_INSTANCE_IWINDOW_INSTANCE_HPP_INCLUDED
