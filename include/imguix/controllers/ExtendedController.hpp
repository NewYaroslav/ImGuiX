#pragma once
#ifndef _IMGUIX_CONTROLLERS_EXTENDED_CONTROLLER_HPP_INCLUDED
#define _IMGUIX_CONTROLLERS_EXTENDED_CONTROLLER_HPP_INCLUDED

/// \file ExtendedController.hpp
/// \brief 

#include <imguix/core/controller/Controller.hpp>
#include <imguix/core/application/Application.hpp>
#include <imguix/core/window/WindowControl.hpp>

namespace ImGuiX::Controllers {

    /// \brief Extended controller with convenient access to the Application instance.
    ///
    /// Provides utility methods such as `application()` and `createWindow()` 
    /// that allow seamless interaction with the owning application.
    class ExtendedController : public Controller {
    public:
        using Controller::Controller;

        /// \brief Returns the underlying Application instance (cast from ApplicationControl).
        Application& application() {
            return static_cast<Application&>(window().application());
        }

        /// \brief Creates a new window instance of the specified type.
        /// \tparam WindowType Type derived from WindowInstance.
        /// \tparam Args Arguments forwarded to the window constructor.
        /// \return Reference to the created window instance.
        template <typename WindowType, typename... Args>
        WindowType& createWindow(Args&&... args) {
            return application().createWindow<WindowType>(std::forward<Args>(args)...);
        }
    };

} // namespace ImGuiX::Controllers

#endif // _IMGUIX_CONTROLLERS_EXTENDED_CONTROLLER_HPP_INCLUDED