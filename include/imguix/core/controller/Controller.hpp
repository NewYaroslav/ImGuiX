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

        /// \brief Optional initialization callback invoked once.
        virtual void onInit() {}

        /// \brief Renders frame content (background, world, etc.).
        virtual void drawContent() = 0;

        /// \brief Renders UI overlay (widgets, HUDs, debug).
        virtual void drawUi() = 0;

        /// \brief Access the global event bus.
        /// \return Event bus.
        Pubsub::EventBus& eventBus() {
            return m_window.eventBus();
        }

        /// \brief Access the global resource registry.
        /// \return Resource registry.
        ResourceRegistry& registry() {
            return m_window.registry();
        }

        /// \brief Access the global options store.
        /// \return Options store.
        OptionsStore::Control& options() {
            return m_window.options();
        }

        /// \brief Read-only access to the global options store.
        /// \return Options store view.
        const OptionsStore::View& options() const {
            return static_cast<const WindowInterface&>(m_window).options();
        }

        /// \brief Access associated window control.
        /// \return Window control.
        WindowInterface& window() {
            return m_window;
        }
        
        /// \brief Access the notification manager.
        /// \return Notification manager instance.
        ImGuiX::Notify::NotificationManager& notifications() {
            return m_window.notifications();
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
        
        /// \brief Set active theme identifier.
        /// \param id Identifier of registered theme.
        void setTheme(std::string id) { 
            themeManager().setTheme(std::move(id)); 
        }

    protected:
        WindowInterface& m_window; ///< Controlled window instance.
    };

} // namespace ImGuiX

#endif // _IMGUIX_CORE_ICONTROLLER_HPP_INCLUDED
