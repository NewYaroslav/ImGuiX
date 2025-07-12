#pragma once
#ifndef _IMGUIX_CORE_WINDOW_CONTROL_HPP_INCLUDED
#define _IMGUIX_CORE_WINDOW_CONTROL_HPP_INCLUDED

/// \file WindowControl.hpp
/// \brief Interface for controlling and querying a single window instance.

#ifdef IMGUIX_USE_SFML_BACKEND
#   include <SFML/Graphics/RenderWindow.hpp>
#endif

namespace ImGuiX {

    /// \brief Interface for controlling and querying a single window instance.
    ///
    /// Provides access to:
    /// - Window identity (ID, name)
    /// - Geometry (size, visibility, active state)
    /// - Window state (open, minimized, maximized)
    /// - Global services (event bus, resource registry)
    class WindowControl {
    public:
        virtual ~WindowControl() = default;

        /// \brief Returns unique window ID.
        /// \return 
        virtual int id() const = 0;

        /// \brief Returns window name (title).
        /// \return 
        virtual const std::string& name() const = 0;

        /// \brief Returns current window width in pixels.
        /// \return 
        virtual int width() const = 0;

        /// \brief Returns current window height in pixels.
        /// \return 
        virtual int height() const = 0;

        /// \brief Sets window dimensions in pixels.
        virtual void setSize(int w, int h) = 0;

        /// \brief Closes the window.
        virtual void close() = 0;

        /// \brief Minimizes the window.
        virtual void minimize() = 0;

        /// \brief Maximizes the window.
        virtual void maximize() = 0;

        /// \brief Restores the window from minimized or maximized state.
        virtual void restore() = 0;
        
        /// \brief Returns true if window is currently maximized.
        /// \return 
        virtual bool isMaximized() const = 0;

        /// \brief Toggles between maximized and restored states.
        virtual void toggleMaximizeRestore() = 0;

        /// \brief Sets whether the window is active (focused).
        /// \return 
        virtual bool setActive(bool active) = 0;

        /// \brief Returns true if the window is currently active (focused).
        /// \return 
        virtual bool isActive() const = 0;

        /// \brief Sets whether the window is visible.
        virtual void setVisible(bool visible) = 0;

        /// \brief Returns true if the window is currently open.
        /// \return 
        virtual bool isOpen() const = 0;

        /// \brief Provides access to the global event bus.
        /// \return 
        virtual Pubsub::EventBus& eventBus() = 0;

        /// \brief Provides access to the global resource registry.
        /// \return 
        virtual ResourceRegistry& registry() = 0;
        
        /// \brief 
        /// \return 
        virtual ApplicationControl& application() = 0;

#       ifdef IMGUIX_USE_SFML_BACKEND
        virtual sf::RenderWindow& getRenderTarget() = 0;
#       endif

    };

} // namespace ImGuiX

#endif // _IMGUIX_CORE_WINDOW_CONTROL_HPP_INCLUDED