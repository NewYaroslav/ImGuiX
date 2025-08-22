#pragma once
#ifndef _IMGUIX_PUBSUB_EVENT_MEDIATOR_HPP_INCLUDED
#define _IMGUIX_PUBSUB_EVENT_MEDIATOR_HPP_INCLUDED

/// \file EventMediator.hpp
/// \brief Defines the EventMediator class for managing subscriptions and notifications through EventBus.
/// \ingroup Core

namespace ImGuiX::Pubsub {

    /// \class EventMediator
    /// \brief Facilitates event subscriptions and notifications through an associated EventBus.
    ///
    /// Provides a unified interface for modules to subscribe to, notify, and asynchronously post events.
    class EventMediator : public EventListener {
    public:
        /// \brief Constructs EventMediator with a raw pointer to an EventBus instance.
        /// \param bus Pointer to the EventBus instance.
        explicit EventMediator(EventBus* bus) : m_event_bus(bus) {}

        /// \brief Constructs EventMediator with a reference to an EventBus instance.
        /// \param bus Reference to the EventBus instance.
        explicit EventMediator(EventBus& bus) : m_event_bus(&bus) {}

        /// \brief Constructs EventMediator with a unique pointer to an EventBus instance.
        /// \param bus Unique pointer to the EventBus instance.
        explicit EventMediator(std::unique_ptr<EventBus>& bus) : m_event_bus(bus.get()) {}

        ~EventMediator() noexcept override {
            cancelAllAwaiters();
            unsubscribeAll();
        }
        
        // --- Subscriptions ---
         
        /// \brief Subscribes to an event type with a custom callback function taking a concrete event reference.
        /// \tparam EventType Type of the event to subscribe to.
        /// \param callback Callback function accepting a const reference to the event.
        template <typename EventType>
        void subscribe(std::function<void(const EventType&)> callback) {
            m_event_bus->subscribe<EventType>(this, std::move(callback));
        }

        /// \brief Subscribes to an event type with a generic callback function taking a base event pointer.
        /// \tparam EventType Type of the event to subscribe to.
        /// \param callback Callback function accepting a const pointer to the base event.
        template <typename EventType>
        void subscribe(std::function<void(const Event* const)> callback) {
            m_event_bus->subscribe<EventType>(this, std::move(callback));
        }

        /// \brief Subscribes this object as a listener to the specified event type.
        /// \tparam EventType Type of the event to subscribe to.
        template <typename EventType>
        void subscribe() {
            m_event_bus->subscribe<EventType>(this);
        }

        /// \brief Unsubscribes this mediator from a specific event type.
        /// \tparam EventType Type of the event to unsubscribe from.
        template <typename EventType>
        void unsubscribe() {
            m_event_bus->unsubscribe<EventType>(this);
        }
        
        /// \brief Unsubscribes this mediator from all event types.
        void unsubscribeAll() {
            m_event_bus->unsubscribeAll(this);
        }
        
        // --- Notify/dispatch ---

        /// \brief Notifies all subscribers of an event (shared pointer dereferenced).
        /// \param event Shared pointer to the event.
        void notify(const std::shared_ptr<Event>& event) const {
            m_event_bus->notify(event.get());
        }
        
        /// \brief Notifies all subscribers of an event (unique pointer dereferenced).
        /// \param event Unique pointer to the event.
        void notify(const std::unique_ptr<Event>& event) const {
            m_event_bus->notify(event.get());
        }

        /// \brief Notifies all subscribers of an event (raw pointer).
        /// \param event Raw pointer to the event.
        void notify(const Event* const event) const {
            m_event_bus->notify(event);
        }

        /// \brief Notifies all subscribers of an event (reference).
        /// \param event Reference to the event.
        void notify(const Event& event) const {
            m_event_bus->notify(event);
        }

        /// \brief Queues an event for asynchronous processing.
        /// \param event Unique pointer to the event.
        void notifyAsync(std::unique_ptr<Event> event) {
            m_event_bus->notifyAsync(std::move(event));
        }
        
        // --- Await helpers (optionx semantics; ImGuiX naming) ---

        /// \brief Await a single occurrence of EventType that matches predicate; auto-unsubscribe.
        template <typename EventType, typename Pred, typename Cb>
        void awaitOnce(Pred&& pred, Cb&& cb, AwaitOptions opt = {}) {
            pruneDeadAwaiters();

            using AW = EventAwaiter<EventType>;
            auto aw = AW::create(
                *m_event_bus,
                std::function<bool(const EventType&)>(std::forward<Pred>(pred)),
                std::function<void(const EventType&)>(std::forward<Cb>(cb)),
                opt
            );

            m_event_bus->registerAwaiter(std::static_pointer_cast<IAwaiterEx>(aw));

            std::lock_guard<std::mutex> lk(m_mutex);
            m_awaiters.emplace_back(aw);
        }

        /// \brief Await the first EventType (no predicate).
        template <typename EventType, typename Cb>
        void awaitOnce(Cb&& cb, AwaitOptions opt = {}) {
            awaitOnce<EventType>(
                [](const EventType&) { return true; },
                std::forward<Cb>(cb),
                opt
            );
        }

        template <typename EventType, typename Pred, typename Cb>
        void awaitOnce(Pred&& pred, Cb&& cb, std::chrono::milliseconds timeout,
                       std::function<void()> on_timeout = {}) {
            AwaitOptions opt{};
            opt.timeout = timeout;
            opt.on_timeout = std::move(on_timeout);
            awaitOnce<EventType>(std::forward<Pred>(pred), std::forward<Cb>(cb), std::move(opt));
        }

        template <typename EventType, typename Cb>
        void awaitOnce(Cb&& cb, std::chrono::milliseconds timeout,
                       std::function<void()> on_timeout = {}) {
            awaitOnce<EventType>(
                [](const EventType&) { return true; },
                std::forward<Cb>(cb),
                timeout,
                std::move(on_timeout)
            );
        }

        /// \brief Multi-shot awaiter; returns token (IAwaiter) to cancel manually.
        template <typename EventType, typename Pred, typename Cb>
        std::shared_ptr<IAwaiter> awaitEach(Pred&& pred, Cb&& cb, AwaitOptions opt = {}) {
            pruneDeadAwaiters();

            opt.single_shot = false;

            using AW = EventAwaiter<EventType>;
            auto aw = AW::create(
                *m_event_bus,
                std::function<bool(const EventType&)>(std::forward<Pred>(pred)),
                std::function<void(const EventType&)>(std::forward<Cb>(cb)),
                opt
            );

            m_event_bus->registerAwaiter(std::static_pointer_cast<IAwaiterEx>(aw));

            std::lock_guard<std::mutex> lk(m_mutex);
            m_awaiters.emplace_back(aw);
            return aw;
        }

        template <typename EventType, typename Cb>
        std::shared_ptr<IAwaiter> awaitEach(Cb&& cb, AwaitOptions opt = {}) {
            return awaitEach<EventType>(
                [](const EventType&) { return true; },
                std::forward<Cb>(cb),
                std::move(opt)
            );
        }

        template <typename EventType, typename Pred, typename Cb>
        std::shared_ptr<IAwaiter> awaitEach(Pred&& pred, Cb&& cb,
                                            std::chrono::milliseconds timeout,
                                            std::function<void()> on_timeout = {}) {
            AwaitOptions opt{};
            opt.timeout = timeout;
            opt.on_timeout = std::move(on_timeout);
            return awaitEach<EventType>(std::forward<Pred>(pred), std::forward<Cb>(cb), std::move(opt));
        }

        template <typename EventType, typename Cb>
        std::shared_ptr<IAwaiter> awaitEach(Cb&& cb, std::chrono::milliseconds timeout,
                                            std::function<void()> on_timeout = {}) {
            return awaitEach<EventType>(
                [](const EventType&) { return true; },
                std::forward<Cb>(cb),
                timeout,
                std::move(on_timeout)
            );
        }

        // Untyped hook if needed
        void onEvent(const Event* const) override {}

    private:
        EventBus* m_event_bus{nullptr}; ///< Associated EventBus instance.
        std::mutex m_mutex;
        std::vector<std::weak_ptr<IAwaiter>> m_awaiters; ///< Weak list of active awaiters
        
        void pruneDeadAwaiters() {
            std::lock_guard<std::mutex> lk(m_mutex);
            auto& v = m_awaiters;
            v.erase(std::remove_if(v.begin(), v.end(),
                [](const std::weak_ptr<IAwaiter>& w){
                    if (w.expired()) return true;
                    if (auto sp = w.lock()) return !sp->isActive();
                    return true;
                }), v.end());
        }

        void cancelAllAwaiters() {
            std::vector<std::shared_ptr<IAwaiter>> live;

            std::unique_lock<std::mutex> lock(m_mutex);
            live.reserve(m_awaiters.size());
            for (auto& w : m_awaiters) {
                if (auto sp = w.lock()) live.emplace_back(std::move(sp));
            }
            m_awaiters.clear();
            lock.unlock();

            for (auto& sp : live) sp->cancel();
        }
    };

} // namespace ImGuiX::Pubsub

#endif // _IMGUIX_PUBSUB_EVENT_MEDIATOR_HPP_INCLUDED
