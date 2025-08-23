#pragma once
#ifndef _IMGUIX_CONTROLLERS_EXTENDED_CONTROLLER_HPP_INCLUDED
#define _IMGUIX_CONTROLLERS_EXTENDED_CONTROLLER_HPP_INCLUDED

/// \file ExtendedController.hpp
/// \brief Extended controller that combines child and strategy controllers with access to the application context.

#include "StrategicController.hpp"

namespace ImGuiX::Controllers {

    /// \brief Extended controller with convenient access to window creation helpers.
    class ExtendedController : public StrategicController {
    public:
        using StrategicController::StrategicController;

        /// \brief Create and register a child controller.
        /// \tparam ControllerType Controller type derived from Controller.
        /// \tparam Args Arguments passed to the controller constructor.
        /// \return Reference to the created controller.
        template <typename ControllerType, typename... Args>
        ControllerType& createChildrenController(Args&&... args) {
            static_assert(std::is_base_of<Controller, ControllerType>::value,
                          u8"ControllerType must derive from Controller");

            auto ctrl = std::make_unique<ControllerType>(window(), std::forward<Args>(args)...);
            ControllerType& ref = *ctrl;
            m_children.push_back(std::move(ctrl));
            return ref;
        }

        /// \brief Draw UI from all registered child controllers.
        void drawChildUi() {
            for (auto& c : m_children) c->drawUi();
        }
        
        /// \brief Draw content from all registered child controllers.
        void drawChildContent() {
            for (auto& c : m_children) c->drawContent();
        }

        /// \brief Render UI overlay (widgets, HUDs, debug).
        void drawUi() override {
            drawChildUi();
            drawStrategyUi();
        }
        
        /// \brief Render frame content (background, world, etc.).
        void drawContent() override {
            drawChildContent();
            drawStrategyContent();
        }
        
    private:
        std::vector<std::unique_ptr<Controller>> m_children;
    };

} // namespace ImGuiX::Controllers

#endif // _IMGUIX_CONTROLLERS_EXTENDED_CONTROLLER_HPP_INCLUDED
