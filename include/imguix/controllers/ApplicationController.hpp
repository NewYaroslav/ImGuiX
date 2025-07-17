#pragma once
#ifndef _IMGUIX_CONTROLLERS_APPLICATION_CONTROLLER_HPP_INCLUDED
#define _IMGUIX_CONTROLLERS_APPLICATION_CONTROLLER_HPP_INCLUDED

/// \file ApplicationController.hpp
/// \brief Controller with direct access to the owning Application instance.

#include <imguix/core/controller/Controller.hpp>
#include <imguix/core/application/Application.hpp>

namespace ImGuiX::Controllers {

    /// \brief Controller that provides convenient access to the Application instance.
    ///
    /// This controller offers helper methods to access the `Application` object
    /// and create new windows within the application context.
    class ApplicationController : public Controller {
    public:
        using Controller::Controller;

        /// \brief Returns a reference to the owning Application instance.
        ///
        /// Internally casts the window's `ApplicationControl` to `Application`.
        /// \return Reference to the `Application` object.
        Application& application() {
            return static_cast<Application&>(window().application());
        }

        /// \brief Creates and registers a new window instance of the specified type.
        /// \tparam WindowType Type derived from `WindowInstance`.
        /// \tparam Args Arguments forwarded to the constructor of the window.
        /// \return Reference to the created window.
        template <typename WindowType, typename... Args>
        WindowType& createWindow(Args&&... args) {
            return application().createWindow<WindowType>(std::forward<Args>(args)...);
        }
    };

} // namespace ImGuiX::Controllers

#endif // _IMGUIX_CONTROLLERS_APPLICATION_CONTROLLER_HPP_INCLUDED
