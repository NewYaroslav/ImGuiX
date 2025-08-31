#include <iostream>
#include <imguix/core.hpp>
#include <random>
#include <cmath>

// === Core UI: Ввод, валидация, списки, текст ===
#include <imguix/widgets/input/validated_input.hpp>
#include <imguix/widgets/input/validated_password_input.hpp>
#include <imguix/widgets/input/input_vertical_stepper.hpp>
#include <imguix/widgets/misc/text_center.hpp>
#include <imguix/widgets/input/list_editor.hpp>

// === Авторизация и связанные панели ===
#include <imguix/widgets/auth/auth_panel.hpp>
#include <imguix/widgets/auth/auth_js_panel.hpp>
#include <imguix/widgets/auth/domain_selector.hpp>
#include <imguix/widgets/input/virtual_keyboard.hpp>

// === Время, дата, часы ===
#include <imguix/widgets/time/time_picker.hpp>
#include <imguix/widgets/time/date_picker.hpp>
#include <imguix/widgets/time/hours_selector.hpp>
#include <imguix/widgets/time/days_selector.hpp>

// === Уведомления ===
#include <imguix/widgets/notify/notifications.hpp>

// === Дополнительно: прокси, спиннер, маркеры ===
#include <imguix/widgets/auth/proxy_panel.hpp>
#include <imguix/widgets/misc/loading_spinner.hpp>
#include <imguix/widgets/misc/markers.hpp>
#include <imguix/widgets/misc/theme_picker.hpp>

// === Themes ===
#include <imguix/themes/CorporateGreyTheme.hpp>
#include <imguix/themes/DarkCharcoalTheme.hpp>
#include <imguix/themes/DarkGraphiteTheme.hpp>
#include <imguix/themes/DarkTealTheme.hpp>
#include <imguix/themes/DeepDarkTheme.hpp>
#include <imguix/themes/GoldBlackTheme.hpp>
#include <imguix/themes/GreenBlueTheme.hpp>
#include <imguix/themes/LightBlueTheme.hpp>
#include <imguix/themes/LightGreenTheme.hpp>
#include <imguix/themes/OSXTheme.hpp>
#include <imguix/themes/PearlLightTheme.hpp>
#include <imguix/themes/SlateDarkTheme.hpp>
#include <imguix/themes/VisualStudioDarkTheme.hpp>
#include <imguix/themes/Y2KTheme.hpp>
#include <imguix/themes/CyberY2KTheme.hpp>

// === Графики ===
#ifdef IMGUI_ENABLE_IMPLOT
#include <imguix/widgets/plot/PlotOHLCChart.hpp>
#endif

#ifdef IMGUI_ENABLE_IMNODEFLOW
#include <ImNodeFlow.h>   // ImFlow::Editor, BaseNode, etc.
#endif

#ifdef IMGUI_ENABLE_IMGUIFILEDIALOG
#include <ImGuiFileDialog.h>
#endif

#ifdef IMGUI_ENABLE_PFD
#include <portable-file-dialogs.h>
#endif

#ifdef IMGUI_ENABLE_IMSPINNER
#include <imspinner.h>
#endif

#ifdef IMGUI_ENABLE_IMPLOT
/// \brief Generate synthetic OHLCV bars.
/// \param out Output bar container.
/// \param count Number of bars to produce.
/// \param tf_sec Timeframe in seconds.
/// \param start_time Starting timestamp.
/// \param start_price Starting price.
/// \param seed Random seed.
inline static void GenerateBars(
        std::vector<ImGuiX::Widgets::OhlcvBar>& out,
        int count, int tf_sec, std::uint64_t start_time,
        double start_price, unsigned seed
    ) {
    using BarT = ImGuiX::Widgets::OhlcvBar;
    out.clear();
    out.reserve(std::max(0, count));

    const std::uint64_t tf_ms = static_cast<std::uint64_t>(tf_sec);

    std::mt19937 rng(seed);
    std::normal_distribution<double> step(0.0, 0.002);      // ~0.2% шаг
    std::normal_distribution<double> wick(0.0, 0.0008);     // фитили
    std::lognormal_distribution<double> voln(0.0, 0.25);    // шум для объёма

    double prev_close = std::max(0.1, start_price);

    for (int i = 0; i < count; ++i) {
        BarT b;
        b.time  = start_time + static_cast<std::uint64_t>(i) * tf_sec;

        const double drift = 0.00002 * tf_sec;              // лёгкий дрейф по ТФ
        const double ch    = (drift + step(rng)) * prev_close;

        b.open  = prev_close;
        b.close = std::max(0.01, b.open + ch);

        // фитили на основе диапазона бара
        const double base_range = std::max(1e-6, std::abs(b.close - b.open));
        const double up_wick    = std::max(0.0, (base_range * 0.5) + wick(rng) * prev_close);
        const double dn_wick    = std::max(0.0, (base_range * 0.5) + wick(rng) * prev_close);

        b.high  = std::max({b.open, b.close}) + up_wick;
        b.low   = std::max(0.01, std::min({b.open, b.close}) - dn_wick);

        // объём: базовый + пропорционален диапазону
        const double range = b.high - b.low;
        b.volume = std::max(0.0, 800.0 + 50000.0 * range / std::max(1e-6, b.close) * voln(rng));

        out.push_back(b);
        prev_close = b.close;
    }
}
#endif

#ifdef IMGUI_ENABLE_IMNODEFLOW
class SimpleSum final : public ImFlow::BaseNode {
public:
    SimpleSum() {
        setTitle("Simple sum");
        setStyle(ImFlow::NodeStyle::green());

        // Фильтр ставим на ВХОД
        addIN<int>("A", 0, ImFlow::ConnectionFilter::SameType());

        // ВЫХОД: во второй параметр передаём стиль пина (или nullptr)
        addOUT<int>("Result", nullptr)
            ->behaviour([this]() { return getInVal<int>("A") + b_; });
    }

    void draw() override {
        ImGui::SetNextItemWidth(100.0f);
        ImGui::InputInt("B", &b_);
    }
private:
    int b_ = 0;
};

// Редактор/хэндлер графа
static ImFlow::ImNodeFlow g_editor;

void DrawNodeEditorFrame() {
    static bool inited = false;
    if (!inited) {
        // Создаём узел на сцене (позиция в пикселях экранных координат)
        g_editor.placeNodeAt<SimpleSum>(ImVec2(120, 80));
        inited = true;
    }

    // Рисуем редактор внутри окна ImGui
    ImGui::Begin("Node Editor");
    g_editor.update();           // <-- вместо g_editor.draw(...)
    ImGui::End();
}
#endif

// Контроллер окна демо-виджетов
class WidgetsController : public ImGuiX::Controller {
public:
    WidgetsController(ImGuiX::WindowInterface& window)
        : Controller(window) {
        m_state.auth_data.host     = options().getStrOr("host", "demo.local");
        m_state.auth_data.email    = options().getStrOr("email", "guest@example.com");
        m_state.auth_data.password = options().getStrOr("password", "");
        ImGuiX::Widgets::ApplyStoredTheme(this);
    }

    void drawContent() override {
        // пространство для фонового рендера; всё демо — в drawUi()
    }

    void drawUi() override {
        ImGui::PushID(window().id());
        ImGui::PushFont(nullptr, 18.0f); // Размер шрифта (обёртка ImGuiX)
        static bool show_imgui_demo = false;

#       ifdef IMGUI_ENABLE_IMPLOT
        static bool show_implot_demo = false;
#       endif

#       ifdef IMGUI_ENABLE_IMPLOT3D
        static bool show_implot3d_demo = false;
#       endif

#       ifdef IMGUI_ENABLE_IMNODEFLOW
        static bool show_imnodeflow_demo = false;
#       endif

#       ifdef IMGUI_ENABLE_IMGUIFILEDIALOG
        static bool show_igfd = false;
#       endif

#       ifdef IMGUI_ENABLE_PFD
        static bool show_pfd = false;
#       endif

#       ifdef IMGUI_ENABLE_IMSPINNER
        static bool show_imspinner_demo = false;
#       endif

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Demo")) {
                if (ImGui::MenuItem("ImGui", nullptr, false, !show_imgui_demo)) {
                    show_imgui_demo = true;
                }

#               ifdef IMGUI_ENABLE_IMPLOT
                if (ImGui::MenuItem("ImPlot", nullptr, false, !show_implot_demo)) {
                    show_implot_demo = true;
                }
#               endif

#               ifdef IMGUI_ENABLE_IMPLOT3D
                if (ImGui::MenuItem("ImPlot3D", nullptr, false, !show_implot3d_demo)) {
                    show_implot3d_demo = true;
                }
#               endif

#               ifdef IMGUI_ENABLE_IMNODEFLOW
                if (ImGui::MenuItem("ImNodeFlow", nullptr, false, !show_imnodeflow_demo)) {
                    show_imnodeflow_demo = true;
                }
#               endif

#               ifdef IMGUI_ENABLE_IMGUIFILEDIALOG
                if (ImGui::MenuItem("File Dialog", nullptr, false, !show_igfd)) {
                    show_igfd = true;
                }
#               endif

#               ifdef IMGUI_ENABLE_PFD
                if (ImGui::MenuItem("File Dialog (pfd)", nullptr, false, !show_pfd)) show_pfd = true;
#               endif

#               ifdef IMGUI_ENABLE_IMSPINNER
                if (ImGui::MenuItem("ImSpinner", nullptr, false, !show_imspinner_demo))
                    show_imspinner_demo = true;
#               endif
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        ImGui::Begin("Widgets Demo");

        ImGui::Separator();
        drawWidgetsDemo();

        ImGui::End();
        if (show_imgui_demo) ImGui::ShowDemoWindow(&show_imgui_demo);

#       ifdef IMGUI_ENABLE_IMPLOT
        if (show_implot_demo) ImPlot::ShowDemoWindow(&show_implot_demo);
#       endif

#       ifdef IMGUI_ENABLE_IMPLOT3D
        if (show_implot3d_demo) ImPlot3D::ShowDemoWindow(&show_implot3d_demo);
#       endif

#       ifdef IMGUI_ENABLE_IMNODEFLOW
        if (show_imnodeflow_demo) {
            ImGui::Begin("ImNodeFlow Demo", &show_imnodeflow_demo);
            DrawNodeEditorFrame();              // <- твоя функция с g_editor.draw(...)
            ImGui::End();
        }
#       endif

#       ifdef IMGUI_ENABLE_IMGUIFILEDIALOG
        if (show_igfd) {
            ImGui::Begin("ImGuiFileDialog", &show_igfd);
            if (ImGui::Button("Open...")) {
                IGFD::FileDialogConfig cfg;   // путь/фильтры см. README
                cfg.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog("OpenDlg", "Open File", ".*", cfg);
            }
            if (ImGuiFileDialog::Instance()->Display("OpenDlg")) {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    const std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
                    // TODO: использовать path
                }
                ImGuiFileDialog::Instance()->Close();
            }
            ImGui::End();
        }
#       endif

#       ifdef IMGUI_ENABLE_PFD
        if (show_pfd) {
            ImGui::Begin("portable-file-dialogs", &show_pfd);

            static std::string last_open, last_save;
            if (ImGui::Button("Open...")) {
                auto sel = pfd::open_file("Open file",
                                          ".",            // стартовая папка
                                          { "All files", "*" },
                                          pfd::opt::multiselect).result();
                if (!sel.empty()) last_open = sel.front(); // возьми первый, либо перечисли все
            }
            ImGui::SameLine();
            ImGui::TextDisabled("%s", last_open.c_str());

            if (ImGui::Button("Save As...")) {
                last_save = pfd::save_file("Save file", ".", { "Text", "*.txt", "All files", "*" }).result();
            }
            ImGui::SameLine();
            ImGui::TextDisabled("%s", last_save.c_str());

            if (ImGui::Button("Message box")) {
                pfd::message("Notice", "Operation finished.", pfd::choice::ok, pfd::icon::info).result();
            }

            ImGui::End();
        }
#       endif

#ifdef IMGUI_ENABLE_IMSPINNER
        if (show_imspinner_demo) {
            ImGui::Begin("ImSpinner Demo", &show_imspinner_demo);
#           ifdef IMSPINNER_DEMO
            ImSpinner::demoSpinners();   // демо из README
#           else
            ImGui::TextDisabled("Build without IMSPINNER_DEMO");
#           endif
            ImGui::End();
        }
#endif

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
        
#       ifdef IMGUI_ENABLE_IMPLOT
        // ------------------ 8) OHLC Bars Plot (demo) ------------------
        int  tf_index = 0;            // 0=M1, 1=M30, 2=H1
        int  bars_count = 300;        // сколько баров генерить
        unsigned rng_seed = 42;       // зерно генератора
        double start_price = 100.0;   // базовая цена
        // Данные для трёх ТФ (время — миллисекунды)
        std::vector<ImGuiX::Widgets::OhlcvBar> bars_m1;
        std::vector<ImGuiX::Widgets::OhlcvBar> bars_m30;
        std::vector<ImGuiX::Widgets::OhlcvBar> bars_h1;

        ImGuiX::Widgets::OHLCChartConfig ohlc_cfg{}; // конфиг виджета графика
#       endif

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

        // --- Theme Picker ---
        if (ImGui::CollapsingHeader("Theme", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGuiX::Widgets::ThemePicker("demo.theme", this);
        }

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
            const auto res = ImGuiX::Widgets::AuthPanel("login.api", cfg_api, m_state.auth_data);
            using E = ImGuiX::Widgets::AuthPanelResult;
            if (Has(res, E::HostChanged) && m_state.auth_data.host_valid) {
                options().setStr("host", m_state.auth_data.host);
            }
            if (Has(res, E::EmailChanged) && m_state.auth_data.email_valid) {
                options().setStr("email", m_state.auth_data.email);
            }
            if (Has(res, E::PasswordChanged)) {
                options().setStr("password", m_state.auth_data.password);
            }
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
        
        // --- Notifications ---
        if (ImGui::CollapsingHeader("Notifications", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Примеры без заголовка (аналог оригинала)
            if (ImGui::Button("Success")) {
                ImGuiX::Widgets::NotifyFmt(
                    this, ImGuiX::Notify::Type::Success, 0,
                    u8"That is a success! {}", "(Format here)"
                );
            }
            ImGui::SameLine();
            if (ImGui::Button("Warning")) {
                ImGuiX::Widgets::NotifyFmt(
                    this, ImGuiX::Notify::Type::Warning, 0,
                    u8"This is a warning!"
                );
            }
            ImGui::SameLine();
            if (ImGui::Button("Error")) {
                ImGuiX::Widgets::NotifyFmt(
                    this, ImGuiX::Notify::Type::Error, 0,
                    u8"Segmentation fault"
                );
            }
            ImGui::SameLine();
            if (ImGui::Button("Info")) {
                ImGuiX::Widgets::NotifyFmt(
                    this, ImGuiX::Notify::Type::Info, 0,
                    u8"Info about ImGui..."
                );
            }
            ImGui::SameLine();
            if (ImGui::Button("Info long")) {
                ImGuiX::Widgets::NotifyFmt(
                    this, ImGuiX::Notify::Type::Info, 0,
                    u8"Hi, I'm a long notification. I'm here to show you that you can write a lot of text in me. "
                    u8"I'm also here to show you that I can wrap text, so you don't have to worry about that."
                );
            }
            ImGui::SameLine();
            if (ImGui::Button("Notify with button")) {
                ImGuiX::Widgets::NotifyWithButtonFmt(
                    this,
                    ImGuiX::Notify::Type::Error,
                    0,
                    u8"Click me!",
                    // on_click: постим «успех»
                    [this]() {
                        ImGuiX::Widgets::NotifyFmt(
                            this, ImGuiX::Notify::Type::Success, 0,
                            u8"Thanks for clicking!"
                        );
                    },
                    // title, content
                    u8"Notification", "Content with action"
                );
            }

            ImGui::Separator();
            ImGui::TextDisabled(u8"Do it yourself:");

            // Поля ввода, как в оригинале
            static char title_buf[4096]   = u8"Hello there!";
            static char content_buf[4096] = u8"General Kenobi! \n- Grievous";
            ImGui::InputTextMultiline(u8"Title",   title_buf,   IM_ARRAYSIZE(title_buf));
            ImGui::InputTextMultiline(u8"Content", content_buf, IM_ARRAYSIZE(content_buf));

            static int duration_ms = 5000;
            ImGui::InputInt(u8"Duration (ms)", &duration_ms, 100);
            if (duration_ms < 0) duration_ms = 0;

            static const char* type_str[] = { u8"None", u8"Success", u8"Warning", u8"Error", u8"Info" };
            static ImGuiX::Notify::Type type = ImGuiX::Notify::Type::Success;
            if (ImGui::BeginCombo(u8"Type", type_str[static_cast<int>(type)])) {
                for (int n = 0; n < IM_ARRAYSIZE(type_str); ++n) {
                    const bool selected = (static_cast<int>(type) == n);
                    if (ImGui::Selectable(type_str[n], selected)) {
                        type = static_cast<ImGuiX::Notify::Type>(n);
                    }
                    if (selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            static bool enable_title = true, enable_content = true;
            ImGui::Checkbox(u8"Enable title", &enable_title);
            ImGui::SameLine();
            ImGui::Checkbox(u8"Enable content", &enable_content);

            if (ImGui::Button(u8"Show")) {
                ImGuiX::Notify::Notification toast(type, duration_ms);
                if (enable_title)   toast.setTitle("%s", title_buf);
                if (enable_content) toast.setContent("%s", content_buf);
                ImGuiX::Widgets::InsertNotification(this, std::move(toast));
            }
        }

#       ifdef IMGUI_ENABLE_IMPLOT
        // --- OHLC Bars / Plot ---
        if (ImGui::CollapsingHeader(u8"OHLC Bars / Plot")) {
            // ТФы: M1, M30, H1
            static const int TFs[] = {60, 30*60, 60*60};
            static const char* TF_NAMES[] = {"M1", "M30", "H1"};

            // Инициализация данных (разово)
            /*
            if (m_state.bars_m1.empty()) {
                const std::uint64_t start_time = 1700000000ULL;
                GenerateBars(m_state.bars_m1,  m_state.bars_count, TFs[0], start_time, m_state.start_price, m_state.rng_seed);
                GenerateBars(m_state.bars_m30, m_state.bars_count, TFs[1], start_time, m_state.start_price, m_state.rng_seed + 1);
                GenerateBars(m_state.bars_h1,  m_state.bars_count, TFs[2], start_time, m_state.start_price, m_state.rng_seed + 2);
            }
            */

            // Панель управления
            ImGui::PushItemWidth(160);
            ImGui::Combo("Timeframe", &m_state.tf_index, TF_NAMES, IM_ARRAYSIZE(TF_NAMES));
            ImGui::SameLine();
            ImGui::TextDisabled("(%d)", TFs[m_state.tf_index]);
            ImGui::PopItemWidth();

            ImGui::SliderInt("Bars", &m_state.bars_count, 50, 2000);
            //ImGui::SliderFloat("Start Price", &m_state.start_price, 1.0f, 10000.0f, "%.2f");
            double min_price = 1.0, max_price = 10000.0;
            ImGui::SliderScalar("Start Price", ImGuiDataType_Double, &m_state.start_price, &min_price, &max_price, "%.2f");
            ImGui::InputScalar("Seed", ImGuiDataType_U32, &m_state.rng_seed);

            if (ImGui::Button("Regenerate")) {
                const std::uint64_t start_time = 1700000000ULL;
                GenerateBars(m_state.bars_m1,  m_state.bars_count, TFs[0], start_time, m_state.start_price, m_state.rng_seed);
                GenerateBars(m_state.bars_m30, m_state.bars_count, TFs[1], start_time, m_state.start_price, m_state.rng_seed + 1);
                GenerateBars(m_state.bars_h1,  m_state.bars_count, TFs[2], start_time, m_state.start_price, m_state.rng_seed + 2);
            }
            ImGui::SameLine();
            if (ImGui::Button("Shuffle Seed")) {
                m_state.rng_seed = (m_state.rng_seed * 1664525u + 1013904223u);
            }

            // Выбор массива баров по текущему ТФ
            const int tf_sec = TFs[m_state.tf_index];
            const std::vector<ImGuiX::Widgets::OhlcvBar>& bars =
                (m_state.tf_index == 0 ? m_state.bars_m1
                 : m_state.tf_index == 1 ? m_state.bars_m30
                                         : m_state.bars_h1);

            // Имя серии/окна графика
            std::string title = std::string("Bars (") + TF_NAMES[m_state.tf_index] + ")";

            // Вызов твоего виджета (использует DefaultBarAdapter: &T::open/.../&T::time)
            ImGuiX::Widgets::PlotOHLCChart<ImGuiX::Widgets::OhlcvBar>(title.c_str(), bars, m_state.ohlc_cfg);
        }
#       endif
    }

    void postLanguageChange(const std::string& lang, bool apply_to_all = true) {
        notify(ImGuiX::Events::LangChangeEvent(lang, apply_to_all, window().id()));
    }
};

// Окно-приложение: шрифты, размеры, инициализация контроллера
class ExampleWindow : public ImGuiX::WindowInstance {
public:
    ExampleWindow(int id,
               ImGuiX::ApplicationContext& app,
               std::string name)
        : WindowInstance(id, app, std::move(name)) {}

    void onInit() override {
        createController<WidgetsController>();
        create(id() == 0 ? 800 : 640, id() == 0 ? 600 : 480);
        setWindowIcon("data/resources/icons/icon.png");

        auto& tm = themeManager();
        ImGuiX::Themes::registerCorporateGreyTheme(tm);
        ImGuiX::Themes::registerDarkCharcoalTheme(tm);
        ImGuiX::Themes::registerDarkGraphiteTheme(tm);
        ImGuiX::Themes::registerDarkTealTheme(tm);
        ImGuiX::Themes::registerDeepDarkTheme(tm);
        ImGuiX::Themes::registerGoldBlackTheme(tm);
        ImGuiX::Themes::registerGreenBlueTheme(tm);
        ImGuiX::Themes::registerLightBlueTheme(tm);
        ImGuiX::Themes::registerLightGreenTheme(tm);
        ImGuiX::Themes::registerOSXTheme(tm);
        ImGuiX::Themes::registerPearlLightTheme(tm);
        ImGuiX::Themes::registerSlateDarkTheme(tm);
        ImGuiX::Themes::registerVisualStudioDarkTheme(tm);
        ImGuiX::Themes::registerY2KTheme(tm);
        ImGuiX::Themes::registerCyberY2KTheme(tm);
        ImGuiX::Widgets::ApplyStoredTheme(this);

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
#   ifdef IMGUI_ENABLE_IMPLOT
    std::cout << "IMGUI_ENABLE_IMPLOT" << std::endl;
#   endif
    ImGuiX::Application app;
    app.createWindow<ExampleWindow>("Main Window");
    app.run();
    return 0;
}
