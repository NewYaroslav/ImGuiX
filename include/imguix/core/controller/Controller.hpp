#pragma once
#ifndef _IMGUIX_CORE_ICONTROLLER_HPP_INCLUDED
#define _IMGUIX_CORE_ICONTROLLER_HPP_INCLUDED

/// \file Controller.hpp
/// \brief Base class for window-attached logic/rendering controllers.

namespace ImGuiX {

    /// \brief Base class for controllers that attach to a window.
    ///
    /// Provides access to window-level context, including event bus and resources.
    /// Override `drawContent()` and `drawUi()` to render content and interface.
    class Controller : public Pubsub::EventMediator {
    public:
        /// \brief Constructs a controller bound to a window.
        /// \param window Reference to associated window control.
        explicit Controller(WindowControl& window)
            : EventMediator(window.eventBus()), m_window(window) {}
            
        Controller(const Controller&) = delete;
        Controller& operator=(const Controller&) = delete;

        Controller(Controller&&) = delete;
        Controller& operator=(Controller&&) = delete;

        virtual ~Controller() = default;

        /// \brief Renders frame content (background, world, etc.).
        virtual void drawContent() = 0;

        /// \brief Renders UI overlay (widgets, HUDs, debug).
        virtual void drawUi() = 0;

        /// \brief Access to the global event bus via window.
        Pubsub::EventBus& eventBus() {
            return m_window.eventBus();
        }

        /// \brief Access to the global resource registry via window.
        ResourceRegistry& registry() {
            return m_window.registry();
        }
        
        /// \brief Returns reference to the associated window control.
        WindowControl& window() {
            return m_window;
        }

    protected:
        WindowControl& m_window;
    };

} // namespace ImGuiX

#endif // _IMGUIX_CORE_ICONTROLLER_HPP_INCLUDED