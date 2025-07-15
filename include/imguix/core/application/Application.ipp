#include <imgui.h>
#include <imguix/utils/path_utils.hpp>
#include <imguix/utils/encoding_utils.hpp>
#ifdef __EMSCRIPTEN__
#   include <emscripten.h>
#endif

namespace ImGuiX {

    Application::Application()
        : m_event_bus(), m_registry(), 
          m_window_manager(*static_cast<ApplicationControl*>(this)) {}

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
        m_window_manager.flushPending();
        m_window_manager.initializePending();
        initializePendingModels();
    }

    bool Application::loopIteration() {
        m_window_manager.flushPending();
        m_window_manager.initializePending();
        initializePendingModels();

        m_window_manager.removeClosed();
        if (allWindowsClosed()) {
#ifdef __EMSCRIPTEN__
            endLoop();
            emscripten_cancel_main_loop();
#endif
            return false;
        }

        if (!m_is_ini_once) {
            m_is_ini_once = true;
#ifdef __EMSCRIPTEN__
#   ifdef IMGUIX_EMSCRIPTEN_IDBFS
            ImGui::GetIO().IniFilename = "/imguix_fs/imgui.ini";
#   else
            ImGui::GetIO().IniFilename = nullptr;
#   endif
#else
            ImGuiIO& io = ImGui::GetIO();
            io.IniFilename = nullptr;
#endif
        }

        for (auto& model : m_models) {
            model->process();
        }

        m_event_bus.process();

        m_window_manager.handleEvents();
        m_window_manager.tickAll();
        m_window_manager.drawContentAll();
        m_window_manager.drawUiAll();
        m_window_manager.presentAll();

        if (!m_is_ini_loaded) {
            m_is_ini_loaded = true;
#ifdef __EMSCRIPTEN__
#   ifdef IMGUIX_EMSCRIPTEN_IDBFS
            // Ini settings will be loaded from mounted IDBFS automatically
            ImGui::LoadIniSettingsFromDisk("/imguix_fs/imgui.ini");
#   else
            ImGui::GetIO().IniFilename = nullptr;
#   endif
#else
            std::string ini_path = Utils::resolveExecPath(IMGUIX_INI_PATH);
            Utils::createDirectories(Utils::resolveExecPath(IMGUIX_CONFIG_DIR));
            ImGui::LoadIniSettingsFromDisk(ini_path.c_str());
#endif
        }

        if (++m_ini_save_frame_counter >= m_ini_save_interval) {
            m_ini_save_frame_counter = 0;
            if (ImGui::GetIO().WantSaveIniSettings) {
                saveIniSettings();
                ImGui::GetIO().WantSaveIniSettings = false;
            }
        }

        return true;
    }

    void Application::endLoop() {
        saveIniSettings();
        m_is_closing = true;
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

    void Application::saveIniSettings() {
#ifdef __EMSCRIPTEN__
#   ifdef IMGUIX_EMSCRIPTEN_IDBFS
        ImGui::SaveIniSettingsToDisk("/imguix_fs/imgui.ini");
        EM_ASM({ FS.syncfs(false, function(){}); });
#   endif
#else
        std::string ini_path = Utils::resolveExecPath(IMGUIX_INI_PATH);
        Utils::createDirectories(Utils::resolveExecPath(IMGUIX_CONFIG_DIR));
        ImGui::SaveIniSettingsToDisk(ini_path.c_str());
#endif
    }
} // namespace ImGuiX
