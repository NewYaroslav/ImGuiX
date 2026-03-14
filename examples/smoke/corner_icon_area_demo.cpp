#include <imguix/core.hpp>
#include <imguix/windows/ImGuiFramedWindow.hpp>
#include <imguix/controllers/ExtendedController.hpp>
#include <imguix/themes/CorporateGreyTheme.hpp>

#if defined(IMGUIX_USE_SFML_BACKEND)

class CornerIconDemoController : public ImGuiX::Controllers::ExtendedController {
public:
    using ExtendedController::ExtendedController;

    void onInit() override {
        setTheme("corporate-grey");
    }

    void drawUi() override {
        ImGui::Begin("Corner Icon Mode");
        ImGui::TextUnformatted("WindowFlags::HasCornerIconArea is enabled.");
        ImGui::TextUnformatted("Menu placement: InTitleBar.");
        ImGui::TextUnformatted("Menu starts after title text and stays before control buttons.");
        ImGui::TextUnformatted("InTitleBar menu uses transparent background.");
        ImGui::TextUnformatted("Side panel starts below icon slot + corner gap.");
        ImGui::TextUnformatted("Right control buttons use compact corner+border geometry.");
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

class CornerIconDemoWindow : public ImGuiX::Windows::ImGuiFramedWindow {
public:
    using WindowFlags = ImGuiX::Windows::WindowFlags;

    CornerIconDemoWindow(int id, ImGuiX::ApplicationContext& app, std::string name, std::string title)
        : ImGuiFramedWindow(
              id,
              app,
              std::move(name),
              std::move(title),
              WindowFlags::HasMenuBar |
                  WindowFlags::DefaultControlButtons |
                  WindowFlags::HasCornerIconArea |
                  WindowFlags::CornerModeRounding |
                  WindowFlags::CornerModeBorder,
              makeConfig()) {}

    void onInit() override {
        createController<CornerIconDemoController>();
        create(980, 680);

        ImGuiX::Themes::registerCorporateGreyTheme(themeManager());
        setTheme("corporate-grey");

        setWindowIcon("data/resources/icons/icon.png");
    }

    void drawTitleBarText() override {
        const float text_y = (m_config.title_bar_height - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::SetCursorPosY(ImMax(0.0f, text_y));
        ImGui::TextUnformatted("HasCornerIconArea Demo");
    }

    void drawMenuBar() override {
        if (!ImGui::BeginMenuBar()) {
            return;
        }

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                close();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    void drawSidePanel() override {
        ImGui::TextUnformatted("A1");
        ImGui::TextUnformatted("A2");
        ImGui::TextUnformatted("A3");
    }

    void drawCornerIcon() override {
        // Reuse default SFML icon rendering loaded via setWindowIcon().
        ImGuiFramedWindow::drawCornerIcon();
    }

private:
    static ImGuiX::Windows::ImGuiFramedWindowConfig makeConfig() {
        ImGuiX::Windows::ImGuiFramedWindowConfig cfg{};
        cfg.title_bar_height = 40;
        cfg.side_panel_width = 0; // corner mode auto-width: equals title_bar_height
        cfg.corner_icon_mode_rounding_radius = 8.0f;
        cfg.corner_menu_bar_placement = ImGuiX::Windows::CornerMenuBarPlacement::InTitleBar;
        return cfg;
    }
};

int main() {
    ImGuiX::Application app;
    app.createWindow<CornerIconDemoWindow>("CornerIconDemo", "Corner Icon Area Demo");
    app.run();
    return 0;
}

#else
int main() {
    static_assert(false, "corner_icon_area_demo.cpp requires IMGUIX_USE_SFML_BACKEND defined.");
    return 0;
}
#endif
