#pragma once
#ifndef _IMGUIX_PUBSUB_EVENT_AWAITER_HPP_INCLUDED
#define _IMGUIX_PUBSUB_EVENT_AWAITER_HPP_INCLUDED

/// \file EventAwaiter.hpp
/// \brief Provides utilities for awaiting specific events with optional predicates.

namespace ImGuiX::Pubsub {

    /// \class EventAwaiter
    /// \brief Helper listener that waits for events matching a predicate.
    ///
    /// Instances manage their own lifetime by holding a shared_ptr to themselves
    /// until cancelled or, if single-shot, after the first match.
    template <typename EventType>
    class EventAwaiter : public EventListener, public std::enable_shared_from_this<EventAwaiter<EventType>> {
    public:
        using predicate_t = std::function<bool(const EventType&)>;
        using match_cb_t = std::function<void(const EventType&)>;

        /// \brief Creates and subscribes a new awaiter.
        /// \param bus Event bus to subscribe on.
        /// \param pred Predicate to filter events. If empty, all events match.
        /// \param on_match Callback invoked when a matching event is received.
        /// \param single_shot Whether the awaiter should automatically cancel after first match.
        /// \return Shared pointer keeping the awaiter alive.
        static std::shared_ptr<EventAwaiter> create(EventBus& bus,
                                                    predicate_t pred = {},
                                                    match_cb_t on_match = {},
                                                    bool single_shot = false) {
            auto ptr = std::shared_ptr<EventAwaiter>(
                new EventAwaiter(bus, std::move(pred), std::move(on_match), single_shot));
            ptr->m_self = ptr;
            bus.template subscribe<EventType>(ptr.get());
            return ptr;
        }

        ~EventAwaiter() override { cancel(); }

        /// \brief Cancels the awaiter and unsubscribes from the bus.
        void cancel() {
            if (m_cancelled) return;
            m_cancelled = true;
            m_bus.template unsubscribe<EventType>(this);
            m_self.reset();
        }

        /// \brief EventListener override invoked by the EventBus.
        void onEvent(const Event* const event) override {
            if (m_cancelled) return;
            const auto& e = *static_cast<const EventType*>(event);
            if (m_pred && !m_pred(e)) return;
            if (m_on_match) m_on_match(e);
            if (m_single_shot) cancel();
        }

    private:
        EventAwaiter(EventBus& bus, predicate_t pred, match_cb_t on_match, bool single_shot)
            : m_bus(bus),
              m_pred(std::move(pred)),
              m_on_match(std::move(on_match)),
              m_single_shot(single_shot) {}

        EventBus& m_bus;
        predicate_t m_pred;
        match_cb_t m_on_match;
        bool m_single_shot{false};
        bool m_cancelled{false};
        std::shared_ptr<EventAwaiter> m_self; ///< Keeps this object alive until cancellation
    };

    /// \brief Sugar helper: await once with self-captured lifetime; no handle needed.
    /// \details Creates an awaiter and keeps it alive by capturing its shared_ptr in the callback.
    template <typename EventType, typename Pred, typename Cb>
    inline void await_once(EventBus& bus, Pred pred, Cb cb) {
        using AW = EventAwaiter<EventType>;
        auto aw = AW::create(bus, pred, nullptr, /*single_shot=*/true);
        auto keeper = AW::create(
            bus,
            std::move(pred),
            [cb = std::move(cb), hold = aw](const EventType& e) mutable {
                cb(e);
                // `hold` goes out of scope after callback — both awaiters are single-shot
            },
            /*single_shot=*/true);
        (void)keeper;
    }

    /// \brief Await a single event without a predicate.
    template <typename EventType, typename Cb>
    inline void await_once(EventBus& bus, Cb cb) {
        await_once<EventType>(bus, [](const EventType&) { return true; }, std::move(cb));
    }

} // namespace ImGuiX::Pubsub

#endif // _IMGUIX_PUBSUB_EVENT_AWAITER_HPP_INCLUDED
