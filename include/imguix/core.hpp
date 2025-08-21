#pragma once
#ifndef _IMGUIX_CORE_HPP_INCLUDED
#define _IMGUIX_CORE_HPP_INCLUDED

/// \file core.hpp
/// \brief Primary include file aggregating core ImGuiX components.
/// 
/// Includes the main interfaces and modules for application control,
/// window and event systems, controller-model interaction, and resource management.

#if defined(IMGUIX_USE_SFML_BACKEND)
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <imgui-SFML.h>
#elif defined(IMGUIX_USE_GLFW_BACKEND)
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#elif defined(IMGUIX_USE_SDL2_BACKEND)
#include <SDL.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#else
// Без бэкенда
#endif

#ifdef IMGUI_ENABLE_FREETYPE
#include <imgui_freetype.h>
#endif

#include <imgui.h>

// --- Event and PubSub system ---
#include "core/pubsub.hpp"                         ///< EventBus, Event, EventMediator
#include "core/events.hpp"                         ///< Common built-in events

// --- Locale and Fonts ---
#include "core/i18n.hpp"                           ///<
//#include "core/fonts/FontManager.hpp"              ///< FontManager: централизованная загрузка шрифтов (atlas ImGui + FreeType).

// --- Resource system ---
#include "core/resource/ResourceRegistry.hpp"      ///< Global registry for shared resources

// --- Controller and model interfaces ---
#include "core/application/ApplicationControl.hpp" ///< Interface for application access
#include "core/window/WindowControl.hpp"           ///< Interface for window control
#include "core/controller/Controller.hpp"          ///< Base interface for controllers
#include "core/model/Model.hpp"                    ///< Base interface for models

// --- Windowing system ---
#include "core/window/WindowInstance.hpp"          ///< Abstract window interface
#include "core/window/WindowManager.hpp"           ///< Window management

// --- Application infrastructure ---
#include "core/application/Application.hpp"        ///< Main application class

#endif // _IMGUIX_CORE_HPP_INCLUDED