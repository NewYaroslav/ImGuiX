#pragma once
#ifndef _IMGUIX_EVENTS_WINDOW_CLOSED_EVENT_HPP_INCLUDED
#define _IMGUIX_EVENTS_WINDOW_CLOSED_EVENT_HPP_INCLUDED

/// \file WindowClosedEvent.hpp
/// \brief Emitted when a window is closed.

namespace ImGuiX::Events {

    /// \brief Emitted when a window is closed.
    class WindowClosedEvent : public Pubsub::Event {
    public:
        int id; ///< ID of the closed window.
        std::string window_name; ///< Name of the closed window (for debugging).

        WindowClosedEvent(int id, std::string name)
            : id(id), window_name(std::move(name)) {}

        std::type_index type() const override {
            return typeid(WindowClosedEvent);
        }

        const char* name() const override {
            return u8"WindowClosedEvent";
        }
    };

} // namespace ImGuiX::Events

#endif // _IMGUIX_EVENTS_WINDOW_CLOSED_EVENT_HPP_INCLUDED