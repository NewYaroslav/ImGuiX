#include <imguix/core.hpp>
#include <imguix/windows/ImGuiFramedWindow.hpp>
#include <imguix/controllers/ExtendedController.hpp>
#include <imguix/themes/CorporateGreyTheme.hpp>
#include <algorithm>

#if defined(IMGUIX_USE_SFML_BACKEND)

class CornerIconOffNoMenuOddTitleDemoController : public ImGuiX::Controllers::ExtendedController {
public:
    using ExtendedController::ExtendedController;

    void onInit() override {
        setTheme("corporate-grey");
    }

    void drawUi() override {
        ImGui::Begin("Corner Icon Mode OFF (No Menu, Odd Title Height)");
        ImGui::TextUnformatted("WindowFlags::HasCornerIconArea is disabled.");
        ImGui::TextUnformatted("WindowFlags::HasMenuBar is disabled.");
        ImGui::TextUnformatted("Title bar height is intentionally set to 47 px.");
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

class CornerIconOffNoMenuOddTitleDemoWindow : public ImGuiX::Windows::ImGuiFramedWindow {
public:
    using WindowFlags = ImGuiX::Windows::WindowFlags;

    CornerIconOffNoMenuOddTitleDemoWindow(int id, ImGuiX::ApplicationContext& app, std::string name, std::string title)
        : ImGuiFramedWindow(
              id,
              app,
              std::move(name),
              std::move(title),
              WindowFlags::DefaultControlButtons,
              makeConfig()) {}

    void onInit() override {
        createController<CornerIconOffNoMenuOddTitleDemoController>();
        create(980, 680);

        ImGuiX::Themes::registerCorporateGreyTheme(themeManager());
        setTheme("corporate-grey");

        setWindowIcon("data/resources/icons/icon.png");
    }

    void drawTitleBarText() override {
        const float text_y = (m_config.title_bar_height - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::SetCursorPosY(std::max(0.0f, text_y));
        ImGui::TextUnformatted("HasCornerIconArea OFF (No Menu Bar, Title=46) Demo");
    }

private:
    static ImGuiX::Windows::ImGuiFramedWindowConfig makeConfig() {
        ImGuiX::Windows::ImGuiFramedWindowConfig cfg{};
        cfg.title_bar_height = 46;
        cfg.side_panel_width = 0;
        return cfg;
    }
};

int main() {
    ImGuiX::Application app;
    app.createWindow<CornerIconOffNoMenuOddTitleDemoWindow>(
        "CornerIconOffNoMenuOddTitleDemo",
        "Corner Icon Area OFF No Menu Odd Title Demo");
    app.run();
    return 0;
}

#else
int main() {
    static_assert(false, "corner_icon_area_off_no_menu_odd_title_demo.cpp requires IMGUIX_USE_SFML_BACKEND defined.");
    return 0;
}
#endif
