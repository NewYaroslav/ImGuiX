#include <algorithm>

namespace ImGuiX {

    WindowManager::WindowManager(ApplicationControl& app)
        : EventMediator(app.eventBus()), m_application(app) {}

    void WindowManager::addWindow(std::unique_ptr<WindowInstance> window) {
        m_pending_init.push_back(window.get());
        m_pending_add.push_back(std::move(window));
    }
    
    void WindowManager::onEvent(const Pubsub::Event* const event) {
        if (event->is<Events::ApplicationExitEvent>()) {
            closeAll();
        }
    }
    
    void WindowManager::flushPending() {
        for (auto& window : m_pending_add) {
            m_windows.push_back(std::move(window));
        }
        m_pending_add.clear();
    }

    void WindowManager::initializePending() {
        for (auto* window : m_pending_init) {
            if (window) window->onInit();
        }
        m_pending_init.clear();
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
            if (!window->isOpen()) continue;
            window->drawUi();
        }
    }

    void WindowManager::drawContentAll() {
        for (auto& window : m_windows) {
            if (!window->isOpen()) continue;
            window->drawContent();
        }
    }

    void WindowManager::presentAll() {
        for (auto& window : m_windows) {
            if (!window->isOpen()) continue;
            window->present();
        }
    }

    void WindowManager::removeClosed() {
        m_windows.erase(
            std::remove_if(m_windows.begin(), m_windows.end(),
                           [](const std::unique_ptr<WindowInstance>& w) {
                               return w == nullptr || !w->isOpen();
                           }),
            m_windows.end());
    }

    std::size_t WindowManager::windowCount() const {
        return m_windows.size();
    }

    bool WindowManager::allWindowsClosed() const {
        return m_windows.empty();
    }

    ResourceRegistry& WindowManager::registry() {
        return m_application.registry();
    }

} // namespace ImGuiX

