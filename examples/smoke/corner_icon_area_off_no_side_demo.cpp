#include <imguix/core.hpp>
#include <imguix/windows/ImGuiFramedWindow.hpp>
#include <imguix/controllers/ExtendedController.hpp>
#include <imguix/themes/CorporateGreyTheme.hpp>

#if defined(IMGUIX_USE_SFML_BACKEND)

class CornerIconOffNoSideDemoController : public ImGuiX::Controllers::ExtendedController {
public:
    using ExtendedController::ExtendedController;

    void onInit() override {
        setTheme("corporate-grey");
    }

    void drawUi() override {
        ImGui::Begin("Corner Icon Mode OFF (No Side Panel)");
        ImGui::TextUnformatted("WindowFlags::HasCornerIconArea is disabled.");
        ImGui::TextUnformatted("This classic sample uses no side panel.");
        ImGui::TextUnformatted("Title separator is intentionally absent.");
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

class CornerIconOffNoSideDemoWindow : public ImGuiX::Windows::ImGuiFramedWindow {
public:
    using WindowFlags = ImGuiX::Windows::WindowFlags;

    CornerIconOffNoSideDemoWindow(int id, ImGuiX::ApplicationContext& app, std::string name, std::string title)
        : ImGuiFramedWindow(
              id,
              app,
              std::move(name),
              std::move(title),
              WindowFlags::HasMenuBar |
                  WindowFlags::DefaultControlButtons,
              makeConfig()) {}

    void onInit() override {
        createController<CornerIconOffNoSideDemoController>();
        create(980, 680);

        ImGuiX::Themes::registerCorporateGreyTheme(themeManager());
        setTheme("corporate-grey");

        setWindowIcon("data/resources/icons/icon.png");
    }

    void drawTitleBarText() override {
        const float text_y = (m_config.title_bar_height - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::SetCursorPosY(ImMax(0.0f, text_y));
        ImGui::TextUnformatted("HasCornerIconArea OFF (No Side Panel) Demo");
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
    app.createWindow<CornerIconOffNoSideDemoWindow>(
        "CornerIconOffNoSideDemo",
        "Corner Icon Area OFF No Side Demo");
    app.run();
    return 0;
}

#else
int main() {
    static_assert(false, "corner_icon_area_off_no_side_demo.cpp requires IMGUIX_USE_SFML_BACKEND defined.");
    return 0;
}
#endif
