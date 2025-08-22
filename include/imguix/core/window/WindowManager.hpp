#pragma once
#ifndef _IMGUIX_CORE_WINDOW_MANAGER_HPP_INCLUDED
#define _IMGUIX_CORE_WINDOW_MANAGER_HPP_INCLUDED

/// \file WindowManager.hpp
/// \brief WindowManager controls and orchestrates all active windows.

namespace ImGuiX {

    /// \brief Manage all windows in the application.
    /// \note Delegates lifecycle operations to each registered window instance.
    /// \note Provides interfaces for ticking, drawing, and shutdown control.
    class WindowManager : public Pubsub::EventMediator {
    public:
        /// \brief Construct window manager with access to the application.
        /// \param app Reference to application context interface.
        explicit WindowManager(ApplicationContext& app);

        /// \brief Destructor. Unsubscribe from all events.
        virtual ~WindowManager() = default;

        /// \brief Handle events posted to the global bus.
        /// \param event Event pointer.
        void onEvent(const Pubsub::Event* const event) override;

        /// \brief Add new window to the manager.
        /// \param window Window instance.
        void addWindow(std::unique_ptr<WindowInstance> window);

        /// \brief Prepare window state before each frame.
        /// \note Moves pending windows, initializes them, and removes closed windows.
        void prepareFrame();

        /// \brief Close all managed windows.
        void closeAll();

        /// \brief Initialize ImGui ini handling for all windows.
        void initIniAll();

        /// \brief Execute full frame processing sequence.
        void processFrame();

        /// \brief Get number of managed windows.
        /// \return Number of windows.
        std::size_t windowCount() const;

        /// \brief Check whether all windows have been closed.
        /// \return True if no window remains open.
        bool allWindowsClosed() const;
        
        /// \brief Perform backend-specific shutdown tasks.
        void shutdown();

    private:
        /// \brief Forward handle_events to all windows.
        void handleEvents();

        /// \brief Forward tick to all windows.
        void tickAll();

        /// \brief Forward draw_ui to all windows.
        void drawUiAll();

        /// \brief Forward draw_content to all windows.
        void drawContentAll();

        /// \brief Forward present to all windows.
        void presentAll();

        /// \brief Load ImGui settings for all windows.
        void loadIniAll();

        /// \brief Save ImGui settings for all windows immediately.
        void saveIniNowAll();

        /// \brief Periodically save ImGui settings for all windows.
        void saveIniAll();

        /// \brief Move pending windows into the active list.
        void flushPending();

        /// \brief Call onInit() on newly added windows.
        void initializePending();

        /// \brief Remove windows that are no longer open.
        void removeClosed();

    protected:
        std::vector<std::unique_ptr<WindowInstance>> m_windows;      ///< Managed windows.
        std::vector<std::unique_ptr<WindowInstance>> m_pending_add;  ///< Newly created windows waiting to be added.
        std::vector<WindowInstance*> m_pending_init;                 ///< Windows pending initialization.
        ApplicationContext&          m_application;                  ///< Reference to the owning application.
        std::deque<Events::LangChangeEvent> m_lang_events;           ///< Queued language change events.
        int                          m_ini_save_frame_counter{0};    ///< Frame counter for ini saving.
        static constexpr int         m_ini_save_interval{300};       ///< Frames between ini saves.

        /// \brief Shortcut to the application resource registry.
        /// \return Reference to the ResourceRegistry owned by the application.
        ResourceRegistry& registry();

        /// \brief Process queued language change events.
        void processLanguageEvents();

        /// \brief Find window by identifier.
        /// \param id Window identifier.
        /// \return Window pointer or nullptr.
        WindowInstance* findWindowById(int id) noexcept;

        /// \brief Find window by identifier.
        /// \param id Window identifier.
        /// \return Window pointer or nullptr.
        const WindowInstance* findWindowById(int id) const noexcept;
    };

} // namespace ImGuiX

#ifdef IMGUIX_HEADER_ONLY
#   include "WindowManager.ipp"
#endif

#endif // _IMGUIX_CORE_WINDOW_MANAGER_HPP_INCLUDED

