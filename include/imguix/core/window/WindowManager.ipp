#include <algorithm>

namespace ImGuiX {

    WindowManager::WindowManager(ApplicationContext& app)
        : EventMediator(app.eventBus()), m_application(app) {
        subscribe<Events::ApplicationExitEvent>();
        subscribe<Events::LangChangeEvent>();
    }

    void WindowManager::addWindow(std::unique_ptr<WindowInstance> window) {
        m_pending_init.push_back(window.get());
        m_pending_add.push_back(std::move(window));
    }
    
    void WindowManager::onEvent(const Pubsub::Event* const event) {
        if (event->is<Events::ApplicationExitEvent>()) {
            closeAll();
        } else
        if (event->is<Events::LangChangeEvent>()) {
            m_lang_events.push_back(event->asRef<Events::LangChangeEvent>());
        }
    }

    void WindowManager::prepareFrame() {
        // Handle lifecycle transitions before processing a frame.
        flushPending();        // move pending windows into the active list
        initializePending();   // run onInit on newly added windows
        removeClosed();        // purge windows that have been closed
    }
    
    void WindowManager::flushPending() {
        for (auto& window : m_pending_add) {
            m_windows.push_back(std::move(window));
        }
        m_pending_add.clear();
    }

    void WindowManager::initializePending() {
        for (auto* window : m_pending_init) {
			if (!window) continue;
			window->fontsStartInit();
            window->onInit();
			window->buildFonts();
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
    
    void WindowManager::processLanguageEvents() {
        while (!m_lang_events.empty()) {
            Events::LangChangeEvent ev = std::move(m_lang_events.front());
            m_lang_events.pop_front();

            if (ev.apply_to_all) {
                for (auto& window : m_windows) {
                    window->requestLanguageChange(ev.lang);
                    window->applyPendingLanguageChange();
                }
            } else {
                auto* window = findWindowById(ev.window_id);
                if (window) {
                    window->requestLanguageChange(ev.lang);
                    window->applyPendingLanguageChange();
                }
            }
        }
    }
    
    void WindowManager::initIniAll() {
        for (auto& window : m_windows) {
            window->initIni();
        }
    }
    
    void WindowManager::loadIniAll() {
        for (auto& window : m_windows) {
            window->loadIni();
        }
    }
    
    void WindowManager::saveIniNowAll() {
        for (auto& window : m_windows) {
            window->saveIniNow();
        }
    }
    
    void WindowManager::saveIniAll() {
        if (++m_ini_save_frame_counter >= m_ini_save_interval) {
            m_ini_save_frame_counter = 0;
            saveIniNowAll();
        }
    }

    void WindowManager::tickAll() {
        processLanguageEvents();

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

    void WindowManager::processFrame() {
        handleEvents();
        tickAll();
        drawContentAll();
        drawUiAll();
        presentAll();
        loadIniAll();
        saveIniAll();
    }

    void WindowManager::removeClosed() {
        // Remove-erase idiom to drop null or closed windows.
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
    
    inline WindowInstance* WindowManager::findWindowById(int id) noexcept {
        for (auto& window : m_windows) {
            WindowInstance* ptr = window.get();
            if (ptr && ptr->id() == id) return ptr;
        }
        return nullptr;
    }

    inline const WindowInstance* WindowManager::findWindowById(int id) const noexcept {
        for (auto const& window : m_windows) {
            auto const* ptr = window.get();
            if (ptr && ptr->id() == id) return ptr;
        }
        return nullptr;
    }

    void WindowManager::shutdown() {}

} // namespace ImGuiX

