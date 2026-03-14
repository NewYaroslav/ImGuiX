#include <imguix/core.hpp>
#include <imguix/windows/ImGuiFramedWindow.hpp>
#include <imguix/controllers/ExtendedController.hpp>
#include <imguix/themes/CorporateGreyTheme.hpp>

#if defined(IMGUIX_USE_SFML_BACKEND)

class CornerIconDemoV3NoTopLeftController : public ImGuiX::Controllers::ExtendedController {
public:
    using ExtendedController::ExtendedController;

    void onInit() override {
        setTheme("corporate-grey");
    }

    void drawUi() override {
        ImGui::Begin("Corner Icon Mode V3 (No Top-Left)");
        ImGui::TextUnformatted("WindowFlags::HasCornerIconArea is enabled.");
        ImGui::TextUnformatted("Corner rounding style: NoTopLeftOnTitleAndSide.");
        ImGui::TextUnformatted("Menu bar is intentionally disabled in this variant.");
        ImGui::Separator();

        if (ImGui::Button(m_disable_background ? "Enable Background" : "Disable Background")) {
            m_disable_background = !m_disable_background;
            window().setDisableBackground(m_disable_background);
        }

        ImGui::End();
    }

private:
    bool m_disable_background = false;
};

class CornerIconDemoV3NoTopLeftWindow : public ImGuiX::Windows::ImGuiFramedWindow {
public:
    using WindowFlags = ImGuiX::Windows::WindowFlags;

    CornerIconDemoV3NoTopLeftWindow(int id, ImGuiX::ApplicationContext& app, std::string name, std::string title)
        : ImGuiFramedWindow(
              id,
              app,
              std::move(name),
              std::move(title),
              WindowFlags::DefaultControlButtons |
                  WindowFlags::HasCornerIconArea |
                  WindowFlags::CornerModeRounding |
                  WindowFlags::CornerModeBorder,
              makeConfig()) {}

    void onInit() override {
        createController<CornerIconDemoV3NoTopLeftController>();
        create(980, 680);

        ImGuiX::Themes::registerCorporateGreyTheme(themeManager());
        setTheme("corporate-grey");

        setWindowIcon("data/resources/icons/icon.png");
    }

    void drawTitleBarText() override {
        const float text_y = (m_config.title_bar_height - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::SetCursorPosY(ImMax(0.0f, text_y));
        ImGui::TextUnformatted("HasCornerIconArea Demo V3 (No Top-Left)");
    }

    void drawSidePanel() override {
        ImGui::TextUnformatted("A1");
        ImGui::TextUnformatted("A2");
        ImGui::TextUnformatted("A3");
    }

    void drawCornerIcon() override {
        ImGuiFramedWindow::drawCornerIcon();
    }

private:
    static ImGuiX::Windows::ImGuiFramedWindowConfig makeConfig() {
        ImGuiX::Windows::ImGuiFramedWindowConfig cfg{};
        cfg.title_bar_height = 40;
        cfg.side_panel_width = 0;
        cfg.corner_icon_mode_rounding_radius = 8.0f;
        cfg.corner_rounding_style = ImGuiX::Windows::CornerRoundingStyle::NoTopLeftOnTitleAndSide;
        cfg.corner_menu_bar_placement = ImGuiX::Windows::CornerMenuBarPlacement::MainRegion;
        return cfg;
    }
};

int main() {
    ImGuiX::Application app;
    app.createWindow<CornerIconDemoV3NoTopLeftWindow>(
        "CornerIconDemoV3NoTopLeft",
        "Corner Icon Area Demo V3 (No Top-Left)");
    app.run();
    return 0;
}

#else
int main() {
    static_assert(
        false,
        "corner_icon_area_demo_v3_no_top_left.cpp requires IMGUIX_USE_SFML_BACKEND defined.");
    return 0;
}
#endif
