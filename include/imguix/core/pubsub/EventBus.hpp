#pragma once
#ifndef _IMGUIX_PUBSUB_EVENT_BUS_HPP_INCLUDED
#define _IMGUIX_PUBSUB_EVENT_BUS_HPP_INCLUDED

/// \file EventBus.hpp
/// \brief Contains the EventBus class for event-based communication between modules.

namespace ImGuiX::Pubsub {

    /// \class EventBus
    /// \brief Manages subscriptions and notifications for event-based communication.
    class EventBus {
    public:
        using callback_t = std::function<void(const Event* const)>;

        struct CallbackRecord {
            EventListener* owner;
            callback_t callback;
        };

        using callback_list_t = std::vector<CallbackRecord>;
        using listener_list_t = std::vector<class EventListener*>;

        /// \brief Subscribes to an event type with a custom callback function taking a concrete event reference.
        /// \tparam EventType Type of the event to subscribe to.
        /// \param owner Object that owns the subscription, used for later unsubscription.
        /// \param callback Callback function accepting a const reference to the event.
        template <typename EventType>
        void subscribe(EventListener* owner, std::function<void(const EventType&)> callback);

        /// \brief Subscribes to an event type using a generic callback function taking a base event pointer.
        /// \tparam EventType Type of the event to subscribe to.
        /// \param owner Object that owns the subscription, used for later unsubscription.
        /// \param callback Callback function accepting a const pointer to the base event.
        template <typename EventType>
        void subscribe(EventListener* owner, std::function<void(const Event* const)> callback);

        /// \brief Subscribes using an EventListener-derived object.
        /// \tparam EventType Type of the event the listener subscribes to.
        /// \param listener EventListener object to receive event notifications.
        template <typename EventType>
        void subscribe(EventListener* listener);
        
        /// \brief Unsubscribes all subscriptions (callbacks and listener entries)
        ///        associated with the given owner for the specified event type.
        /// \tparam EventType Type of the event to unsubscribe from.
        /// \param owner Pointer to the listener object that owns the subscription.
        template <typename EventType>
        void unsubscribe(EventListener* owner);
        
        /// \brief Unsubscribes all subscriptions of a given listener across all event types.
        /// \param owner Pointer to the listener to unsubscribe completely.
        void unsubscribeAll(EventListener* owner);

        /// \brief Notifies all subscribers of an event by raw pointer.
        /// \param event Raw pointer to the event to notify subscribers of.
        void notify(const Event* const event) const;

        /// \brief Notifies subscribers of an event by reference.
        /// \param event Reference to the event to notify subscribers of.
        void notify(const Event& event) const;

        /// \brief Queues an event for asynchronous processing.
        /// \param event Unique pointer to the event.
        void notifyAsync(std::unique_ptr<Event> event);

        /// \brief Processes queued events.
        /// Should be called from the main thread to process events safely.
        void process();

    private:
        std::unordered_map<std::type_index, callback_list_t> m_event_callbacks; ///< Event type -> callbacks
        std::unordered_map<std::type_index, listener_list_t> m_event_listeners; ///< Event type -> listeners

        mutable std::mutex m_queue_mutex; ///< Mutex for thread-safe queue operations
        mutable std::mutex m_subscriptions_mutex;
        std::queue<std::unique_ptr<Event>> m_event_queue; ///< Queue for asynchronous event processing
    };

} // namespace ImGuiX::Pubsub

#if defined(IMGUIX_HEADER_ONLY)
#include "EventBus.ipp"
#endif

#endif // _IMGUIX_PUBSUB_EVENT_BUS_HPP_INCLUDED
