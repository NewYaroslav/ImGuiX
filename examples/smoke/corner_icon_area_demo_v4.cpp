#include <imguix/core.hpp>
#include <imguix/controllers/ExtendedController.hpp>
#include <imguix/themes/CorporateGreyTheme.hpp>
#include <imguix/windows/ImGuiFramedWindow.hpp>
#include <algorithm>

#if defined(IMGUIX_USE_SFML_BACKEND)

class CornerIconDemoV4Controller : public ImGuiX::Controllers::ExtendedController {
public:
    using ExtendedController::ExtendedController;

    void onInit() override {
        setTheme("corporate-grey");
    }

    void drawUi() override {
        if (ImGui::BeginChild(
                "##corner_v4_main_content",
                ImVec2(0.0f, 0.0f),
                ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding,
                ImGuiWindowFlags_None)) {
            ImGui::SeparatorText("Nested Main Child");
            ImGui::TextUnformatted("This demo mirrors the mgc main-region pattern.");
            ImGui::TextUnformatted("The framed window applies main_region_padding first.");
            ImGui::TextUnformatted("Then this nested child uses its own child padding inside.");

            ImGui::Spacing();
            ImGui::BulletText("corner layout");
            ImGui::BulletText("no menu bar");
            ImGui::BulletText("cfg.main_region_padding = ImVec2(16, 12)");
            ImGui::BulletText("nested child uses ImGuiChildFlags_AlwaysUseWindowPadding");

            ImGui::Spacing();
            if (ImGui::Button(m_disable_background ? "Enable Background" : "Disable Background")) {
                m_disable_background = !m_disable_background;
                window().setDisableBackground(m_disable_background);
            }

            ImGui::Spacing();
            ImGui::SeparatorText("Width Probe");
            ImGui::Text("Child content width: %.1f px", ImGui::GetContentRegionAvail().x);
            ImGui::Button("Primary Action", ImVec2(std::min(260.0f, ImGui::GetContentRegionAvail().x), 0.0f));
            ImGui::Spacing();
            ImGui::TextWrapped(
                "Use this sample to inspect the gap from the framed window chrome to the nested child border, "
                "and then the child-internal padding before the actual widgets.");
        }
        ImGui::EndChild();
    }

private:
    bool m_disable_background = false;
};

class CornerIconDemoV4Window : public ImGuiX::Windows::ImGuiFramedWindow {
public:
    using WindowFlags = ImGuiX::Windows::WindowFlags;

    CornerIconDemoV4Window(int id, ImGuiX::ApplicationContext& app, std::string name, std::string title)
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
        createController<CornerIconDemoV4Controller>();
        create(980, 680);

        ImGuiX::Themes::registerCorporateGreyTheme(themeManager());
        setTheme("corporate-grey");

        setWindowIcon("data/resources/icons/icon.png");
    }

    void drawTitleBarText() override {
        const float text_y = (m_config.title_bar_height - ImGui::GetTextLineHeight()) * 0.5f;
        ImGui::SetCursorPosY(std::max(0.0f, text_y));
        ImGui::TextUnformatted("HasCornerIconArea Demo V4");
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
        cfg.main_region_padding = ImVec2(16.0f, 12.0f);
        cfg.side_panel_content_alignment = ImGuiX::Windows::SidePanelContentAlignment::SymmetricInset;
        cfg.corner_icon_mode_rounding_radius = 8.0f;
        cfg.corner_rounding_style = ImGuiX::Windows::CornerRoundingStyle::NoTopLeftOnTitleAndSide;
        cfg.corner_menu_bar_placement = ImGuiX::Windows::CornerMenuBarPlacement::MainRegion;
        return cfg;
    }
};

int main() {
    ImGuiX::Application app;
    app.createWindow<CornerIconDemoV4Window>("CornerIconDemoV4", "Corner Icon Area Demo V4");
    app.run();
    return 0;
}

#else
int main() {
    static_assert(false, "corner_icon_area_demo_v4.cpp requires IMGUIX_USE_SFML_BACKEND defined.");
    return 0;
}
#endif
