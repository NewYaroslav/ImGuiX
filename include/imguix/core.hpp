#pragma once
#ifndef _IMGUIX_CORE_HPP_INCLUDED
#define _IMGUIX_CORE_HPP_INCLUDED

/// \file core.hpp
/// \brief Primary include file aggregating core ImGuiX components.
/// 
/// Includes the main interfaces and modules for application control,
/// window and event systems, controller-model interaction, and resource management.

// --- Event and PubSub system ---
#include "core/pubsub.hpp"                   ///< EventBus, Event, EventMediator
#include "core/events.hpp"                   ///< Common built-in events

// --- Resource system ---
#include "core/resource/ResourceRegistry.hpp" ///< Global registry for shared resources

// --- Controller and model interfaces ---
#include "core/window/WindowControl.hpp"     ///< Interface for window control
#include "core/controller/Controller.hpp"    ///< Base interface for controllers
#include "core/application/ApplicationControl.hpp" ///< Interface for application access
#include "core/model/Model.hpp"              ///< Base interface for models

// --- Windowing system ---
#include "core/window/WindowInstance.hpp"    ///< Abstract window interface
#include "core/window/WindowManager.hpp"     ///< Window management

// --- Application infrastructure ---
#include "core/application/Application.hpp"  ///< Main application class

#endif // _IMGUIX_CORE_HPP_INCLUDED