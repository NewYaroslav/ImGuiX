#pragma once
#ifndef _IMGUIX_CORE_ICONTROLLER_HPP_INCLUDED
#define _IMGUIX_CORE_ICONTROLLER_HPP_INCLUDED

/// \file Controller.hpp
/// \brief Base class for window-attached logic/rendering controllers.

namespace ImGuiX {

    class WindowInterface;

    /// \brief Base class for controllers that attach to a window.
    /// \note Provides access to window-level context, including event bus and resources.
    /// \note Override `drawContent()` and `drawUi()` to render content and interface.
    class Controller : public Pubsub::EventMediator {
    public:
        /// \brief Constructs a controller bound to a window.
        /// \param window Reference to associated window control.
        explicit Controller(WindowInterface& window)
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

        /// \brief Access to the global options store via window.
        OptionsStore::Control& options() {
            return m_window.options();
        }

        /// \brief Read-only access to the global options store.
        const OptionsStore::View& options() const {
            return static_cast<const WindowInterface&>(m_window).options();
        }

        /// \brief Returns reference to the associated window control.
        WindowInterface& window() {
            return m_window;
        }
        
        /// \brief Get language store.
        /// \return Language store.
        const ImGuiX::I18N::LangStore& langStore() const {
            return m_window.langStore();
        }

        /// \brief Access the theme manager.
        /// \return Theme manager instance.
        Themes::ThemeManager& themeManager() {
            return m_window.themeManager();
        }

    protected:
        WindowInterface& m_window; ///< Controlled window instance.
    };

} // namespace ImGuiX

#endif // _IMGUIX_CORE_ICONTROLLER_HPP_INCLUDED
