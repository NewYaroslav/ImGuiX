#include <iostream>
#include <imguix/core.hpp>

// === Core UI: Ввод, валидация, списки, текст ===
#include <imguix/widgets/validated_input.hpp>
#include <imguix/widgets/validated_password_input.hpp>
#include <imguix/widgets/input_vertical_stepper.hpp>
#include <imguix/widgets/text_center.hpp>
#include <imguix/widgets/list_editor.hpp>

// === Авторизация и связанные панели ===
#include <imguix/widgets/auth_panel.hpp>
#include <imguix/widgets/auth_js_panel.hpp>
#include <imguix/widgets/domain_selector.hpp>
#include <imguix/widgets/virtual_keyboard.hpp>

// === Время, дата, часы ===
#include <imguix/widgets/time_picker.hpp>
#include <imguix/widgets/date_picker.hpp>
#include <imguix/widgets/hours_selector.hpp>
#include <imguix/widgets/days_selector.hpp>

// === Дополнительно: прокси, спиннер, маркеры ===
#include <imguix/widgets/proxy_panel.hpp>
#include <imguix/widgets/loading_spinner.hpp>
#include <imguix/widgets/markers.hpp>

namespace i18n = ImGuiX::I18N;

// Контроллер окна демо-виджетов
class I18nController : public ImGuiX::Controller {
public:
    I18nController(ImGuiX::WindowInterface& window)
        : Controller(window) {}

    void drawContent() override {
        // пространство для фонового рендера; всё демо — в drawUi()
    }

    void drawUi() override {
        ImGui::PushID(window().id());
        ImGui::PushFont(nullptr, 18.0f); // Размер шрифта (обёртка ImGuiX)
        ImGui::Begin(window().id() == 0 ? "i18n main" : "i18n child");

        ImGui::Separator();
        drawWidgetsDemo();

        ImGui::End();
        ImGui::PopFont();
        ImGui::PopID();
    }

private:
    // --- служебные поля контроллера (пример, если пригодятся) ---
    std::string m_last_font_lang;
    int         m_items_count{1};
    int         m_notif_count{2};

    // Состояние демо: хранит конфиги и данные всех виджетов
    struct WidgetsDemoState {
        // ------------------ 1) Авторизация и вход ------------------
        ImGuiX::Widgets::AuthPanelConfig auth_cfg{};   // Конфиг панели логина
        ImGuiX::Widgets::AuthData        auth_data;    // Данные входа (email/password/host)
        ImGuiX::Widgets::AuthJsSettings  js_st{};      // Настройки JS-панели

        // ------------------ 2) Виртуальные клавиатуры ------------------
        ImGuiX::Widgets::VirtualKeyboardConfig kcfg;   // Общий конфиг экранной клавиатуры
        bool kbd_email = false; // Тогглер VK для email
        bool kbd_pass  = false; // Тогглер VK для password
        bool kbd_host  = false; // Тогглер VK для host

        // ------------------ 3) Домен и прокси ------------------
        ImGuiX::Widgets::DomainSelectorConfig dom_cfg{};      // Конфиг выбора домена
        ImGuiX::Widgets::ProxySettings        proxy{};        // Настройки прокси
        ImGuiX::Widgets::ProxyPanelConfig     proxy_cfg{};    // Конфиг панели прокси

        // ------------------ 4) Часы (рабочие периоды) ------------------
        std::vector<int>                     hours{9,10,11,12,13,14,15,16,17}; // по умолчанию 9-17
        ImGuiX::Widgets::HoursSelectorConfig hs_cfg{};                          // Конфиг выбора часов

        // ------------------ 5) Время и смещение ------------------
        int      time_of_day_sec = 18 * 3600; // 18:00:00
        int64_t  tz_offset_sec   = 2 * 3600;  // +02:00:00
        bool     has_dst_out     = false;     // DST наблюдается (для выбора TZ)
        int      tz_index_io     = 0;         // Индекс текущей TZ

        ImGuiX::Widgets::TimePickerConfig       tp_cfg{}; // Конфиг выбора времени
        ImGuiX::Widgets::TimeOffsetPickerConfig to_cfg{}; // Конфиг выбора смещения

        // ------------------ 6) Дата ------------------
        int64_t                                  date_ts = 0; // Unix-эпоха (сек)
        ImGuiX::Widgets::DatePickerConfig        dp_cfg{};    // Конфиг выбора даты

        // ------------------ 7) Разное ------------------
        ImGuiX::Widgets::LoadingSpinnerConfig sp_cfg{}; // Конфиг спиннера
        std::vector<std::string>              names{"Alice", "Bob"};
        std::vector<int>                      numbers{1, 2, 3};

        // ------------------ Конструктор: дефолты ------------------
        WidgetsDemoState() {
            // --- Auth panel defaults ---
            auth_cfg.header                = "Login";
            auth_cfg.hint_email            = "email";
            auth_cfg.hint_password         = "password";
            auth_cfg.hint_host             = "host";
            auth_cfg.connected_label       = "connected";
            auth_cfg.connect_label         = "connect";
            auth_cfg.show_host             = true;
            auth_cfg.show_connection_state = true;
            auth_cfg.validate_email        = true;
            auth_cfg.init                  = true;
            auth_cfg.connected             = false;

            // Виртуальная клавиатура для полей логина
            auth_cfg.vk_host               = true;
            auth_cfg.vk_email              = true;
            auth_cfg.vk_password           = true;

            auth_cfg.vk_cfg.submit_on_enter     = true;
            auth_cfg.vk_cfg.capture_hw_enter    = true;    // захват физического Enter
            auth_cfg.vk_cfg.shift_enter_newline = true;    // Shift+Enter = перенос строки
            auth_cfg.vk_cfg.border              = false;
            auth_cfg.vk_cfg.on_submit = [](const std::string& /*text*/) {
                // Пользовательский submit (опционально)
            };

            // Кнопка "connect" (демо: просто переключает флаг)
            auth_cfg.on_connect = [this]() {
                auth_cfg.connected = !auth_cfg.connected;
            };

            // --- Auth data defaults ---
            auth_data.email    = "guest@example.com";
            auth_data.password = "";
            auth_data.host     = "demo.local";

            // --- VK config defaults ---
            kcfg.size               = ImVec2(0, 0); // авторазмер
            kcfg.locales            = {
                "en", "ru", "es", "pt-BR", "id",
                "uk", "be", "pt", "de", "fr", "it", "pl", "tr",
                "vi", "th", "ar", "hi", "ur"
            };
            kcfg.start_locale_index = 0;
            kcfg.show_locale_combo  = true;
            kcfg.key_size           = ImVec2(26, 26);
            kcfg.spacing            = 4.0f;
            kcfg.show_top_preview   = false;

            // --- Domain selector defaults ---
            dom_cfg.header         = "Server";
            dom_cfg.hint_domain    = "domain";
            dom_cfg.custom_text    = "Custom";
            dom_cfg.default_domain = auth_data.host; // при переходе к "Custom" сохраняем текущий
            dom_cfg.show_help      = true;
            dom_cfg.help_text      = "Choose predefined server or type custom host.";
            dom_cfg.domains        = { "demo.local", "test.example.com" };

            // --- Proxy defaults ---
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
            proxy_cfg.on_check = [this]() {
                // демо-поведение: отмечаем как "проверено" и чередуем успех/провал
                proxy_cfg.checked  = true;
                proxy_cfg.check_ok = !proxy_cfg.check_ok;
            };

            // --- Hours selector defaults ---
            hs_cfg.label       = "Hours";
            hs_cfg.empty_hint  = "none";
            // hs_cfg.popup_size  = ImVec2(260, 110);
            // hs_cfg.cell_size   = ImVec2(18, 18);
            // hs_cfg.rows        = 3;
            // hs_cfg.cols        = 8;
            // hs_cfg.combo_width = 260.0f;

            // --- Time pickers defaults ---
            tp_cfg.label = "Time of day";
            tp_cfg.desc  = "HH:MM:SS";

            to_cfg.label    = "GMT offset";
            to_cfg.desc     = "±HH:MM:SS";
            to_cfg.show_gmt = true;

            // dp_cfg (конфиг даты) можно настроить в UI-секции Date Picker
        }
    };

    WidgetsDemoState m_state{};

    // Рендер всей демо-витрины виджетов (сгруппировано под спойлеры)
    void drawWidgetsDemo() {
        ImGui::SeparatorText("Widgets Demo");

        // --- Authorization block ---
        if (ImGui::CollapsingHeader("Authorization", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Панель логина: email/password/host
            ImGuiX::Widgets::AuthPanel(
                "login.panel", m_state.auth_cfg, m_state.auth_data);

            // API key/secret — отдельная панель без кнопки connect
            ImGuiX::Widgets::AuthPanelConfig cfg_api{};
            cfg_api.header              = u8"Authorization (API key)";
            cfg_api.show_api_keys       = true;
            cfg_api.vk_api_key          = true;
            cfg_api.vk_api_secret       = true;
            cfg_api.show_connect_button = false;
            ImGuiX::Widgets::AuthPanel("login.api", cfg_api, m_state.auth_data);
        }

        // --- Validated inputs block ---
        if (ImGui::CollapsingHeader("Validated Inputs")) {
            bool email_valid   = false;
            bool api_key_valid = false;

            // Тогглер для экранной клавиатуры в полях ввода
            ImGuiX::Widgets::KeyboardToggleConfig kb_cfg;
            kb_cfg.icon_text           = u8"\uE312";
            kb_cfg.tooltip_toggle_on   = u8"Show keyboard";
            kb_cfg.tooltip_toggle_off  = u8"Hide keyboard";

            // Email с VK и валидацией RegEx
            ImGuiX::Widgets::InputTextWithVKValidated(
                "email##validated",
                "email",
                m_state.auth_data.email,
                true,
                ImGuiX::Widgets::InputValidatePolicy::OnTouch,
                R"(.+@.+\.\w+)",
                email_valid,
                kb_cfg
            );

            // API Key (валидируемый)
            ImGuiX::Widgets::InputTextValidated(
                "apikey##validated",
                "api key (public)",
                m_state.auth_data.api_key,
                true,
                ImGuiX::Widgets::InputValidatePolicy::OnTouch,
                R"(^[A-Za-z0-9.\-:]+$)",
                api_key_valid
            );

            // Пароль/кей с toggle + VK
            ImGuiX::Widgets::PasswordToggleConfig toggle_cfg;
            ImGuiX::Widgets::InputPasswordWithToggleVK(
                "apikey##togglevk",
                "api key (public)",
                m_state.auth_data.api_key,
                true,
                ImGuiX::Widgets::InputValidatePolicy::OnTouch,
                R"(^[A-Za-z0-9.\-:]+$)",
                api_key_valid,
                toggle_cfg,
                kb_cfg
            );
        }

        // --- Status block ---
        if (ImGui::CollapsingHeader("Status Info")) {
            ImGui::Text("Connection: %s", m_state.auth_cfg.connected ? "connected" : "disconnected");
            ImGui::Text("Email Valid: %s", m_state.auth_data.email_valid ? "yes" : "no");
            ImGui::Text("API Key: %s", m_state.auth_data.api_key.c_str());
            ImGui::Text("API Secret: %s", m_state.auth_data.api_secret.c_str());
        }

        // --- Virtual keyboards ---
        if (ImGui::CollapsingHeader("Virtual Keyboards")) {
            ImGui::Checkbox("Email VK",    &m_state.kbd_email); ImGui::SameLine();
            ImGui::Checkbox("Password VK", &m_state.kbd_pass);  ImGui::SameLine();
            ImGui::Checkbox("Host VK",     &m_state.kbd_host);

            if (m_state.kbd_email)
                ImGuiX::Widgets::VirtualKeyboard("vk.email", m_state.auth_data.email,    m_state.kcfg);
            if (m_state.kbd_pass)
                ImGuiX::Widgets::VirtualKeyboard("vk.pass",  m_state.auth_data.password, m_state.kcfg);
            if (m_state.kbd_host)
                ImGuiX::Widgets::VirtualKeyboard("vk.host",  m_state.auth_data.host,     m_state.kcfg);
        }

        // --- Domain selector ---
        if (ImGui::CollapsingHeader("Domain Selector")) {
            if (ImGuiX::Widgets::DomainSelector("domain.selector", m_state.dom_cfg, m_state.auth_data.host)) {
                // Реакция на изменение хоста при необходимости
            }
        }

        // --- JS Auth panel ---
        if (ImGui::CollapsingHeader("JS Panel")) {
            ImGuiX::Widgets::AuthJsPanelConfig js_cfg{};
            ImGuiX::Widgets::AuthJsPanel("js.panel", js_cfg, m_state.js_st);
        }

        // --- Proxy Settings ---
        if (ImGui::CollapsingHeader("Proxy Settings")) {
            if (ImGuiX::Widgets::ProxyPanel("proxy.panel", m_state.proxy_cfg, m_state.proxy)) {
                // применить настройки к сетевому слою при необходимости
            }
        }

        // --- Hours Selector ---
        if (ImGui::CollapsingHeader("Hours Selector")) {
            if (ImGui::SmallButton("Work 9–18")) {
                m_state.hours.clear();
                for (int h = 9; h <= 18; ++h) m_state.hours.push_back(h);
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("Night 0–6")) {
                m_state.hours.clear();
                for (int h = 0; h <= 6; ++h) m_state.hours.push_back(h);
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("Even hours")) {
                m_state.hours.clear();
                for (int h = 0; h < 24; h += 2) m_state.hours.push_back(h);
            }

            bool changed = ImGuiX::Widgets::HoursSelector("hours", m_state.hours, m_state.hs_cfg);
            ImGui::SameLine();
            ImGui::TextDisabled("(%d selected)", (int)m_state.hours.size());
            if (changed) {
                // обработать изменения часов
            }
        }

        // --- WeekDays Selector ---
        if (ImGui::CollapsingHeader("WeekDays Selector")) {
            static std::vector<int> selected_days; // выбранные дни (0=Mon..6=Sun)

            ImGuiX::Widgets::DaysSelectorConfig cfg;
            cfg.label       = u8"Working days";
            //cfg.combo_width = 180.0f;
            cfg.short_names = { u8"Пн", u8"Вт", u8"Ср", u8"Чт", u8"Пт", u8"Сб", u8"Вс" };
            if (ImGuiX::Widgets::DaysOfWeekSelector("##days_selector", selected_days, cfg)) {
                // обработать выбранные дни
            }
        }

        // --- Time & Offset ---
        if (ImGui::CollapsingHeader("Time & Offset")) {
            static int h = 14, m = 30, s = 0;
            ImGuiX::Widgets::TimePickerConfig simple_tp;
            simple_tp.label = "Time (H/M/S)";
            ImGuiX::Widgets::TimePicker("simple.time", h, m, s, simple_tp);

            bool t_changed = ImGuiX::Widgets::TimePicker(
                "time", m_state.time_of_day_sec, m_state.tp_cfg);

            ImGui::SameLine();

            bool tz_changed = ImGuiX::Widgets::TimeOffsetPicker(
                "offset",
                m_state.tz_offset_sec,
                m_state.has_dst_out,
                m_state.tz_index_io,
                m_state.to_cfg
            );

            if (t_changed || tz_changed) {
                // обработать новое время/смещение
            }
        }

        // --- Date Picker ---
        if (ImGui::CollapsingHeader("Date Picker")) {
            // Выбор по компонентам (год/месяц/день)
            static int64_t y = 2025;
            static int     m = 8, d = 25;
            ImGuiX::Widgets::DatePickerConfig dc;
            dc.label        = "Date";
            dc.show_weekday = true;
            dc.min_year     = 1950;
            dc.max_year     = 2100;
            ImGuiX::Widgets::DatePicker("date_struct", y, m, d, dc);

            // Выбор по unix timestamp (UTC)
            static int64_t ts = 0;
            ImGuiX::Widgets::DatePickerConfig dc_ts;
            dc_ts.label               = "Date (timestamp)";
            dc_ts.show_weekday        = true;
            dc_ts.preserve_time_of_day = false; // привязка к 00:00:00
            ImGuiX::Widgets::DatePicker("date_ts", ts, dc_ts);
        }

		// --- Markers ---
		if (ImGui::CollapsingHeader("Markers")) {
			// 1) Цветные маркеры (бейджи)
			ImGui::TextDisabled("Colored markers:");
			ImGuiX::Widgets::ColoredMarker("OK",          "All good",                  ImVec4(0.10f, 0.75f, 0.30f, 1.0f));
			ImGui::SameLine();
			ImGuiX::Widgets::ColoredMarker("WARNING",     "Spread high",               ImVec4(0.95f, 0.75f, 0.10f, 1.0f));
			ImGui::SameLine();
			ImGuiX::Widgets::ColoredMarker("ERROR",       "Disconnected",              ImVec4(0.95f, 0.25f, 0.25f, 1.0f));
			ImGui::SameLine();
			ImGuiX::Widgets::ColoredMarker("SIMULATION",  "Demo / paper trading",      ImVec4(0.70f, 0.70f, 0.80f, 1.0f));

			ImGui::Separator();

			// 2) Message markers (иконка с тултипом/текстом)
			ImGui::TextDisabled("Message markers:");
			// Help
			ImGui::TextUnformatted("Help:"); 
			ImGui::SameLine();
			ImGuiX::Widgets::HelpMarker("Trading server to place real orders.\nUse with care.");

			// Info
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 2);
			ImGui::TextUnformatted("Info:"); 
			ImGui::SameLine();
			ImGuiX::Widgets::InfoMarker("New version available: v1.4.2. Update when idle.");

			// Warning
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 2);
			ImGui::TextUnformatted("Warn:"); 
			ImGui::SameLine();
			ImGuiX::Widgets::WarningMarker("Funding API rate-limit almost reached; consider backoff.");

			// Success (иконка + инлайн-вариант)
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x * 2);
			ImGui::TextUnformatted("Success:");
			ImGui::SameLine();
			ImGuiX::Widgets::SuccessMarker("License validated. All features enabled.");
			// Инлайн-текст рядом с иконкой (без тултипа)
			ImGuiX::Widgets::SuccessMarker("License validated.", ImGuiX::Widgets::MarkerMode::InlineText);

			ImGui::Separator();

			// (опционально) Пресеты с SelectableMarker
			ImGui::TextDisabled("Selectable markers:");
			static std::vector<std::string> presets = {
				"London Session", "New York Session", "Asia Session", "Custom Range"
			};
			static int last_clicked = -1;
			for (int i = 0; i < (int)presets.size(); ++i) {
				if (ImGuiX::Widgets::SelectableMarker(presets[i])) {
					last_clicked = i;
				}
			}
			ImGui::Separator();
			ImGui::Text("Last clicked: %s", (last_clicked >= 0 ? presets[last_clicked].c_str() : "none"));
		}

        // --- Spinner ---
        if (ImGui::CollapsingHeader("Loading Spinner")) {
            ImGuiX::Widgets::LoadingSpinner("spinner", m_state.sp_cfg);
            ImGui::SliderFloat("radius",    &m_state.sp_cfg.radius,        2.0f, 48.0f, "%.0f");
            ImGui::SliderFloat("thickness", &m_state.sp_cfg.thickness,     1.0f, 12.0f, "%.1f");
            ImGui::SliderInt  ("segments",  &m_state.sp_cfg.segments,         8, 128);
            ImGui::SliderFloat("sweep",     &m_state.sp_cfg.sweep_ratio,   0.1f, 1.0f, "%.2f");
            ImGui::SliderFloat("speed",     &m_state.sp_cfg.angular_speed, 0.0f, 20.0f, "%.1f");
        }

        // --- Centered Text ---
        if (ImGui::CollapsingHeader("Text Centering")) {
            ImGuiX::Widgets::TextCenteredFmt(
                "Welcome, %s!", m_state.auth_data.email.empty() ? "guest" : m_state.auth_data.email.c_str());
            ImGuiX::Widgets::TextUnformattedCentered("This line is centered.");
            ImGuiX::Widgets::TextWrappedCentered(
                "This is a long message that demonstrates how wrapped text can be visually centered "
                "by placing it inside a centered child region.",
                420.0f
            );
        }

        // --- List Editors ---
        if (ImGui::CollapsingHeader("List Editors")) {
            ImGuiX::Widgets::ListEditor("list.names",   "Names",   m_state.names);
            ImGuiX::Widgets::ListEditor("list.numbers", "Numbers", m_state.numbers);
        }
    }

    void postLanguageChange(const std::string& lang, bool apply_to_all = true) {
        notify(ImGuiX::Events::LangChangeEvent(lang, apply_to_all, window().id()));
    }
};

// Окно-приложение: шрифты, размеры, инициализация контроллера
class I18nWindow : public ImGuiX::WindowInstance {
public:
    I18nWindow(int id,
               ImGuiX::ApplicationContext& app,
               std::string name)
        : WindowInstance(id, app, std::move(name)) {}

    void onInit() override {
        createController<I18nController>();

        // Размеры основного/дочернего окна
        create(id() == 0 ? 800 : 640, id() == 0 ? 600 : 480);

        // Иконка окна
        setWindowIcon("data/resources/icons/icon.png");

        // Шрифты (ручная сборка)
        fontsBeginManual();
        fontsSetRangesPreset("Default+Cyrillic+Vietnamese+Punct+PUA+LatinExtA");
        fontsAddBody({ "NotoSans-Regular.ttf", 18.0f });
        fontsAddMerge(ImGuiX::Fonts::FontRole::Icons, { "MaterialIcons-Regular.ttf", 18.0f, 4.0f, true });
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
