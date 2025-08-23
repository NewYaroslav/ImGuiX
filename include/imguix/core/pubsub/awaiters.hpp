#pragma once
#ifndef _IMGUIX_PUBSUB_AWAITERS_HPP_INCLUDED
#define _IMGUIX_PUBSUB_AWAITERS_HPP_INCLUDED

/// \file awaiters.hpp
/// \brief Interfaces for cancelable event awaiters.
/// \ingroup Core

namespace ImGuiX::Pubsub {

    /// \brief Minimal awaiter interface for mediator bookkeeping.
    struct IAwaiter {
        /// \brief Cancel the awaiter.
        virtual void cancel() noexcept = 0;

        /// \brief Check whether the awaiter is still active.
        /// \return True if active.
        virtual bool isActive() const noexcept = 0;

        virtual ~IAwaiter() = default;
    };

    /// \brief Extended awaiter interface with timeout polling.
    struct IAwaiterEx : public IAwaiter {
        /// \brief Poll for timeout or cancellation conditions.
        virtual void pollTimeout() noexcept = 0;

        ~IAwaiterEx() override = default;
    };

} // namespace ImGuiX::Pubsub

#endif // _IMGUIX_PUBSUB_AWAITERS_HPP_INCLUDED
