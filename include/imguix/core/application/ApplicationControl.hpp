#pragma once
#ifndef _IMGUIX_CORE_APPLICATION_CONTROL_HPP_INCLUDED
#define _IMGUIX_CORE_APPLICATION_CONTROL_HPP_INCLUDED

/// \file ApplicationControl.hpp
/// \brief Interface for application-level control and global services.

namespace ImGuiX {

    /// \brief Interface for application-level control and global services.
    ///
    /// This interface allows window instances, controllers, and components
    /// to interact with core application functionality without tight coupling.
    ///
    /// It provides access to:
    /// - Closing application
    /// - Application name
    /// - Global event bus
    /// - Shared resource registry
    class ApplicationControl {
    public:
        virtual ~ApplicationControl() = default;

        /// \brief Requests the application to close gracefully.
        virtual void close() = 0;

        /// \brief Checks whether the application is shutting down.
        /// \return true if the application is in the process of closing.
        virtual bool isClosing() const = 0;

        /// \brief Returns the name of the application, if defined.
        /// \return Constant reference to the application name.
        virtual const std::string& name() const = 0;

        /// \brief Provides access to the global event bus.
        /// \return Reference to the event bus instance.
        virtual Pubsub::EventBus& eventBus() = 0;

        /// \brief Provides access to the global resource registry.
        /// \return Reference to the resource registry.
        virtual ResourceRegistry& registry() = 0;
    };

} // namespace ImGuiX

#endif // _IMGUIX_CORE_APPLICATION_CONTROL_HPP_INCLUDED