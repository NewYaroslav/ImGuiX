#pragma once
#ifndef _IMGUIX_UTILS_EVENT_HPP_INCLUDED
#define _IMGUIX_UTILS_EVENT_HPP_INCLUDED

/// \file Event.hpp
/// \brief Defines the base Event class used in the publish-subscribe pattern.

namespace ImGuiX::utils {

    /// \class Event
    /// \brief Base class for events in the publish-subscribe system.
    ///
    /// Derived classes represent concrete events that can be published and dispatched
    /// using the EventHub. This abstract class provides a common base for all event types.
    class Event {
    public:
        /// \brief Default virtual destructor.
        virtual ~Event() = default;

        /// \brief Returns the runtime type information of the event.
        /// \return The type index representing the concrete event type.
        /// \note This method allows identifying the exact type of the event at runtime.
        ///       Useful for dispatching or logging without relying on dynamic_cast.
        virtual std::type_index type() const = 0;
        
        /// \brief Returns the name of the event type.
        /// \return A string literal representing the name of the event.
        /// \note This method is intended for debugging, logging, or serialization purposes.
        virtual const char* name() const = 0;
    };

} // namespace ImGuiX::utils

#endif // _IMGUIX_UTILS_EVENT_HPP_INCLUDED