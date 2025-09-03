#ifdef __EMSCRIPTEN__
#   include <emscripten.h>
#   ifdef IMGUIX_EMSCRIPTEN_IDBFS
#       ifdef IMGUIX_ASYNCIFY
EM_ASYNC_JS(void, imguix_mount_idbfs, (void (*cb)(void*), void* ctx), {
    FS.mkdir('/imguix_fs');
    FS.mount(IDBFS, {}, '/imguix_fs');
    await FS.syncfs(true);
    Module["dynCall_vi"]($0, $1);
});
#       else
EM_JS(void, imguix_mount_idbfs, (void (*cb)(void*), void* ctx), {
    FS.mkdir('/imguix_fs');
    FS.mount(IDBFS, {}, '/imguix_fs');
    FS.syncfs(true, function(err) {
        Module["dynCall_vi"]($0, $1);
    });
});
#       endif
#   endif
#endif

#include <chrono>

#include <imguix/config/paths.hpp>
#include <imguix/core/options/OptionsStore.hpp>

namespace ImGuiX {

    Application::Application()
        : m_event_bus(), m_registry(),
          m_window_manager(*static_cast<ApplicationContext*>(this)) {
        initFilesystem();
        if (!m_is_fs_ready.load()) {
            std::unique_lock<std::mutex> lock(m_fs_ready_mutex);
            m_fs_ready_cv.wait(lock, [this] { return m_is_fs_ready.load(); });
        }
        m_registry.registerResource<OptionsStore>([] {
            return std::make_shared<OptionsStore>();
        });
    }

    Application::~Application() {
        if (m_main_thread.joinable()) {
            m_main_thread.join();
        }
    }

    void Application::run(bool async) {
#       ifdef IMGUIX_USE_SFML_BACKEND
        if (!registry().registerResource<DeltaClockSfml>()) {
            throw std::runtime_error(u8"Failed to register DeltaClockSfml resource");
        }
#       endif

#       if defined(__EMSCRIPTEN__)
        (void)async;
        startLoop();
        mainLoop();
#       else
        if (async) {
            m_main_thread = std::thread([this]() {
                registry().getResource<DeltaClockSfml>().update();
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
#       endif
    }

    WindowInstance& Application::createWindowImpl(WindowFactory factory) {
        int id = m_next_window_id++;
        auto window = factory(id);
        WindowInstance& ref = *window;
        m_window_manager.addWindow(std::move(window));
        return ref;
    }

    Model& Application::createModelImpl(ModelFactory factory) {
        auto model = factory();
        Model& ref = *model;
        m_pending_models.push_back(&ref);
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
            try { FS.mkdir('/imguix_fs/data'); } catch (e) {}
            try { FS.mkdir('/imguix_fs/data/config'); } catch (e) {}
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
        ImGuiX::Pubsub::SyncNotifier notifier{m_event_bus};
        if (allWindowsClosed()) {
            m_event_bus.process();
            for (auto& model : m_models) {
                model->process(notifier);
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
            model->process(notifier);
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

    void Application::initFilesystem() {
#if defined(__EMSCRIPTEN__) && defined(IMGUIX_EMSCRIPTEN_IDBFS)
        m_is_fs_ready = false;
        imguix_mount_idbfs(&Application::filesystemReady, this);
#endif
    }

#ifdef __EMSCRIPTEN__
    void Application::filesystemReady(void* arg) {
        auto* self = static_cast<Application*>(arg);
        {
            std::lock_guard<std::mutex> lock(self->m_fs_ready_mutex);
            self->m_is_fs_ready = true;
        }
        self->m_fs_ready_cv.notify_all();
    }
#endif

} // namespace ImGuiX
