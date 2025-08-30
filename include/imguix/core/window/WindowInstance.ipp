#include <imgui.h>
#include <imguix/utils/path_utils.hpp>

namespace ImGuiX {

    WindowInstance::WindowInstance(int id, ApplicationContext& app, std::string name)
        : EventMediator(app.eventBus()),
          m_window_id(id),
          m_window_name(std::move(name)),
          m_application(app) {
        m_theme_manager.registerTheme("light", std::make_unique<Themes::LightTheme>());
        m_theme_manager.registerTheme("dark", std::make_unique<Themes::DarkTheme>());
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
        notifications().render();
    }

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

    void WindowInstance::initializePendingControllers() {
        setCurrentWindow();
        for (auto* ctrl : m_pending_controllers) {
            if (ctrl) ctrl->onInit();
        }
        m_pending_controllers.clear();
    }

// --- WindowInterface interface ---

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
    
    void WindowInstance::setTheme(std::string id) {
        themeManager().setTheme(std::move(id));
    }
    
    Pubsub::EventBus& WindowInstance::eventBus() {
        return m_application.eventBus();
    }

    ResourceRegistry& WindowInstance::registry() {
        return m_application.registry();
    }

    OptionsStore::Control& WindowInstance::options() {
        return m_application.registry().getResource<OptionsStore>().control();
    }

    const OptionsStore::View& WindowInstance::options() const {
        return m_application.registry().getResource<OptionsStore>().view();
    }
    
    ApplicationContext& WindowInstance::application() {
        return m_application;
    }
    
#   ifdef IMGUIX_USE_SFML_BACKEND
    sf::RenderWindow& WindowInstance::getRenderTarget() {
        return m_window;
    }
#   endif

    const ImGuiX::I18N::LangStore& WindowInstance::langStore() const {
        return m_lang_store;
    }
    
    ImGuiX::Fonts::FontManager::View& WindowInstance::fontsView() noexcept { 
        return m_font_manager.view(); 
    }

    ImGuiX::Fonts::FontManager::Control& WindowInstance::fontsControl() noexcept { 
        return m_font_manager.control(); 
    }

    ImGuiX::Themes::ThemeManager& WindowInstance::themeManager() noexcept { 
        return m_theme_manager; 
    }
    
    ImGuiX::Notify::NotificationManager& WindowInstance::notifications() {
        return m_notification_manager;
    }

    std::string WindowInstance::iniPath() const {
#   ifdef __EMSCRIPTEN__
#       ifdef IMGUIX_EMSCRIPTEN_IDBFS
        return u8"/imguix_fs/imgui-" + std::to_string(m_window_id) + u8".ini";
#       else
        return {};
#       endif
#   else
        return ImGuiX::Utils::resolveExecPath(
            std::string(IMGUIX_CONFIG_DIR) + u8"/imgui-" + std::to_string(m_window_id) + u8".ini");
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
    
    void WindowInstance::requestLanguageChange(const std::string& lang) {
        if (!lang.empty()) m_pending_lang = lang;
    }
    
    void WindowInstance::applyPendingLanguageChange() {
        if (m_pending_lang.empty()) return;
        setCurrentWindow();                        // активировать контекст окна
        onBeforeLanguageApply(m_pending_lang);     // виртуальный хук (пересборка шрифтов и т.п.)
        m_lang_store.set_language(m_pending_lang); // фактическая смена языка
        m_font_manager.rebuildIfNeeded();
        m_pending_lang.clear();
    }
    
    // ---
    
    void WindowInstance::fontsStartInit() {
        m_in_init_phase = true;
    }

    void WindowInstance::fontsBeginManual() {
        assert(m_in_init_phase && !m_is_fonts_init && u8"fontsBeginManual() only allowed in onInit(), before initFonts()");
        m_font_manager.beginManual();
        m_is_fonts_manual = true;
    }
    
    void WindowInstance::fontsSetLocale(std::string locale) {
        if (locale == m_font_manager.activeLocale()) {
            return;
        }
        m_font_manager.setLocale(std::move(locale));
    }
    
    void WindowInstance::fontsSetRangesPreset(std::string preset) {
        assert(m_in_init_phase && u8"fontsSetRangesPreset() только в onInit()");
        m_font_manager.setRanges(std::move(preset));
    }

    void WindowInstance::fontsSetRangesExplicit(const std::vector<ImWchar>& pairs) {
        assert(m_in_init_phase && u8"fontsSetRangesExplicit() только в onInit()");
        m_font_manager.setRanges(pairs);
    }

    void WindowInstance::fontsClearRanges() {
        assert(m_in_init_phase && u8"fontsClearRanges() только в onInit()");
        m_font_manager.clearRanges();
    }

    void WindowInstance::fontsAddBody(const ImGuiX::Fonts::FontFile& ff) {
        assert(m_in_init_phase && u8"Only allowed in onInit()");
        m_font_manager.addFontBody(ff);
    }

    void WindowInstance::fontsAddHeadline(ImGuiX::Fonts::FontRole role, const ImGuiX::Fonts::FontFile& ff) {
        assert(m_in_init_phase && u8"Only allowed in onInit()");
        m_font_manager.addFontHeadline(role, ff);
    }

    void WindowInstance::fontsAddMerge(ImGuiX::Fonts::FontRole role, const ImGuiX::Fonts::FontFile& ff) {
        assert(m_in_init_phase && u8"Only allowed in onInit()");
        m_font_manager.addFontMerge(role, ff);
    }

    void WindowInstance::fontsAddMerge(const ImGuiX::Fonts::FontFile& ff) {
        assert(m_in_init_phase && u8"Only allowed in onInit()");
        m_font_manager.addFontMerge(ff);
    }

    bool WindowInstance::fontsBuildNow() {
        assert(m_in_init_phase && u8"Only allowed in onInit()");
        auto br = m_font_manager.buildNow();
        if (br.success) m_is_fonts_init = true;
        if (!br.success) {
            notify(IMGUIX_LOG_EVENT(ImGuiX::Events::LogLevel::Error, u8"Font init failed: {}"));
        }
        return br.success;
    }

    void WindowInstance::buildFonts() {
        m_in_init_phase = false;
        if (m_is_fonts_init) return;
        if (m_is_fonts_manual) return;
        
        auto br = m_font_manager.initFromJsonOrDefaults();
        m_is_fonts_init = br.success;
        if (!br.success) {
            notify(IMGUIX_LOG_EVENT(ImGuiX::Events::LogLevel::Error, u8"Font init failed: {}"));
        }
    }
}
