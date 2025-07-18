#pragma once
#ifndef _IMGUIX_CORE_WINDOW_MANAGER_HPP_INCLUDED
#define _IMGUIX_CORE_WINDOW_MANAGER_HPP_INCLUDED

/// \file WindowManager.hpp
/// \brief WindowManager controls and orchestrates all active windows.

namespace ImGuiX {

    /// \brief Manages all windows in the application.
    ///
    /// Delegates lifecycle operations to each registered window instance,
    /// and provides an interface for ticking, drawing, and shutdown control.
    class WindowManager : public Pubsub::EventMediator {
    public:
        /// \brief Constructs the window manager with access to the application.
        /// \param app Reference to application control interface.
        explicit WindowManager(ApplicationControl& app);

        /// \brief Destructor. Unsubscribes from all events.
        virtual ~WindowManager() {
                        unsubscribeAll();
                }

        /// \brief Handles events posted to the global bus.
        void onEvent(const Pubsub::Event* const event) override;

        /// \brief Adds a new window to the manager.
        void addWindow(std::unique_ptr<WindowInstance> window);

        /// \brief Moves pending windows into the active list.
        void flushPending();

        /// \brief Calls onInit() on newly added windows.
        void initializePending();

        /// \brief Removes windows that are no longer open.
        void removeClosed();

        /// \brief Closes all managed windows.
        void closeAll();

        /// \brief Forwards handle_events to all windows.
        void handleEvents();

        /// \brief Forwards tick to all windows.
        void tickAll();

        /// \brief Forwards draw_ui to all windows.
        void drawUiAll();

        /// \brief Forwards draw_content to all windows.
        void drawContentAll();

        /// \brief Forwards present to all windows.
        void presentAll();
        
        /// \brief Returns the number of managed windows.
        /// \return Number of windows.
        std::size_t windowCount() const;

        /// \brief Checks whether all windows have been closed.
        /// \return true if no window remains open.
        bool allWindowsClosed() const;

    protected:
        std::vector<std::unique_ptr<WindowInstance>> m_windows;      ///< Managed windows.
        std::vector<std::unique_ptr<WindowInstance>> m_pending_add; ///< Newly created windows waiting to be added.
        std::vector<WindowInstance*> m_pending_init;                 ///< Windows pending initialization.
        ApplicationControl& m_application;                           ///< Reference to the owning application.

        /// \brief Shortcut to the application resource registry.
        /// \return Reference to the ResourceRegistry owned by the application.
        ResourceRegistry& registry();
    };

} // namespace ImGuiX

#ifdef IMGUIX_HEADER_ONLY
#   include "WindowManager.ipp"
#endif

#endif // _IMGUIX_CORE_WINDOW_MANAGER_HPP_INCLUDED

