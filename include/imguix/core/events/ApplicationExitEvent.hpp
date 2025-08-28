#pragma once
#ifndef _IMGUIX_EVENTS_APPLICATION_EXIT_EVENT_HPP_INCLUDED
#define _IMGUIX_EVENTS_APPLICATION_EXIT_EVENT_HPP_INCLUDED

/// \file ApplicationExitEvent.hpp
/// \brief Emitted when the application requests a full shutdown.

namespace ImGuiX::Events {

    /// \brief Emitted when the application requests a full shutdown.
    class ApplicationExitEvent : public Pubsub::Event {
    public:
        /// \brief Construct event.
        ApplicationExitEvent() = default;

        /// \copydoc Pubsub::Event::type
        std::type_index type() const override {
            return typeid(ApplicationExitEvent);
        }

        /// \copydoc Pubsub::Event::name
        const char* name() const override {
            return u8"ApplicationExitEvent";
        }
    };

} // namespace ImGuiX::Events

#endif // _IMGUIX_EVENTS_APPLICATION_EXIT_EVENT_HPP_INCLUDED
