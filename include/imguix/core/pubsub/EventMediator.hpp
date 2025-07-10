#pragma once
#ifndef _IMGUIX_PUBSUB_EVENT_MEDIATOR_HPP_INCLUDED
#define _IMGUIX_PUBSUB_EVENT_MEDIATOR_HPP_INCLUDED

/// \file EventMediator.hpp
/// \brief Defines the EventMediator class for managing subscriptions and notifications through EventHub.

namespace ImGuiX::Pubsub {

    /// \class EventMediator
    /// \brief Facilitates event subscriptions and notifications through an associated EventHub.
    ///
    /// Provides a unified interface for modules to subscribe to, notify, and asynchronously post events.
    class EventMediator : public EventListener {
    public:
        /// \brief Constructs EventMediator with a raw pointer to an EventHub instance.
        /// \param hub Pointer to the EventHub instance.
        explicit EventMediator(EventHub* hub) : m_event_hub(hub) {}

        /// \brief Constructs EventMediator with a reference to an EventHub instance.
        /// \param hub Reference to the EventHub instance.
        explicit EventMediator(EventHub& hub) : m_event_hub(&hub) {}

        /// \brief Constructs EventMediator with a unique pointer to an EventHub instance.
        /// \param hub Unique pointer to the EventHub instance.
        explicit EventMediator(std::unique_ptr<EventHub>& hub) : m_event_hub(hub.get()) {}

        virtual ~EventMediator() = default;
        
        /// \brief Subscribes to an event type with a custom callback function taking a concrete event reference.
        /// \tparam EventType Type of the event to subscribe to.
        /// \param callback Callback function accepting a const reference to the event.
        template <typename EventType>
        void subscribe(std::function<void(const EventType&)> callback) {
            m_event_hub->subscribe<EventType>(this, std::move(callback));
        }

        /// \brief Subscribes to an event type with a generic callback function taking a base event pointer.
        /// \tparam EventType Type of the event to subscribe to.
        /// \param callback Callback function accepting a const pointer to the base event.
        template <typename EventType>
        void subscribe(std::function<void(const Event* const)> callback) {
            m_event_hub->subscribe<EventType>(this, std::move(callback));
        }

		/// \brief Subscribes this object as a listener to the specified event type.
		/// \tparam EventType Type of the event to subscribe to.
        template <typename EventType>
        void subscribe() {
            m_event_hub->subscribe<EventType>(this);
        }
		
		/// \brief Unsubscribes this mediator from a specific event type.
		/// \tparam EventType Type of the event to unsubscribe from.
		template <typename EventType>
		void unsubscribe() {
			m_event_hub->unsubscribe<EventType>(this);
		}

        /// \brief Notifies all subscribers of an event (shared pointer dereferenced).
		/// \param event Shared pointer to the event.
        void notify(const std::shared_ptr<Event>& event) const {
            m_event_hub->notify(event.get());
        }
        
        /// \brief Notifies all subscribers of an event (unique pointer dereferenced).
        /// \param event Unique pointer to the event.
        void notify(const std::unique_ptr<Event>& event) const {
            m_event_hub->notify(event.get());
        }

        /// \brief Notifies all subscribers of an event (raw pointer).
        /// \param event Raw pointer to the event.
        void notify(const Event* const event) const {
            m_event_hub->notify(event);
        }

        /// \brief Notifies all subscribers of an event (reference).
        /// \param event Reference to the event.
        void notify(const Event& event) const {
            m_event_hub->notify(event);
        }

        /// \brief Queues an event for asynchronous processing.
        /// \param event Unique pointer to the event.
        void notifyAsync(std::unique_ptr<Event> event) {
            m_event_hub->notifyAsync(std::move(event));
        }

    private:
        EventHub* m_event_hub; ///< Associated EventHub instance.
    };

} // namespace ImGuiX::Pubsub

#endif // _IMGUIX_PUBSUB_EVENT_MEDIATOR_HPP_INCLUDED
