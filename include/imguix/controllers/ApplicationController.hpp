#pragma once
#ifndef _IMGUIX_CONTROLLERS_APPLICATION_CONTROLLER_HPP_INCLUDED
#define _IMGUIX_CONTROLLERS_APPLICATION_CONTROLLER_HPP_INCLUDED

/// \file ApplicationController.hpp
/// \brief Controller with convenience helpers for accessing the application context.

#include <imguix/core/controller/Controller.hpp>
#include <imguix/core/application/ApplicationContext.hpp>

namespace ImGuiX::Controllers {

    /// \brief Controller that provides convenient access to the application context.
    /// \note Offers helper method to create new windows.
    class ApplicationController : public Controller {
    public:
        using Controller::Controller;

        /// \brief Create and register a window of the specified type.
        /// \tparam WindowType Type derived from `WindowInstance`.
        /// \tparam Args Arguments forwarded to the constructor of the window.
        /// \return Reference to the created window.
        template<class WindowType, class... Args>
        WindowType& createWindow(Args&&... args) {
            return window().application().createWindow<WindowType>(
                    std::forward<Args>(args)...);
        }
    };

} // namespace ImGuiX::Controllers

#endif // _IMGUIX_CONTROLLERS_APPLICATION_CONTROLLER_HPP_INCLUDED
