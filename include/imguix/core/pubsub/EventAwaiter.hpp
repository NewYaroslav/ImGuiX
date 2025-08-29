#pragma once
#ifndef _IMGUIX_PUBSUB_EVENT_AWAITER_HPP_INCLUDED
#define _IMGUIX_PUBSUB_EVENT_AWAITER_HPP_INCLUDED

/// \file EventAwaiter.hpp
/// \brief RAII helper to wait for a single event that matches a predicate and then auto-unsubscribe.
/// \ingroup Core

#include <chrono>

#include "cancellation.hpp"
#include "awaiters.hpp"
#include "EventBus.hpp"

namespace ImGuiX::Pubsub {

    /// \brief Awaiter configuration options.
    struct AwaitOptions {
        std::chrono::steady_clock::duration timeout{}; ///< Zero means no timeout.
        CancellationToken token{};                     ///< Empty means no external cancellation.
        bool single_shot{true};                        ///< True to auto-cancel after first match.
        std::function<void()> on_timeout{};            ///< Called when timeout expires.
    };

    /// \class EventAwaiter
    /// \brief Helper listener that waits for events matching a predicate.
    /// \details Holds a shared_ptr to itself until cancelled or, if single-shot, after the first match.
    /// \tparam EventType Event type to await.
    template <typename EventType>
    class EventAwaiter : public EventListener,
                         public IAwaiterEx,
                         public std::enable_shared_from_this<EventAwaiter<EventType>> {
        static_assert(std::is_base_of<Event, EventType>::value,
                      u8"EventType must derive from ImGuiX::Pubsub::Event");
    public:
        /// \brief Predicate type to filter events.
        using Predicate = std::function<bool(const EventType&)>;
        /// \brief Callback invoked when a matching event is received.
        using Callback  = std::function<void(const EventType&)>;

        /// \brief Create and subscribe a new awaiter.
        /// \param bus Event bus to subscribe on.
        /// \param predicate Predicate to filter events. If empty, all events match.
        /// \param on_match Callback invoked when a matching event is received.
        /// \param opt Await options controlling timeout/cancellation and single-shot behavior.
        /// \return Shared pointer keeping the awaiter alive.
        [[nodiscard]] static std::shared_ptr<EventAwaiter> create(
                EventBus& bus,
                Predicate predicate,
                Callback on_match,
                AwaitOptions opt = {}) {
            auto self = std::shared_ptr<EventAwaiter>(new EventAwaiter(
                bus, std::move(predicate), std::move(on_match), std::move(opt)
            ));
            self->subscribeInternal();
            return self;
        }

        /// \copydoc IAwaiter::isActive
        bool isActive() const noexcept override {
            return !m_cancelled.load(std::memory_order_relaxed);
        }

        /// \copydoc IAwaiter::cancel
        /// \note Idempotent.
        void cancel() noexcept override;

        /// \brief Destroy awaiter and cancel subscription.
        ~EventAwaiter() override { cancel(); }

        /// \copydoc EventListener::onEvent
        void onEvent(const Event* const) override {}

    private:
        EventAwaiter(EventBus& bus,
                     Predicate predicate,
                     Callback on_match,
                     AwaitOptions opt)
            : m_bus(bus),
              m_predicate(std::move(predicate)),
              m_on_match(std::move(on_match)),
              m_opt(std::move(opt)) {
            m_on_timeout = std::move(m_opt.on_timeout);
            if (m_opt.timeout.count() > 0) {
                m_has_deadline = true;
                m_deadline = std::chrono::steady_clock::now() + m_opt.timeout;
            }
        }

        void subscribeInternal();

        void handleEvent(const EventType& ev) {
            if (m_cancelled.load(std::memory_order_relaxed)) return;
            if (m_opt.token && m_opt.token.isCancelled()) { cancel(); return; }

            // Stabilize lifetime during callback
            auto hold = this->shared_from_this();

            bool matched = true;
            if (m_predicate) matched = m_predicate(ev);
            if (!matched) return;

            // For single-shot: unsubscribe BEFORE user callback to avoid reentrancy surprises
            if (m_opt.single_shot) cancel();

            if (m_on_match) m_on_match(ev);
        }

        /// \copydoc IAwaiterEx::pollTimeout
        void pollTimeout() noexcept override {
            if (!isActive()) return;
            if (m_opt.token && m_opt.token.isCancelled()) {
                auto hold = this->shared_from_this();
                cancel();
                return;
            }
            if (m_has_deadline && std::chrono::steady_clock::now() >= m_deadline) {
                auto hold = this->shared_from_this();
                cancel();
                if (m_on_timeout) m_on_timeout();
            }
        }

    private:
        EventBus&  m_bus;
        Predicate  m_predicate;
        Callback   m_on_match;
        std::function<void()> m_on_timeout; ///< Fired when timeout elapses.
        AwaitOptions m_opt{};
        bool m_has_deadline{false};
        std::chrono::steady_clock::time_point m_deadline{};
        std::atomic<bool> m_cancelled{false};
        std::shared_ptr<EventAwaiter> m_retain_self; ///< Keeps this object alive until cancellation.
    };

    template <typename EventType>
    inline void EventAwaiter<EventType>::cancel() noexcept {
        bool expected = false;
        if (!m_cancelled.compare_exchange_strong(expected, true)) return;
        m_bus.template unsubscribe<EventType>(this);
        m_retain_self.reset();
    }

    template <typename EventType>
    inline void EventAwaiter<EventType>::subscribeInternal() {
        if (m_opt.single_shot) m_retain_self = this->shared_from_this(); // keep until first hit
        auto weak_self = this->weak_from_this();
        m_bus.subscribe<EventType>(this, [weak_self](const EventType& ev){
            if (auto self = weak_self.lock()) self->handleEvent(ev);
        });
    }

} // namespace ImGuiX::Pubsub

#endif // _IMGUIX_PUBSUB_EVENT_AWAITER_HPP_INCLUDED
