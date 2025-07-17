namespace ImGuiX {

    WindowInstance::WindowInstance(int id, ApplicationControl& app, std::string name)
        : EventMediator(app.eventBus()),
          m_window_id(id),
          m_window_name(std::move(name)),
          m_application(app) {
    }

    void WindowInstance::drawContent() {
        for (auto& ctrl : m_controllers) {
            ctrl->drawContent();
        }
    }

    void WindowInstance::drawUi() {
#       ifdef IMGUIX_USE_SFML_BACKEND
        ImGui::SFML::SetCurrentWindow(m_window);
#       endif
        for (auto& ctrl : m_controllers) {
            ctrl->drawUi();
        }
    }

    template <typename ControllerType, typename... Args>
    ControllerType& WindowInstance::createController(Args&&... args) {
        static_assert(std::is_base_of<Controller, ControllerType>::value,
                      "ControllerType must derive from Controller");

        auto ctrl = std::make_unique<ControllerType>(*this, std::forward<Args>(args)...);
        ControllerType& ref = *ctrl;
        m_controllers.push_back(std::move(ctrl));
        return ref;
    }

// --- WindowControl interface ---

    int WindowInstance::id() const {
        return m_window_id;
    }

    const std::string& WindowInstance::name() const {
        return m_window_name;
    }

    int WindowInstance::width() const {
        return m_width;
    }

    int WindowInstance::height() const {
        return m_height;
    }
    
    Pubsub::EventBus& WindowInstance::eventBus() {
        return m_application.eventBus();
    }

    ResourceRegistry& WindowInstance::registry() {
        return m_application.registry();
    }
    
    ApplicationControl& WindowInstance::application() {
        return m_application;
    }
    
#   ifdef IMGUIX_USE_SFML_BACKEND
    sf::RenderWindow& WindowInstance::getRenderTarget() {
        return m_window;
    }
#   endif
}