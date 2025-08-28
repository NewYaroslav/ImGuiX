#pragma once
#ifndef _IMGUIX_CORE_PUBSUB_HPP_INCLUDED
#define _IMGUIX_CORE_PUBSUB_HPP_INCLUDED

/// \file pubsub.hpp
/// \brief Entry point for the publish-subscribe system.
/// \note Includes all necessary components for event-driven communication.
/// \note Encapsulates Event, EventListener, EventBus, and EventMediator.

#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <functional>
#include <string>
#include <typeindex>
#include <type_traits>
#include <stdexcept>
#include <memory>
#include <optional>

#include "pubsub/Event.hpp"
#include "pubsub/EventListener.hpp"
#include "pubsub/awaiters.hpp"
#include "pubsub/EventBus.hpp"
#include "pubsub/cancellation.hpp"
#include "pubsub/EventAwaiter.hpp"
#include "pubsub/EventMediator.hpp"
#include "pubsub/SyncNotifier.hpp"

#endif // _IMGUIX_CORE_PUBSUB_HPP_INCLUDED
