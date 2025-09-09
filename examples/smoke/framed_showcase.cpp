#include <algorithm>
#include <cmath>
#include <random>

#include <imguix/core.hpp>
#include <imguix/windows/ImGuiFramedWindow.hpp>
#include <imguix/controllers/ExtendedController.hpp>

// --- Widgets we showcase ---
#include <imguix/widgets/misc/theme_picker.hpp>
#include <imguix/widgets/misc/markers.hpp>
#include <imguix/widgets/misc/text_center.hpp>
#include <imguix/widgets/notify/notifications.hpp>

#include <imguix/widgets/time/time_picker.hpp>
#include <imguix/widgets/time/date_picker.hpp>
#include <imguix/widgets/time/days_selector.hpp>
#include <imguix/widgets/time/hours_selector.hpp>
#include <imguix/widgets/time/expiry_picker.hpp>

#include <imguix/widgets/controls/icon_combo.hpp>

// --- Themes we register ---
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

#if defined(IMGUIX_ENABLE_IMPLOT)
#include <imguix/widgets/plot/PlotOHLCChart.hpp>
#endif

#if !defined(IMGUIX_USE_SFML_BACKEND)
#  error "This demo requires SFML backend. Define IMGUIX_USE_SFML_BACKEND."
#endif

// ============================================================================
// Demo controller: draws SFML content + ImGui UI
// ============================================================================
class DemoFramedController : public ImGuiX::Controllers::ExtendedController {
public:
    using ExtendedController::ExtendedController;

    void onInit() override {
        // Basic look & feel
        setTheme("dark");                  // "dark"/"light" are shortcuts in your stack
        ImGui::GetStyle().WindowRounding = 8.0f;
    }

    void drawContent() override {
        // Draw simple SFML background to show compositing under UI
        sf::RenderWindow& target = window().getRenderTarget();
        sf::RectangleShape back(sf::Vector2f(
            static_cast<float>(target.getSize().x),
            static_cast<float>(target.getSize().y)));
        back.setFillColor(m_background_off ? sf::Color(20, 24, 28) : sf::Color(12, 16, 20));
        target.draw(back);

        // A soft decorative circle
        sf::CircleShape shape(90.f);
        shape.setFillColor(sf::Color(0, 120, 255, 40));
        shape.setPosition(sf::Vector2f(80.f, 80.f));
        target.draw(shape);
    }

    void drawUi() override {
        // --- Top toolbar -----------------------------------------------------
        if (ImGui::Begin("Showcase Controls", nullptr,
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::Button(m_background_off ? "Enable Background" : "Disable Background")) {
                m_background_off = !m_background_off;
                window().setDisableBackground(m_background_off);
            }
            ImGui::SameLine();
            if (ImGui::Button("Send Info Notification")) {
                ImGuiX::Widgets::NotifyInfo(this, "Hello: This is a demo notification.");
            }
            ImGui::SameLine();
            if (ImGui::Button("Send Warning")) {
                ImGuiX::Widgets::NotifyWarning(this, "Watch out: Something to keep an eye on.");
            }
            ImGui::SameLine();
            if (ImGui::Button("Send Error")) {
                ImGuiX::Widgets::NotifyError(this, "Oops: Something went wrong.");
            }
            ImGui::SeparatorText("Theme");
            // Theme picker integrated with this controller/window
            ImGuiX::Widgets::ThemePicker("demo.theme", this);
        }
        ImGui::End();

        // --- Main tabbed showcase -------------------------------------------
        ImGui::SetNextWindowSize(ImVec2(920, 640), ImGuiCond_FirstUseEver);
        ImGui::Begin("ImGuiX Framed Showcase", nullptr, ImGuiWindowFlags_MenuBar);

        // Optional menu inside the framed UI
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("ImGui Demo")) {
                    m_show_imgui_demo = !m_show_imgui_demo;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (ImGui::BeginTabBar("showcase.tabs", ImGuiTabBarFlags_Reorderable)) {
            if (ImGui::BeginTabItem("Time / Date")) {
                // These Demo* helpers come with your widget headers
                ImGuiX::Widgets::DemoHoursSelector();
                ImGui::Separator();
                ImGuiX::Widgets::DemoDaysOfWeekSelector();
                ImGui::Separator();
                ImGuiX::Widgets::DemoTimePicker();
                ImGui::Separator();
                ImGuiX::Widgets::DemoDatePicker();
                ImGui::Separator();
                ImGuiX::Widgets::DemoExpiryPicker();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Misc")) {
                ImGuiX::Widgets::DemoMarkers();
                ImGui::Separator();
                ImGuiX::Widgets::DemoTextCenter();

                ImGui::SeparatorText("Icon Combo (Material Icons merged)");
                static int sel_idx = 0;
                static const char* items[] = { u8"One", u8"Two", u8"Three" };
                static const char* icons[] = { u8"\ue430", u8"\ue51c", u8"\ue885" };
                if (ImGuiX::Widgets::BeginIconCombo("icon.combo", items[sel_idx], icons[sel_idx])) {
                    for (int i = 0; i < IM_ARRAYSIZE(items); ++i) {
                        bool selected = (i == sel_idx);
                        if (ImGui::Selectable(items[i], selected))
                            sel_idx = i;
                    }
                    ImGuiX::Widgets::EndIconCombo();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Notifications")) {
                // Notification center demo (overlay toasts)
                ImGuiX::Widgets::DemoNotifications(this);
                ImGui::EndTabItem();
            }
#if defined(IMGUIX_ENABLE_IMPLOT)
            if (ImGui::BeginTabItem("OHLC Plot")) {
                drawOhlcDemo();
                ImGui::EndTabItem();
            }
#endif
            ImGui::EndTabBar();
        }

        if (m_show_imgui_demo)
            ImGui::ShowDemoWindow(&m_show_imgui_demo);

        ImGui::End();
    }

private:
    bool m_background_off = false;
    bool m_show_imgui_demo = false;

#if defined(IMGUIX_ENABLE_IMPLOT)
    // --- Simple OHLC demo with synthetic bars -------------------------------
    struct Bar { double open{}, high{}, low{}, close{}, volume{}; std::uint64_t time{}; };

    static void GenerateBars(std::vector<Bar>& out, int count, int tf_sec,
                             std::uint64_t start_time, double start_price, unsigned seed) {
        out.clear();
        out.reserve(count < 0 ? 0 : count);

        std::mt19937 rng(seed);
        std::normal_distribution<double> step(0.0, 0.002);
        std::normal_distribution<double> wick(0.0, 0.0008);
        std::lognormal_distribution<double> voln(0.0, 0.25);

        double prev_close = std::max(0.1, start_price);
        for (int i = 0; i < count; ++i) {
            Bar b{};
            b.time  = start_time + static_cast<std::uint64_t>(i) * static_cast<std::uint64_t>(tf_sec);
            const double drift = 0.00002 * tf_sec;
            const double ch    = (drift + step(rng)) * prev_close;
            b.open  = prev_close;
            b.close = std::max(0.01, b.open + ch);

            const double base_range = std::max(1e-6, std::abs(b.close - b.open));
            const double up_wick    = std::max(0.0, (base_range * 0.5) + wick(rng) * prev_close);
            const double dn_wick    = std::max(0.0, (base_range * 0.5) + wick(rng) * prev_close);

            b.high  = std::max(b.open, b.close) + up_wick;
            b.low   = std::max(0.01, std::min(b.open, b.close) - dn_wick);

            const double range = b.high - b.low;
            b.volume = std::max(0.0, 800.0 + 50000.0 * range / std::max(1e-6, b.close) * voln(rng));

            out.push_back(b);
            prev_close = b.close;
        }
    }

    void drawOhlcDemo() {
        static int bars_count = 400;
        static int tf_index   = 0; // 0=M1, 1=M30, 2=H1
        static double start_price = 100.0;
        static unsigned rng_seed = 42;
        static const int TFs[] = { 60, 30*60, 60*60 };
        static const char* TF_NAMES[] = { "M1", "M30", "H1" };

        static std::vector<Bar> bars_m1, bars_m30, bars_h1;

        auto regen = [&](){
            const std::uint64_t start_time = 1700000000ULL;
            GenerateBars(bars_m1,  bars_count, TFs[0], start_time, start_price, rng_seed);
            GenerateBars(bars_m30, bars_count, TFs[1], start_time, start_price, rng_seed + 1);
            GenerateBars(bars_h1,  bars_count, TFs[2], start_time, start_price, rng_seed + 2);
        };

        ImGui::PushItemWidth(160);
        ImGui::Combo("Timeframe", &tf_index, TF_NAMES, IM_ARRAYSIZE(TF_NAMES));
        ImGui::PopItemWidth();
        ImGui::SliderInt("Bars", &bars_count, 50, 2000);
        ImGui::DragScalar("Start Price", ImGuiDataType_Double, &start_price, 0.1f);
        ImGui::InputScalar("Seed", ImGuiDataType_U32, &rng_seed);
        if (ImGui::Button("Regenerate")) regen();
        ImGui::SameLine();
        if (ImGui::Button("Shuffle Seed")) rng_seed = (rng_seed * 1664525u + 1013904223u), regen();

        const auto& bars = (tf_index == 0 ? bars_m1 : tf_index == 1 ? bars_m30 : bars_h1);
        ImGuiX::Widgets::OHLCChartConfig cfg{};
        ImGuiX::Widgets::PlotOHLCChart<Bar>("Synthetic OHLC", bars, cfg);
    }
#endif
};

// ============================================================================
// Framed window: registers themes, fonts, spawns controller
// ============================================================================
class DemoFramedWindow : public ImGuiX::Windows::ImGuiFramedWindow {
public:
    using WindowFlags = ImGuiX::Windows::WindowFlags;

    DemoFramedWindow(int id, ImGuiX::ApplicationContext& app, std::string name, std::string title)
        : ImGuiFramedWindow(id, app, std::move(name), std::move(title),
            WindowFlags::HasMenuBar | WindowFlags::DefaultControlButtons) {}

    void onInit() override {
        createController<DemoFramedController>();
        create(1024, 720);

        // Register a palette of themes for ThemePicker
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

        // Fonts: Cyrillic + Material Icons merged (adjust paths to your data dir)
        setWindowIcon("data/resources/icons/icon.png");
        fontsBeginManual();
        fontsSetRangesPreset("Default+Cyrillic+Vietnamese+Punct+PUA+LatinExtA");
        fontsAddBody({ "NotoSans-Regular.ttf", 18.0f });
        fontsAddMerge(ImGuiX::Fonts::FontRole::Icons,
                      { "MaterialIcons-Regular.ttf", 18.0f, 4.0f, true });
        fontsAddHeadline(ImGuiX::Fonts::FontRole::H1, { "NotoSans-Bold.ttf", 24.0f });
        fontsBuildNow();
    }

    void drawMenuBar() override {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) close();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Theme")) {
                if (ImGui::MenuItem("Reset to stored")) {
                    ImGuiX::Widgets::ApplyStoredTheme(this);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
};

// ============================================================================
// Entry
// ============================================================================
int main() {
    ImGuiX::Application app;
    app.createWindow<DemoFramedWindow>("MainFramed", "ImGuiX Framed Showcase");
    app.run();
    return 0;
}

