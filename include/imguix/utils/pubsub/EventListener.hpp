#pragma once
#ifndef _IMGUIX_UTILS_EVENT_LISTENER_HPP_INCLUDED
#define _IMGUIX_UTILS_EVENT_LISTENER_HPP_INCLUDED

/// \file EventListener.hpp
/// \brief Defines the EventListener class for receiving event notifications.

namespace ImGuiX::utils {

    /// \class EventListener
    /// \brief Abstract base class for receiving event notifications.
    ///
    /// Derived classes implement event handlers and can receive events either as shared pointers
    /// or as raw pointers, depending on the dispatch method.
    class EventListener {
    public:
        virtual ~EventListener() = default;

        /// \brief Handles an event notification received as a raw pointer.
        /// \param event Raw pointer to the received event.
        virtual void on_event(const Event* const event) = 0;
    };

} // namespace ImGuiX::utils

#endif // _IMGUIX_UTILS_EVENT_LISTENER_HPP_INCLUDED