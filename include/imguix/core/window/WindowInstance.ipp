#include <imgui.h>
#include <imguix/utils/path_utils.hpp>

namespace ImGuiX {

    WindowInstance::WindowInstance(int id, ApplicationControl& app, std::string name)
        : EventMediator(app.eventBus()),
          m_window_id(id),
          m_window_name(std::move(name)),
          m_application(app) {
    }

    void WindowInstance::drawContent() {
        setCurrentWindow();
        for (auto& ctrl : m_controllers) {
            ctrl->drawContent();
        }
    }

    void WindowInstance::drawUi() {
        setCurrentWindow();
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

    std::string WindowInstance::iniPath() const {
#   ifdef __EMSCRIPTEN__
#       ifdef IMGUIX_EMSCRIPTEN_IDBFS
        return "/imguix_fs/imgui-" + std::to_string(m_window_id) + ".ini";
#       else
        return {};
#       endif
#   else
        return ImGuiX::Utils::resolveExecPath(
            std::string(IMGUIX_CONFIG_DIR) + "/imgui-" + std::to_string(m_window_id) + ".ini");
#   endif
    }

    void WindowInstance::initIni() {
        if (m_is_ini_once) return;
        setCurrentWindow();
        m_ini_path = iniPath();
#   ifdef __EMSCRIPTEN__
#       ifdef IMGUIX_EMSCRIPTEN_IDBFS
        ImGui::GetIO().IniFilename = m_ini_path.c_str();
#       else
        ImGui::GetIO().IniFilename = nullptr;
#       endif
#   else
        ImGui::GetIO().IniFilename = nullptr;
#   endif
        m_is_ini_once = true;
    }

    void WindowInstance::loadIni() {
        if (m_is_ini_loaded) return;
        setCurrentWindow();
#   ifdef __EMSCRIPTEN__
#       ifdef IMGUIX_EMSCRIPTEN_IDBFS
        // Ini settings will be loaded from mounted IDBFS automatically
        ImGui::LoadIniSettingsFromDisk(m_ini_path.c_str());
#       else
        ImGui::GetIO().IniFilename = nullptr;
#       endif
#   else
        Utils::createDirectories(Utils::resolveExecPath(IMGUIX_CONFIG_DIR));
        ImGui::LoadIniSettingsFromDisk(m_ini_path.c_str());
#   endif
        m_is_ini_loaded = true;
    }

    void WindowInstance::saveIniNow() {
        setCurrentWindow();
        ImGui::SaveIniSettingsToDisk(iniPath().c_str());
        if (!ImGui::GetIO().WantSaveIniSettings) return;
#   ifdef __EMSCRIPTEN__
#       ifdef IMGUIX_EMSCRIPTEN_IDBFS
        ImGui::SaveIniSettingsToDisk(m_ini_path.c_str());
        EM_ASM({ FS.syncfs(false, function(){}); });
#       endif
#   else
        Utils::createDirectories(Utils::resolveExecPath(IMGUIX_CONFIG_DIR));
        ImGui::SaveIniSettingsToDisk(m_ini_path.c_str());
#   endif
        ImGui::GetIO().WantSaveIniSettings = false;
    }
}