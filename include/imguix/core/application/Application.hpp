#pragma once
#ifndef _IMGUIX_CORE_APPLICATION_HPP_INCLUDED
#define _IMGUIX_CORE_APPLICATION_HPP_INCLUDED

/// \file Application.hpp
/// \brief Defines the main application class responsible for window management, event bus, and application loop.

#include <atomic>
#include <thread>

#ifndef IMGUIX_CONFIG_DIR
#   define IMGUIX_CONFIG_DIR "data/config"
#endif

#ifndef IMGUIX_INI_PATH
#	define IMGUIX_INI_PATH   IMGUIX_CONFIG_DIR "/imgui.ini"
#endif

namespace ImGuiX {

    /// \brief Main application class.
    ///
    /// Controls window lifecycle, global event dispatching, and the main loop.
    class Application : public ApplicationControl {
    public:
        /// \brief Constructs the application instance.
        Application();

        ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(Application) = delete;

        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        /// \brief Starts the application loop.
        /// \param async If true, runs the main loop in a separate thread.
        void run(bool async = false);

        /// \brief Creates a new window instance of the specified type.
        /// \tparam WindowType Type derived from WindowInstance.
        /// \tparam Args Arguments forwarded to the window constructor.
        /// \return Reference to the created window instance.
        template<typename T, typename... Args>
        T& createWindow(Args&&... args);
        
        /// \brief Creates and registers a model.
        /// \tparam T Class derived from Model.
        /// \tparam Args Arguments forwarded to the constructor.
        /// \return Reference to the created model.
        template<typename T, typename... Args>
        T& createModel(Args&&... args);

        /// \brief Returns the global event bus.
        /// \return Reference to the event bus.
        Pubsub::EventBus& eventBus() override;

        /// \brief Returns the global resource registry.
        /// \return Reference to the resource registry.
        ResourceRegistry& registry() override;

        /// \brief Requests graceful application shutdown.
        void close() override;

        /// \brief Returns true if the application is shutting down.
        /// \return True if the application is closing.
        bool isClosing() const override;

        /// \brief Returns the application name.
        /// \return Reference to the name string.
        const std::string& name() const override;

    private:
        Pubsub::EventBus m_event_bus;
        ResourceRegistry m_registry;
        WindowManager m_window_manager;
        std::thread m_main_thread;
        std::atomic<bool> m_is_closing{false};
        std::atomic<bool> m_is_ini_once{false};
        std::atomic<bool> m_is_ini_loaded{false};
        std::atomic<int> m_next_window_id{0};
        std::string m_app_name = "ImGuiX Application";
        std::vector<std::unique_ptr<Model>> m_models;
        std::vector<Model*> m_pending_models;

        /// \brief Main application loop.
        void mainLoop();

        /// \brief Checks if all windows have been closed.
        /// \return True if no windows remain open.
        bool allWindowsClosed() const;

        /// \brief Initializes all pending models.
        void initializePendingModels();
    };

} // namespace ImGuiX

#ifdef IMGUIX_HEADER_ONLY
#   include "Application.ipp"
#endif

#endif // _IMGUIX_CORE_APPLICATION_HPP_INCLUDED

