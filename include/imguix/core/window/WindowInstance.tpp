#pragma once

namespace ImGuiX {

    template <typename ControllerType, typename... Args>
    ControllerType& WindowInstance::createController(Args&&... args) {
        static_assert(std::is_base_of<Controller, ControllerType>::value,
                      u8"ControllerType must derive from Controller");

        auto ctrl = std::make_unique<ControllerType>(
            static_cast<WindowInterface&>(*this),
            std::forward<Args>(args)...);
        ControllerType* ptr = ctrl.get();
        ControllerType& ref = *ptr;
        m_pending_controllers.push_back(ptr);
        m_controllers.push_back(std::move(ctrl));
        return ref;
    }

} // namespace ImGuiX
