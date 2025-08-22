#ifdef __EMSCRIPTEN__
#   include <emscripten.h>
#endif

#include "../options/OptionsStore.hpp"

#ifndef IMGUIX_CONFIG_DIR
#   define IMGUIX_CONFIG_DIR "data/config"
#endif

namespace ImGuiX {

    Application::Application()
        : m_event_bus(), m_registry(),
          m_window_manager(*static_cast<ApplicationControl*>(this)) {
        m_registry.registerResource<OptionsStore>([] {
            return std::make_shared<OptionsStore>(
                    std::string(IMGUIX_CONFIG_DIR) + "/options.json",
                    0.5);
        });
    }

    void Application::run(bool async) {
#       ifdef IMGUIX_USE_SFML_BACKEND
        if (!registry().registerResource<DeltaClockSfml>()) {
            throw std::runtime_error("Failed to register DeltaClockSfml resource");
        }
#       endif

#if defined(__EMSCRIPTEN__)
        (void)async;
        startLoop();
        mainLoop();
#else
        if (async) {
            m_main_thread = std::thread([this]() {
#               ifdef IMGUIX_USE_SFML_BACKEND
                registry().getResource<DeltaClockSfml>().update();
#               endif
                startLoop();
                mainLoop();
            });
        } else {
#           ifdef IMGUIX_USE_SFML_BACKEND
            registry().getResource<DeltaClockSfml>().update();
#           endif
            startLoop();
            mainLoop();
        }
#endif
    }

    template<typename T, typename... Args>
    T& Application::createWindow(Args&&... args) {
        static_assert(std::is_base_of<WindowInstance, T>::value,
                      "T must derive from WindowInstance");

        int id = m_next_window_id++;

        auto window = std::make_unique<T>(id, *this,
                                          std::forward<Args>(args)...);
        T& ref = *window;

        m_window_manager.addWindow(std::move(window));
        return ref;
    }
    
    template<typename T, typename... Args>
    T& Application::createModel(Args&&... args) {
        static_assert(std::is_base_of<Model, T>::value, "T must be derived from Model");
        auto model = std::make_unique<T>(*this, std::forward<Args>(args)...);
        T& ref = *model;
        m_pending_models.push_back(model.get());
        m_models.emplace_back(std::move(model));
        return ref;
    }

    void Application::close() {
        auto async_event = std::make_unique<Events::ApplicationExitEvent>();
        m_event_bus.notifyAsync(std::move(async_event));
    }

    bool Application::isClosing() const {
        return m_is_closing;
    }

    Pubsub::EventBus& Application::eventBus() {
        return m_event_bus;
    }

    ResourceRegistry& Application::registry() {
        return m_registry;
    }
    
    const std::string& Application::name() const {
        return m_app_name;
    }

    bool Application::allWindowsClosed() const {
        return m_window_manager.allWindowsClosed();
    }

    void Application::initializePendingModels() {
        for (auto* model : m_pending_models) {
            if (model) model->onInit();
        }
        m_pending_models.clear();
    }

    void Application::startLoop() {
#ifdef __EMSCRIPTEN__
#   ifdef IMGUIX_EMSCRIPTEN_IDBFS
        EM_ASM({
            FS.mkdir('/imguix_fs');
            FS.mount(IDBFS, {}, '/imguix_fs');
            FS.syncfs(true, function(){});
        });
#   endif
#endif
        // Ensure initial windows and models are ready before entering loop
        m_window_manager.prepareFrame();
        initializePendingModels();
    }

    bool Application::loopIteration() {
        // Update window lifecycles before rendering the frame
        m_window_manager.prepareFrame();
        initializePendingModels();
        if (allWindowsClosed()) {
            m_event_bus.process();
            for (auto& model : m_models) {
                model->process();
            }
            m_event_bus.process();
#ifdef __EMSCRIPTEN__
            endLoop();
            emscripten_cancel_main_loop();
#endif
            return false;
        }

        m_window_manager.initIniAll();
        for (auto& model : m_models) {
            model->process();
        }
        m_event_bus.process();
        
        m_window_manager.processFrame();

        m_registry.getResource<OptionsStore>().update();

        return true;
    }

    void Application::endLoop() {
        m_is_closing = true;
        m_window_manager.shutdown();
        m_registry.getResource<OptionsStore>().saveNow();
    }

    void Application::mainLoop() {
#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop_arg([](void* arg) {
            static_cast<Application*>(arg)->loopIteration();
        }, this, 0, true);
#else
        while (loopIteration()) {}
        endLoop();
#endif
    }

} // namespace ImGuiX
