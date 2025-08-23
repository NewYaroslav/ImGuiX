#include <iostream>
#include <imguix/core.hpp>
#include <imguix/widgets/auth_panel.hpp>
#include <imguix/widgets/domain_selector.hpp>
#include <imguix/widgets/virtual_keyboard.hpp>

#include <imguix/widgets/hours_selector.hpp>
#include <imguix/widgets/time_picker.hpp>
#include <imguix/widgets/date_picker.hpp>

#include <imguix/widgets/loading_spinner.hpp>
#include <imguix/widgets/proxy_panel.hpp>

#include <imguix/widgets/text_center.hpp>
#include <imguix/widgets/list_editor.hpp>

namespace i18n = ImGuiX::I18N;

class I18nController : public ImGuiX::Controller {
public:
    I18nController(ImGuiX::WindowInterface& window)
        : Controller(window) {}

    void drawContent() override {

    }

    // --- UI demo ---
    void drawUi() override {
        ImGui::PushID(window().id());
        ImGui::PushFont(nullptr, 18.0f);
        ImGui::Begin(window().id() == 0 ? "i18n main" : "i18n child");

        // ... your existing i18n demo UI ...

        // Insert the auth demo section
        ImGui::Separator();
        drawAuthDemo();

        ImGui::End();
        ImGui::PopFont();
        ImGui::PopID();
    }


private:
    std::string                          m_last_font_lang;
    int                                  m_items_count{1};
    int                                  m_notif_count{2};

    struct AuthDemoState {
        // UI toggles for on-screen keyboards
        ImGuiX::Widgets::VirtualKeyboardConfig kcfg;
    
        bool kbd_email = false;
        bool kbd_pass  = false;
        bool kbd_host  = false;

        // Configs (static UI config, can be tweaked at runtime)
        ImGuiX::Widgets::AuthPanelConfig      auth_cfg{};
        ImGuiX::Widgets::AuthData             auth_data;

        ImGuiX::Widgets::DomainSelectorConfig dom_cfg{};
        
        ImGuiX::Widgets::LoadingSpinnerConfig sp_cfg{};
        
        // hours selector state
        std::vector<int>        hours {9,10,11,12,13,14,15,16,17}; // default: 9-17
        ImGuiX::Widgets::HoursSelectorConfig hs_cfg{};
        
        // proxy
        ImGuiX::Widgets::ProxySettings   proxy{};
        ImGuiX::Widgets::ProxyPanelConfig proxy_cfg{};
        
        // 
        int      time_of_day_sec = 18 * 3600 + 0 * 60 + 0;  // 18:00:00
        int64_t  tz_offset_sec   = 2 * 3600;                // +02:00:00
        bool     has_dst_out = false;
        int      tz_index_io = 0;
        int64_t  date_ts       = 0;                          // 1970-01-01

        ImGuiX::Widgets::TimePickerConfig       tp_cfg{};
        ImGuiX::Widgets::TimeOffsetPickerConfig to_cfg{};
        ImGuiX::Widgets::DatePickerConfig       dp_cfg{};
        
        std::vector<std::string> names {"Alice", "Bob"};
        std::vector<int> numbers {1, 2, 3};

        AuthDemoState() {
            auth_cfg.header                 = "Login";
            auth_cfg.hint_email             = "email";
            auth_cfg.hint_password          = "password";
            auth_cfg.hint_host              = "host";
            auth_cfg.connected_label        = "connected";
            auth_cfg.connect_label          = "connect";
            auth_cfg.show_host              = true;
            auth_cfg.show_connection_state  = true;
            auth_cfg.password_toggle.enabled  = true;
            auth_cfg.password_toggle.use_icon = true;

            auth_cfg.validate_email         = true;
            auth_cfg.init                   = true;   // connection state is meaningful after init
            auth_cfg.connected              = false;  // pretend we are disconnected
            
            auth_cfg.vk.enabled_host     = true;
            auth_cfg.vk.enabled_email    = true;
            auth_cfg.vk.enabled_password = true;
            auth_cfg.vk.use_icon         = true;
            auth_cfg.vk.icon_text        = u8"\uE312"; // или u8"\uE23E"
            auth_cfg.vk.vk_as_overlay    = true;

            // Поведение Enter: отправить и закрыть
            auth_cfg.vk_cfg.submit_on_enter  = true;
            auth_cfg.vk_cfg.capture_hw_enter = true;      // физический Enter, пока фокус на VK
            auth_cfg.vk_cfg.shift_enter_newline = true;   // Shift+Enter => перенос строки
            auth_cfg.vk_cfg.border = false;
            auth_cfg.vk_cfg.on_submit = [](const std::string& text){
                // your submit logic (дополнительно к автозакрытию внутри обёртки)
            };

            // Connect action
            auth_cfg.on_connect = [this](){
                // NOTE: replace with real connect logic
                // Toggle connected flag as a demo effect.
                auth_cfg.connected = !auth_cfg.connected;
            };
            
            auth_data.email    = "guest@example.com";
            auth_data.password = "";
            auth_data.host     = "demo.local";
            
            // keyboards
            kcfg.size = ImVec2(0, 0);       // авто-высота
            kcfg.locales = { 
                "en",
                "ru","es","pt-BR","id",
                "uk","be",
                "pt","de","fr","it","pl","tr",
                "vi","th","ar","hi","ur"
            };
            kcfg.start_locale_index = 0;    // 0=en, 1=ru
            kcfg.show_locale_combo = true;
            kcfg.key_size = ImVec2(26, 26);
            kcfg.spacing = 4.0f;
            kcfg.show_top_preview = false;

            // Domain presets
            dom_cfg.header         = "Server";
            dom_cfg.hint_domain    = "domain";
            dom_cfg.custom_text    = "Custom";
            dom_cfg.default_domain = auth_data.host; // when switching to "Custom", keep current
            dom_cfg.show_help      = true;
            dom_cfg.help_text      = "Choose predefined server or type custom host.";
            dom_cfg.domains        = { "demo.local", "test.example.com" };
            
            //sp_cfg.radius = 28.0f;
            //sp_cfg.angular_speed = 5.0f; // быстрее вращение
            
            // hours selector config
            hs_cfg.label       = "Hours";
            hs_cfg.empty_hint  = "none";
            hs_cfg.child_size  = ImVec2(260, 110);
            hs_cfg.cell_size   = ImVec2(18, 18);
            hs_cfg.rows        = 3;
            hs_cfg.cols        = 8;   // covers 0..23
            hs_cfg.combo_width = 260.0f;
            
            // Proxy defaults
            proxy.use_proxy = false;
            proxy.ip        = "127.0.0.1";
            proxy.port      = 8080;
            proxy.type      = ImGuiX::Widgets::ProxyType::HTTP;

            proxy_cfg.header          = "Proxy";
            proxy_cfg.label_use_proxy = "use proxy";
            proxy_cfg.button_check    = "check proxy";
            proxy_cfg.label_checked   = "checked";
            proxy_cfg.show_type       = true;
            proxy_cfg.show_check      = true;

            proxy_cfg.on_check = [this](){
                // TODO: replace with real async check.
                // Демо: отмечаем "checked" и переключаем успех/провал.
                proxy_cfg.checked  = true;
                proxy_cfg.check_ok = !proxy_cfg.check_ok;
            };
            
            //
            tp_cfg.label       = "Time of day";
            tp_cfg.desc        = "HH:MM:SS";

            to_cfg.label       = "GMT offset";
            to_cfg.desc        = "±HH:MM:SS";
            to_cfg.show_gmt    = true;
        }
    };
    
    AuthDemoState m_auth{};

    //
    void drawAuthDemo() {
        // ---
        ImGui::SeparatorText("Auth demo");

        // Panel (email+password+host+connect)
        ImGuiX::Widgets::AuthPanelResult r = ImGuiX::Widgets::AuthPanel(
            "auth.panel",
            m_auth.auth_cfg,
            m_auth.auth_data
        );

        // Show a compact status line
        ImGui::TextUnformatted("Status:");
        ImGui::SameLine();
        ImGui::Text("%s | email: %s",
            m_auth.auth_cfg.connected ? "connected" : "disconnected",
            m_auth.auth_data.email_valid ? "ok" : "invalid");

        // --- On-screen keyboard toggles
        ImGui::Separator();
        ImGui::TextUnformatted("Virtual keyboards:");
        ImGui::Checkbox("Email", &m_auth.kbd_email); ImGui::SameLine();
        ImGui::Checkbox("Password", &m_auth.kbd_pass); ImGui::SameLine();
        ImGui::Checkbox("Host", &m_auth.kbd_host);

        if (m_auth.kbd_email)  ImGuiX::Widgets::VirtualKeyboard("kbd.email",  m_auth.auth_data.email, m_auth.kcfg);
        if (m_auth.kbd_pass)   ImGuiX::Widgets::VirtualKeyboard("kbd.pass",   m_auth.auth_data.password,  m_auth.kcfg);
        if (m_auth.kbd_host)   ImGuiX::Widgets::VirtualKeyboard("kbd.host",   m_auth.auth_data.host,  m_auth.kcfg);

        // Domain selector (presets + custom)
        ImGui::Separator();
        if (ImGuiX::Widgets::DomainSelector("auth.domain", m_auth.dom_cfg, m_auth.auth_data.host)) {
            // optional: react on host change
        }

        // Example handling of panel events (bitmask)
        if ((static_cast<unsigned>(r) & 
             static_cast<unsigned>(ImGuiX::Widgets::AuthPanelResult::ConnectClicked))) {
            // e.g., log, start async connect, etc.
            // Here we already toggled connected in on_connect callback.
        }
        
        // Hours section
        ImGui::SeparatorText("Hours");
        // Пресеты (быстрые кнопки)
        if (ImGui::SmallButton("Work 9-18")) {
            m_auth.hours.clear();
            for (int h = 9; h <= 18; ++h) m_auth.hours.push_back(h);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Night 0-6")) {
            m_auth.hours.clear();
            for (int h = 0; h <= 6; ++h) m_auth.hours.push_back(h);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Even")) { // чётные
            m_auth.hours.clear();
            for (int h = 0; h < 24; h += 2) m_auth.hours.push_back(h);
        }

        // Сам комбобокс с гридом 3x8
        bool changed = ImGuiX::Widgets::HoursSelector("auth.hours", m_auth.hours, m_auth.hs_cfg);

        // Короткая сводка
        ImGui::SameLine();
        ImGui::TextDisabled("(%d selected)", (int)m_auth.hours.size());

        // Реакция на изменения (опц.)
        if (changed) {
            // TODO: пересчитать расписание/валидировать др. настройки
        }
        
        // --- Spinner ---
        ImGui::SeparatorText("Spinner");

        ImGuiX::Widgets::LoadingSpinner("auth.spinner", m_auth.sp_cfg);

        // Параметры на лету (для ручного теста)
        ImGui::SliderFloat("radius", &m_auth.sp_cfg.radius, 2.0f, 48.0f, "%.0f");
        ImGui::SliderFloat("thickness", &m_auth.sp_cfg.thickness, 1.0f, 12.0f, "%.1f");
        ImGui::SliderInt("segments", &m_auth.sp_cfg.segments, 8, 128);
        ImGui::SliderFloat("sweep", &m_auth.sp_cfg.sweep_ratio, 0.1f, 1.0f, "%.2f");
        ImGui::SliderFloat("speed", &m_auth.sp_cfg.angular_speed, 0.0f, 20.0f, "%.1f");
        
        ImGui::SeparatorText("Proxy");
        bool proxy_changed = ImGuiX::Widgets::ProxyPanel("auth.proxy", m_auth.proxy_cfg, m_auth.proxy);
        if (proxy_changed) {
            // TODO: применить настройки к вашему сетевому слою
        }

        // ---
        ImGui::SeparatorText("Time pickers");
        bool t_changed  = ImGuiX::Widgets::TimePicker("auth.time", m_auth.time_of_day_sec, m_auth.tp_cfg);
        ImGui::SameLine();
        bool tz_changed = ImGuiX::Widgets::TimeOffsetPicker(
            "auth.tz", 
            m_auth.tz_offset_sec, 
            m_auth.has_dst_out, 
            m_auth.tz_index_io, 
            m_auth.to_cfg
        );

        if (t_changed || tz_changed) {
            // TODO: пересчитать расписание, применить к логике и т.п.
        }

        ImGui::SeparatorText("Date picker");
        ImGuiX::Widgets::DatePicker("auth.date", m_auth.date_ts, m_auth.dp_cfg);

        // ---
        ImGui::SeparatorText("Centered text demo");

        // Однострочный (форматируемый)
        ImGuiX::Widgets::TextCenteredFmt("Welcome, %s!", m_auth.auth_data.email.empty() ? "guest" : m_auth.auth_data.email.c_str());

        // Не форматируемый
        ImGuiX::Widgets::TextUnformattedCentered("This line is centered.");

        // Обёрнутый блок (ширину можно зафиксировать, чтобы текст не растягивался на всю строку)
        ImGuiX::Widgets::TextWrappedCentered(
            "This is a long message that demonstrates how wrapped text can be visually centered "
            "by placing it inside a centered child region.", 420.0f
        );
        
        // ---
        ImGui::SeparatorText("List editor");
        ImGuiX::Widgets::ListEditor("names", m_auth.names);
        ImGuiX::Widgets::ListEditor("numbers", m_auth.numbers);
    }

    void postLanguageChange(const std::string& lang, bool apply_to_all = true) {
        notify(ImGuiX::Events::LangChangeEvent(lang, apply_to_all, window().id()));
    }
};

class I18nWindow : public ImGuiX::WindowInstance {
public:
    I18nWindow(int id, 
               ImGuiX::ApplicationContext& app, 
               std::string name)
        : WindowInstance(id, app, std::move(name)) {}

    void onInit() override {
        createController<I18nController>();
        create(id() == 0 ? 800 : 640, id() == 0 ? 600 : 480);
        setWindowIcon("data/resources/icons/icon.png");
        fontsBeginManual();
        fontsSetRangesPreset("Default+Cyrillic+Vietnamese+Punct+PUA");
        fontsAddBody({ "NotoSans-Regular.ttf", 18.0f });
        fontsAddMerge(ImGuiX::Fonts::FontRole::Icons, { "MaterialIcons-Regular.ttf", 18.0f, 4.0f, true});
        fontsAddHeadline(ImGuiX::Fonts::FontRole::H1, { "NotoSans-Bold.ttf", 24.0f });
        fontsBuildNow();
    }
};

int main() {
    ImGuiX::Application app;
    app.createWindow<I18nWindow>("Main Window");
    app.run();
    return 0;
}