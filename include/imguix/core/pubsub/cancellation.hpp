#pragma once
#ifndef _IMGUIX_PUBSUB_CANCELLATION_HPP_INCLUDED
#define _IMGUIX_PUBSUB_CANCELLATION_HPP_INCLUDED

#include <atomic>
#include <memory>

/// \file cancellation.hpp
/// \brief Lightweight cancellation primitives for awaiters.
/// \ingroup Core

namespace ImGuiX::Pubsub {

    /// \class CancellationToken
    /// \brief Passive handle queried by awaiters for cancellation status.
    class CancellationToken {
    public:
        /// \brief Checks whether cancellation was requested.
        bool isCancelled() const noexcept {
            return m_state && m_state->flag.load(std::memory_order_relaxed);
        }

        /// \brief Indicates whether this token is valid.
        explicit operator bool() const noexcept { return static_cast<bool>(m_state); }

    private:
        struct State { std::atomic_bool flag{false}; };
        std::shared_ptr<State> m_state;
        friend class CancellationSource;
    };

    /// \class CancellationSource
    /// \brief Owner object that can request cancellation on associated tokens.
    class CancellationSource {
    public:
        /// \brief Creates a new cancellation source with fresh state.
        CancellationSource() : m_state(std::make_shared<CancellationToken::State>()) {}

        /// \brief Returns a token linked to this source.
        CancellationToken token() const noexcept {
            CancellationToken t; t.m_state = m_state; return t;
        }

        /// \brief Signals cancellation to all linked tokens.
        void cancel() noexcept {
            if (m_state) m_state->flag.store(true, std::memory_order_relaxed);
        }

    private:
        std::shared_ptr<CancellationToken::State> m_state;
    };

} // namespace ImGuiX::Pubsub

#endif // _IMGUIX_PUBSUB_CANCELLATION_HPP_INCLUDED
