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

        virtual ~WindowManager() = default;
        
        void onEvent(const Pubsub::Event* const event) override;

        /// \brief Adds a new window to the manager.
        void addWindow(std::unique_ptr<WindowInstance> window);
        
        /// \brief Calls onInit() on all windows (after they are added).
        void initializeAll();

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
        std::vector<std::unique_ptr<WindowInstance>> m_windows;
        ApplicationControl& m_application;

        /// \brief Shortcut to the application resource registry.
        ResourceRegistry& registry();
    };

} // namespace ImGuiX

#ifdef IMGUIX_HEADER_ONLY
#   include "WindowManager.ipp"
#endif

#endif // _IMGUIX_CORE_WINDOW_MANAGER_HPP_INCLUDED