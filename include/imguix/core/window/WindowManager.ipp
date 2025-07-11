namespace ImGuiX {

    WindowManager::WindowManager(ApplicationControl& app)
        : EventMediator(app.eventBus()), m_application(app) {}

    void WindowManager::addWindow(std::unique_ptr<WindowInstance> window) {
        m_windows.push_back(std::move(window));
    }
    
    void WindowManager::onEvent(const Pubsub::Event* const event) {
        if (event->is<Events::ApplicationExitEvent>()) {
            closeAll();
        }
    }
	
    void WindowManager::initializeAll() {
        for (auto& window : m_windows) {
            window->onInit();
        }
    }

    void WindowManager::closeAll() {
        for (auto& window : m_windows) {
            window->close();
        }
    }

    void WindowManager::handleEvents() {
        for (auto& window : m_windows) {
            window->handleEvents();
        }
    }

    void WindowManager::tickAll() {
#       ifdef IMGUIX_USE_SFML_BACKEND
        registry().getResource<DeltaClockSfml>().update();
#       endif
        for (auto& window : m_windows) {
            window->tick();
        }
    }

    void WindowManager::drawUiAll() {
        for (auto& window : m_windows) {
            window->drawUi();
        }
    }

    void WindowManager::drawContentAll() {
        for (auto& window : m_windows) {
            window->drawContent();
        }
    }

    void WindowManager::presentAll() {
        for (auto& window : m_windows) {
            window->present();
        }
    }

    std::size_t WindowManager::windowCount() const {
        return m_windows.size();
    }

    bool WindowManager::allWindowsClosed() const {
        for (const auto& window : m_windows) {
            if (window && window->isOpen()) {
                return false;
            }
        }
        return true;
    }

    ResourceRegistry& WindowManager::registry() {
        return m_application.registry();
    }

} // namespace ImGuiX