#pragma once
#ifndef _IMGUIX_CONTROLLERS_EXTENDED_CONTROLLER_HPP_INCLUDED
#define _IMGUIX_CONTROLLERS_EXTENDED_CONTROLLER_HPP_INCLUDED

/// \file ExtendedController.hpp
/// \brief Extended controller that combines child and strategy controllers with access to the application context.

#include "StrategicController.hpp"
#include <imguix/core/application/Application.hpp>
//#include <imguix/core/window/WindowControl.hpp>

namespace ImGuiX::Controllers {

    /// \brief Extended controller with convenient access to the Application instance.
    ///
    /// Provides utility methods such as `application()` and `createWindow()` 
    /// that allow seamless interaction with the owning application.
    class ExtendedController : public StrategicController {
    public:
        using StrategicController::StrategicController;

        /// \brief Creates and registers a child controller.
        /// \tparam ControllerType Controller type derived from Controller.
        /// \tparam Args Arguments passed to the controller constructor.
        /// \return Reference to the created controller.
        template <typename ControllerType, typename... Args>
        ControllerType& createChildrenController(Args&&... args) {
            static_assert(std::is_base_of<Controller, ControllerType>::value,
                          "ControllerType must derive from Controller");

            auto ctrl = std::make_unique<ControllerType>(window(), std::forward<Args>(args)...);
            ControllerType& ref = *ctrl;
            m_children.push_back(std::move(ctrl));
            return ref;
        }

        /// \brief Renders UI from all registered child controllers.
        void drawChildUi() {
            for (auto& c : m_children) c->drawUi();
        }
        
        /// \brief Renders content from all registered child controllers.
        void drawChildContent() {
            for (auto& c : m_children) c->drawContent();
        }

        /// \brief Renders UI overlay (widgets, HUDs, debug).
        void drawUi() override {
            drawChildUi();
            drawStrategyUi();
        }
        
        /// \brief Renders frame content (background, world, etc.).
        void drawContent() override {
            drawChildContent();
            drawStrategyContent();
        }
        
    private:
        std::vector<std::unique_ptr<Controller>> m_children;
    };

} // namespace ImGuiX::Controllers

#endif // _IMGUIX_CONTROLLERS_EXTENDED_CONTROLLER_HPP_INCLUDED