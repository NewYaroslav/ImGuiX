#pragma once
#ifndef _IMGUIX_PUBSUB_EVENT_MEDIATOR_HPP_INCLUDED
#define _IMGUIX_PUBSUB_EVENT_MEDIATOR_HPP_INCLUDED

/// \file EventMediator.hpp
/// \brief Defines the EventMediator class for managing subscriptions and notifications through EventBus.
/// \ingroup Core

namespace ImGuiX::Pubsub {

    /// \class EventMediator
    /// \brief Facilitates event subscriptions and notifications through an associated EventBus.
    /// \details Provides a unified interface for modules to subscribe to, notify, and asynchronously post events.
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

        /// \brief Destroy mediator and cancel subscriptions.
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
        /// \tparam EventType Event type to await.
        /// \tparam Pred Predicate callable type.
        /// \tparam Cb Callback callable type.
        /// \param pred Predicate to filter events.
        /// \param cb Callback invoked on match.
        /// \param opt Await options controlling timeout and cancellation.
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
        /// \tparam EventType Event type to await.
        /// \tparam Cb Callback callable type.
        /// \param cb Callback invoked on match.
        /// \param opt Await options controlling timeout and cancellation.
        template <typename EventType, typename Cb>
        void awaitOnce(Cb&& cb, AwaitOptions opt = {}) {
            awaitOnce<EventType>(
                [](const EventType&) { return true; },
                std::forward<Cb>(cb),
                opt
            );
        }

        /// \brief Await once with timeout.
        /// \tparam EventType Event type to await.
        /// \tparam Pred Predicate callable type.
        /// \tparam Cb Callback callable type.
        /// \param pred Predicate to filter events.
        /// \param cb Callback invoked on match.
        /// \param timeout Maximum wait duration.
        /// \param on_timeout Callback invoked on timeout.
        template <typename EventType, typename Pred, typename Cb>
        void awaitOnce(Pred&& pred, Cb&& cb, std::chrono::milliseconds timeout,
                       std::function<void()> on_timeout = {}) {
            AwaitOptions opt{};
            opt.timeout = timeout;
            opt.on_timeout = std::move(on_timeout);
            awaitOnce<EventType>(std::forward<Pred>(pred), std::forward<Cb>(cb), std::move(opt));
        }

        /// \brief Await once with timeout (no predicate).
        /// \tparam EventType Event type to await.
        /// \tparam Cb Callback callable type.
        /// \param cb Callback invoked on match.
        /// \param timeout Maximum wait duration.
        /// \param on_timeout Callback invoked on timeout.
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

        // --- Cached latest events for polling in render loops ---

        /// \brief Register caching of the latest EventType under a string identifier.
        /// \tparam EventType Event type to cache.
        /// \param id Identifier for the cached event.
        /// \param pred Optional predicate to filter events.
        /// \note Re-registration with the same id is ignored.
        template <typename EventType, typename Pred = std::function<bool(const EventType&)>>
        void registerCachedEvent(const std::string& id,
                                 Pred&& pred = [](const EventType&) { return true; }) {
            auto type = std::type_index(typeid(EventType));
            std::unique_ptr<CachedSlot> slot;
            {
                std::lock_guard<std::mutex> lk(m_cache_mutex);
                if (m_cached_events.count(id)) return;

                slot = std::make_unique<CachedSlot>();
                slot->type = type;

                auto pred_fn = std::function<bool(const EventType&)>(std::forward<Pred>(pred));
                slot->predicate = [pred_fn](const Event* e) {
                    return pred_fn(*static_cast<const EventType*>(e));
                };

                // Вставим заранее, чтобы lifetime был гарантирован до subscribe()
                m_cached_events.emplace(id, std::move(slot));
            }
            {
                std::lock_guard<std::mutex> lk(m_cache_mutex);
                auto& sp = m_cached_events.at(id);
                m_event_bus->subscribe<EventType>(sp.get());
                sp->unsubscriber = [bus = m_event_bus, listener = sp.get()]() {
                    bus->unsubscribe<EventType>(listener);
                };
            }
        }

        /// \brief Unregister cached event by id and remove stored copy.
        /// \param id Identifier used during registration.
        void unregisterCachedEvent(const std::string& id) {
            std::function<void()> unsub;
            {
                std::lock_guard<std::mutex> lk(m_cache_mutex);
                auto it = m_cached_events.find(id);
                if (it == m_cached_events.end()) return;

                if (it->second && it->second->unsubscriber) {
                    unsub = std::move(it->second->unsubscriber);
                }
                auto ptr = std::move(it->second);
                m_cached_events.erase(it);
            }
            if (unsub) unsub();
        }

        /// \brief Get the latest cached EventType for the given id.
        /// \tparam EventType Expected event type.
        /// \param id Identifier used during registration.
        /// \return std::optional with the latest event or std::nullopt if none.
        template <typename EventType>
        std::optional<EventType> getCachedEvent(const std::string& id) {
            std::lock_guard<std::mutex> lk(m_cache_mutex);
            auto it = m_cached_events.find(id);
            if (it == m_cached_events.end()) return std::nullopt;
            if (!it->second) return std::nullopt;
            if (it->second->type != std::type_index(typeid(EventType))) return std::nullopt;
            if (!it->second->event) return std::nullopt;
            return *static_cast<const EventType*>(it->second->event.get());
        }

        /// \brief Multi-shot awaiter; returns token to cancel manually.
        /// \tparam EventType Event type to await.
        /// \tparam Pred Predicate callable type.
        /// \tparam Cb Callback callable type.
        /// \param pred Predicate to filter events.
        /// \param cb Callback invoked on each match.
        /// \param opt Await options controlling timeout and cancellation.
        /// \return Token to cancel awaiter.
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

        /// \brief Multi-shot awaiter without predicate.
        /// \tparam EventType Event type to await.
        /// \tparam Cb Callback callable type.
        /// \param cb Callback invoked on each match.
        /// \param opt Await options controlling timeout and cancellation.
        /// \return Token to cancel awaiter.
        template <typename EventType, typename Cb>
        std::shared_ptr<IAwaiter> awaitEach(Cb&& cb, AwaitOptions opt = {}) {
            return awaitEach<EventType>(
                [](const EventType&) { return true; },
                std::forward<Cb>(cb),
                std::move(opt)
            );
        }

        /// \brief Multi-shot awaiter with timeout.
        /// \tparam EventType Event type to await.
        /// \tparam Pred Predicate callable type.
        /// \tparam Cb Callback callable type.
        /// \param pred Predicate to filter events.
        /// \param cb Callback invoked on each match.
        /// \param timeout Maximum wait duration.
        /// \param on_timeout Callback invoked on timeout.
        /// \return Token to cancel awaiter.
        template <typename EventType, typename Pred, typename Cb>
        std::shared_ptr<IAwaiter> awaitEach(Pred&& pred, Cb&& cb,
                                            std::chrono::milliseconds timeout,
                                            std::function<void()> on_timeout = {}) {
            AwaitOptions opt{};
            opt.timeout = timeout;
            opt.on_timeout = std::move(on_timeout);
            return awaitEach<EventType>(std::forward<Pred>(pred), std::forward<Cb>(cb), std::move(opt));
        }

        /// \brief Multi-shot awaiter with timeout (no predicate).
        /// \tparam EventType Event type to await.
        /// \tparam Cb Callback callable type.
        /// \param cb Callback invoked on each match.
        /// \param timeout Maximum wait duration.
        /// \param on_timeout Callback invoked on timeout.
        /// \return Token to cancel awaiter.
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

        /// \copydoc EventListener::onEvent
        void onEvent(const Event* const) override {}

    private:
        EventBus* m_event_bus{nullptr}; ///< Associated EventBus instance.
        std::mutex m_mutex;
        std::vector<std::weak_ptr<IAwaiter>> m_awaiters; ///< Weak list of active awaiters

        struct CachedSlot : public EventListener {
            std::type_index type{typeid(void)};
            std::function<bool(const Event* const)> predicate;
            std::unique_ptr<Event> event;
            std::function<void()> unsubscriber;

            // Конкретный callback подписки
            std::function<void(const Event*)> handler;

            void onEvent(const Event* const e) override {
                if (predicate(e)) {
                    event = cloneEvent(e);
                }
            }

            std::unique_ptr<Event> cloneEvent(const Event* const e) {
                return e ? e->clone() : nullptr;
            }
        };

        std::unordered_map<std::string, std::unique_ptr<CachedSlot>> m_cached_events;
        std::mutex m_cache_mutex;

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

        template <typename EventType>
        void handleCachedEvent(const EventType& e) {
            std::lock_guard<std::mutex> lk(m_cache_mutex);
            for (auto& [id, slot] : m_cached_events) {
                if (slot.type != std::type_index(typeid(EventType))) continue;
                if (slot.predicate(&e)) {
                    slot.event = std::make_unique<EventType>(e);
                }
            }
        }
    };

} // namespace ImGuiX::Pubsub

#endif // _IMGUIX_PUBSUB_EVENT_MEDIATOR_HPP_INCLUDED
