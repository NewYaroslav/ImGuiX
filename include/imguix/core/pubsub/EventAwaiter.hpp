#pragma once
#ifndef _IMGUIX_PUBSUB_EVENT_AWAITER_HPP_INCLUDED
#define _IMGUIX_PUBSUB_EVENT_AWAITER_HPP_INCLUDED

/// \file EventAwaiter.hpp
/// \brief RAII helper to wait for a single event that matches a predicate and then auto-unsubscribe.

namespace ImGuiX::Pubsub {

    /// \brief Minimal awaiter interface for mediator bookkeeping.
    struct IAwaiter {
        virtual void cancel() noexcept = 0;
        virtual bool isActive() const noexcept = 0;
        virtual ~IAwaiter() = default;
    };

    /// \class EventAwaiter
    /// \brief Helper listener that waits for events matching a predicate.
    ///
    /// Instances manage their own lifetime by holding a shared_ptr to themselves
    /// until cancelled or, if single-shot, after the first match.
    template <typename EventType>
    class EventAwaiter : public EventListener,
                         public IAwaiter,
                         public std::enable_shared_from_this<EventAwaiter<EventType>> {
        static_assert(std::is_base_of<Event, EventType>::value,
                      "EventType must derive from ImGuiX::Pubsub::Event");
    public:
        using Predicate = std::function<bool(const EventType&)>;
        using Callback  = std::function<void(const EventType&)>;

        /// \brief Creates and subscribes a new awaiter.
        /// \param bus Event bus to subscribe on.
        /// \param pred Predicate to filter events. If empty, all events match.
        /// \param onMatch Callback invoked when a matching event is received.
        /// \param singleShot Whether the awaiter should automatically cancel after first match.
        /// \return Shared pointer keeping the awaiter alive.
        [[nodiscard]] static std::shared_ptr<EventAwaiter> create(
                EventBus& bus,
                Predicate predicate,
                Callback onMatch,
                bool singleShot = true) {
            auto self = std::shared_ptr<EventAwaiter>(new EventAwaiter(
                bus, std::move(predicate), std::move(onMatch), singleShot
            ));
            self->subscribeInternal();
            return self;
        }

        bool isActive() const noexcept override {
            return !m_cancelled.load(std::memory_order_relaxed);
        }

        /// \brief Cancels the awaiter (unsubscribe). Idempotent.
        void cancel() noexcept override {
            bool expected = false;
            if (!m_cancelled.compare_exchange_strong(expected, true)) return;
            m_bus.template unsubscribe<EventType>(this);
            m_retain_self.reset();
        }

        ~EventAwaiter() override { cancel(); }

        // EventListener hook (unused when subscribing via typed API)
        void onEvent(const Event* const) override {}

    private:
        EventAwaiter(EventBus& bus,
                     Predicate predicate,
                     Callback on_match,
                     bool single_shot)
            : m_bus(bus),
              m_predicate(std::move(predicate)),
              m_on_match(std::move(on_match)),
              m_single_shot(single_shot) {}

        void subscribeInternal() {
            if (m_single_shot) m_retain_self = this->shared_from_this(); // keep until first hit
            auto weakSelf = this->weak_from_this();
            m_bus.subscribe<EventType>(this, [weakSelf](const EventType& ev){
                if (auto self = weakSelf.lock()) self->handleEvent(ev);
            });
        }

        void handleEvent(const EventType& ev) {
            if (m_cancelled.load(std::memory_order_relaxed)) return;

            // Stabilize lifetime during callback
            auto hold = this->shared_from_this();

            bool matched = true;
            if (m_predicate) matched = m_predicate(ev);
            if (!matched) return;

            // For single-shot: unsubscribe BEFORE user callback to avoid reentrancy surprises
            if (m_single_shot) cancel();

            if (m_on_match) m_on_match(ev);
        }

    private:
        EventBus&  m_bus;
        Predicate  m_predicate;
        Callback   m_on_match;
        const bool m_single_shot{true};
        std::atomic<bool> m_cancelled{false};
        std::shared_ptr<EventAwaiter> m_retain_self; ///< Keeps this object alive until cancellation
    };

} // namespace ImGuiX::Pubsub

#endif // _IMGUIX_PUBSUB_EVENT_AWAITER_HPP_INCLUDED
