#include <imguix/core.hpp>
#include <imguix/windows/ImGuiFramedWindow.hpp>
#include <imguix/controllers/ExtendedController.hpp>
#include <imguix/themes/CorporateGreyTheme.hpp>

#if defined(IMGUIX_USE_SFML_BACKEND)

class CornerIconOffNoMenuDemoController : public ImGuiX::Controllers::ExtendedController {
public:
    using ExtendedController::ExtendedController;

    void onInit() override {
        setTheme("corporate-grey");
    }

    void drawUi() override {
        ImGui::Begin("Corner Icon Mode OFF (No Menu Bar)");
        ImGui::TextUnformatted("WindowFlags::HasCornerIconArea is disabled.");
        ImGui::TextUnformatted("WindowFlags::HasMenuBar is disabled.");
        ImGui::TextUnformatted("Only title bar and system control buttons are shown.");
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

class CornerIconOffNoMenuDemoWindow : public ImGuiX::Windows::ImGuiFramedWindow {
public:
    using WindowFlags = ImGuiX::Windows::WindowFlags;

    CornerIconOffNoMenuDemoWindow(int id, ImGuiX::ApplicationContext& app, std::string name, std::string title)
        : ImGuiFramedWindow(
              id,
              app,
              std::move(name),
              std::move(title),
              WindowFlags::DefaultControlButtons,
              makeConfig()) {}

    void onInit() override {
        createController<CornerIconOffNoMenuDemoController>();
        create(980, 680);

        ImGuiX::Themes::registerCorporateGreyTheme(themeManager());
        setTheme("corporate-grey");

        setWindowIcon("data/resources/icons/icon.png");
    }

    void drawTitleBarText() override {
        const float text_y = (m_config.title_bar_height - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::SetCursorPosY(ImMax(0.0f, text_y));
        ImGui::TextUnformatted("HasCornerIconArea OFF (No Menu Bar) Demo");
    }

private:
    static ImGuiX::Windows::ImGuiFramedWindowConfig makeConfig() {
        ImGuiX::Windows::ImGuiFramedWindowConfig cfg{};
        cfg.title_bar_height = 40;
        cfg.side_panel_width = 0;
        return cfg;
    }
};

int main() {
    ImGuiX::Application app;
    app.createWindow<CornerIconOffNoMenuDemoWindow>(
        "CornerIconOffNoMenuDemo",
        "Corner Icon Area OFF No Menu Demo");
    app.run();
    return 0;
}

#else
int main() {
    static_assert(false, "corner_icon_area_off_no_menu_demo.cpp requires IMGUIX_USE_SFML_BACKEND defined.");
    return 0;
}
#endif
