#pragma once
#ifndef _IMGUIX_CORE_PUBSUB_HPP_INCLUDED
#define _IMGUIX_CORE_PUBSUB_HPP_INCLUDED

/// \file pubsub.hpp
/// \brief Entry point for the publish-subscribe system.
/// \note Includes all necessary components for event-driven communication.
/// \note Encapsulates Event, EventListener, EventBus, and EventMediator.
/// \note Canonical public entrypoint for pubsub subsystem; consumers should prefer this header.

#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <queue>
#include <string>
#include <type_traits>
#include <typeindex>
#include <stdexcept>
#include <vector>

#include "pubsub/Event.hpp"
#include "pubsub/EventListener.hpp"
#include "pubsub/awaiters.hpp"
#include "pubsub/EventBus.hpp"
#include "pubsub/cancellation.hpp"
#include "pubsub/EventAwaiter.hpp"
#include "pubsub/EventMediator.hpp"
#include "pubsub/SyncNotifier.hpp"

#endif // _IMGUIX_CORE_PUBSUB_HPP_INCLUDED
