#include <iostream>
#include <imguix/core.hpp>
#include <imguix/windows/ImGuiFramedWindow.hpp>
#include <imguix/controllers/ExtendedController.hpp>

#if defined(IMGUIX_USE_SFML_BACKEND)

class DemoFramedWindow;

class DemoFramedController : public ImGuiX::Controllers::ExtendedController {
public:
    using ExtendedController::ExtendedController;

    void drawContent() override {
        sf::RenderWindow& target = window().getRenderTarget();
        sf::CircleShape shape(80.f);
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(sf::Vector2f(100.f, 100.f));
        target.draw(shape);
    }

    void drawUi() override {
        ImGui::Begin("ImGui Framed Controller");
        ImGui::Text("This is a custom ImGui-framed window");
        if (ImGui::Button("Open Another Window")) {
            createWindow<DemoFramedWindow>("Framed Window", "ImGui Window");
        }
        if (ImGui::Button("Toggle Background")) { 
            m_background_off = !m_background_off;
            window().setDisableBackground(m_background_off);
        }
        if (ImGui::Button("Close Window")) {
            window().close();
        }
        ImGui::End();

        ImGui::ShowDemoWindow();
    }

private:
    bool m_background_off = false;
};

class DemoFramedWindow : public ImGuiX::Windows::ImGuiFramedWindow {
public:
    using WindowFlags = ImGuiX::Windows::WindowFlags;

    DemoFramedWindow(int id, ImGuiX::ApplicationControl& app, std::string name, std::string title)
        : ImGuiFramedWindow(id, app, std::move(name), std::move(title),
            WindowFlags::HasMenuBar |
            WindowFlags::DefaultControlButtons) {}

    void onInit() override {
        createController<DemoFramedController>();
        create(800, 600);
        ImGui::StyleColorsLight();
        ImGui::GetStyle().WindowRounding = 8.0f;
    }
    
    void drawMenuBar() override {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open")) {
                    
                }
                if (ImGui::MenuItem("Exit")) {
                    close();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                ImGui::MenuItem("Undo");
                ImGui::MenuItem("Redo");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    };
};

int main() {
    ImGuiX::Application app;
    app.createWindow<DemoFramedWindow>("Main Framed Window", "ImGui Framed Window");
    app.run();
    return 0;
}

#else
int main() {
    static_assert(false, "test-framed-window.cpp requires IMGUIX_USE_SFML_BACKEND defined.");
    return 0;
}
#endif
