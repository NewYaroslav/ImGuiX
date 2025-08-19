#pragma once
#ifndef _IMGUIX_PUBSUB_AWAITERS_HPP_INCLUDED
#define _IMGUIX_PUBSUB_AWAITERS_HPP_INCLUDED

/// \file awaiters.hpp
/// \brief Interfaces for cancelable event awaiters.

namespace ImGuiX::Pubsub {

    /// \brief Minimal awaiter interface for mediator bookkeeping.
    struct IAwaiter {
        virtual void cancel() noexcept = 0;
        virtual bool isActive() const noexcept = 0;
        virtual ~IAwaiter() = default;
    };

    /// \brief Extended awaiter interface with timeout polling.
    struct IAwaiterEx : public IAwaiter {
        virtual void pollTimeout() noexcept = 0;
        ~IAwaiterEx() override = default;
    };

} // namespace ImGuiX::Pubsub

#endif // _IMGUIX_PUBSUB_AWAITERS_HPP_INCLUDED
