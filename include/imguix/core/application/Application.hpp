#pragma once
#ifndef _IMGUIX_CORE_APPLICATION_HPP_INCLUDED
#define _IMGUIX_CORE_APPLICATION_HPP_INCLUDED

/// \file Application.hpp
/// \brief Defines the main application class responsible for window management, event bus, and application loop.

#include <atomic>
#include <thread>

namespace ImGuiX {

    /// \brief Main application class.
    ///
    /// Controls window lifecycle, global event dispatching, and the main loop.
    class Application : public ApplicationContext {
    public:
        /// \brief Constructs the application instance.
        Application();

        /// \brief Destructor.
        ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(Application) = delete;

        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        /// \brief Starts the application loop.
        /// \param async If true, runs the main loop in a separate thread.
        void run(bool async = false);

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

    protected:
        WindowInstance& createWindowImpl(WindowFactory factory) override;
        Model& createModelImpl(ModelFactory factory) override;

    private:
        Pubsub::EventBus m_event_bus;                 ///< Global event bus.
        ResourceRegistry m_registry;                   ///< Shared resource registry.
        WindowManager m_window_manager;                ///< Manages all windows.
        std::thread m_main_thread;                     ///< Thread running the main loop when async.
        std::atomic<bool> m_is_closing{false};         ///< Indicates shutdown in progress.
        std::atomic<bool> m_is_ini_once{false};        ///< Ensures imgui ini is saved only once.
        std::atomic<bool> m_is_fs_ready{true};         ///< Set when filesystem mount completes.

        std::atomic<int> m_next_window_id{0};          ///< Incremental ID for new windows.
        std::string m_app_name = u8"ImGuiX Application"; ///< Application name string.
        std::vector<std::unique_ptr<Model>> m_models;  ///< Owned model objects.
        std::vector<Model*> m_pending_models;          ///< Models waiting for initialization.

        /// \brief Main application loop.
        void mainLoop();

        /// \brief Single iteration of the main loop.
        /// \return True to continue the main loop.
        bool loopIteration();

        /// \brief Called before entering the main loop.
        void startLoop();

        /// \brief Called after exiting the main loop.
        void endLoop();

        /// \brief Checks if all windows have been closed.
        /// \return True if no windows remain open.
        bool allWindowsClosed() const;

        /// \brief Initializes all pending models.
        void initializePendingModels();

        /// \brief Initialize persistent filesystem (no-op on native builds).
        void initFilesystem();

#ifdef __EMSCRIPTEN__
        /// \brief Called when asynchronous filesystem mounting finishes.
        static void filesystemReady(void* arg);
#endif
    };

} // namespace ImGuiX

#ifdef IMGUIX_HEADER_ONLY
#   include "Application.ipp"
#endif

#endif // _IMGUIX_CORE_APPLICATION_HPP_INCLUDED

