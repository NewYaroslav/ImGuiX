#pragma once
#ifndef _IMGUIX_PUBSUB_SYNC_NOTIFIER_HPP_INCLUDED
#define _IMGUIX_PUBSUB_SYNC_NOTIFIER_HPP_INCLUDED

/// \file SyncNotifier.hpp
/// \brief Wrapper providing synchronous event dispatch without async queue access.

#include "EventBus.hpp"

namespace ImGuiX::Pubsub {

    /// \class SyncNotifier
    /// \brief Restricted interface for synchronous event notifications.
    /// \details Instances are created by Application and passed to Model::process().
    /// Direct creation is disallowed to prevent misuse.
    class SyncNotifier {
    public:
        SyncNotifier(const SyncNotifier &) = delete;            ///< Deleted copy constructor.
        SyncNotifier &operator=(const SyncNotifier &) = delete; ///< Deleted copy assignment.
        SyncNotifier(SyncNotifier &&) = delete;                 ///< Deleted move constructor.
        SyncNotifier &operator=(SyncNotifier &&) = delete;      ///< Deleted move assignment.

        /// \brief Notifies subscribers of an event by raw pointer.
        /// \param event Raw pointer to the event.
        void notify(const Event *const event) const { m_bus.notify(event); }

        /// \brief Notifies subscribers of an event by reference.
        /// \param event Reference to the event.
        void notify(const Event &event) const { m_bus.notify(event); }
        
        /// \brief Constructs notifier bound to a specific EventBus.
        /// \param bus Reference to the event bus.
        explicit SyncNotifier(EventBus &bus) : m_bus(bus) {}

    private:

        EventBus &m_bus; ///< Underlying event bus.
    };

} // namespace ImGuiX::Pubsub

#endif // _IMGUIX_PUBSUB_SYNC_NOTIFIER_HPP_INCLUDED
