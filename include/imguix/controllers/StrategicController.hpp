#pragma once
#ifndef _IMGUIX_CONTROLLERS_STRATEGIC_CONTROLLER_HPP_INCLUDED
#define _IMGUIX_CONTROLLERS_STRATEGIC_CONTROLLER_HPP_INCLUDED

/// \file StrategicController.hpp
/// \brief Controller with support for strategies and child controllers.

#include "ApplicationController.hpp"
#include <unordered_map>
#include <memory>
#include <cstddef>
#include <type_traits>

namespace ImGuiX::Controllers {

    /// \brief Controller supporting pluggable strategies and child controllers.
    class StrategicController : public ApplicationController {
    public:
        using ApplicationController::ApplicationController;

        virtual ~StrategicController() = default;

        /// \brief Creates and registers a strategy controller under a given key.
        /// \tparam Key Enum or indexable key type.
        /// \tparam ControllerType Controller type derived from Controller.
        /// \tparam Args Arguments passed to the controller constructor.
        /// \return Reference to the created controller.
        template <typename Key, typename ControllerType, typename... Args>
        ControllerType& createStrategyController(Key key, Args&&... args) {
            static_assert(std::is_base_of<Controller, ControllerType>::value,
                          "ControllerType must derive from Controller");

            auto ctrl = std::make_unique<ControllerType>(window(), std::forward<Args>(args)...);
            ControllerType& ref = *ctrl;
            m_strategies.emplace(static_cast<std::size_t>(key), std::move(ctrl));
            return ref;
        }

        /// \brief Selects the current active strategy by index or enum.
        template <typename EnumOrIndex>
        void selectStrategy(EnumOrIndex index) {
            m_strategy_key = static_cast<std::size_t>(index);
        }

        /// \brief Renders content via the active strategy.
        void drawStrategyContent() {
            if (auto it = m_strategies.find(m_strategy_key); it != m_strategies.end()) {
                it->second->drawContent();
            }
        }

        /// \brief Renders UI via the active strategy.
        void drawStrategyUi() {
            if (auto it = m_strategies.find(m_strategy_key); it != m_strategies.end()) {
                it->second->drawUi();
            }
        }

        /// \brief Draws content using the active strategy.
        ///
        /// Calls \ref drawStrategyContent to forward rendering to the currently
        /// selected strategy.
        void drawContent() override {
            drawStrategyContent();
        }

        /// \brief Draws UI using the active strategy.
        ///
        /// Calls \ref drawStrategyUi to display widgets from the selected
        /// strategy controller.
        void drawUi() override {
            drawStrategyUi();
        }

    protected:
        std::unordered_map<std::size_t, std::unique_ptr<Controller>> m_strategies;
        std::size_t m_strategy_key = 0;
    };

} // namespace ImGuiX::Controllers

#endif // _IMGUIX_CONTROLLERS_STRATEGIC_CONTROLLER_HPP_INCLUDED