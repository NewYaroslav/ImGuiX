#pragma once
#ifndef _IMGUIX_CORE_PUBSUB_HPP_INCLUDED
#define _IMGUIX_CORE_PUBSUB_HPP_INCLUDED

/// \file pubsub.hpp
/// \brief Entry point for the publish-subscribe system.
/// 
/// Includes all necessary components for event-driven communication, 
/// allowing modules to use the entire event system with a single import.
/// 
/// This file encapsulates the core components of the Pub/Sub system:
/// Event, EventListener, EventHub, and EventMediator.

#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>
#include <functional>
#include <string>
#include <typeindex>
#include <type_traits>
#include <stdexcept>
#include <memory>

#include "pubsub/Event.hpp"
#include "pubsub/EventListener.hpp"
#include "pubsub/EventHub.hpp"
#include "pubsub/EventMediator.hpp"

#endif // _IMGUIX_CORE_PUBSUB_HPP_INCLUDED