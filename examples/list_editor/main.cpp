#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include <imguix/widgets/list_editor.hpp>
#include <vector>
#include <string>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "ListEditor example");
    window.setFramerateLimit(60);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::SFML::Init(window);

    bool show_demo = false;
    sf::Clock clk;

    std::vector<std::string> names {"Alice", "Bob"};
    std::vector<int> numbers {1, 2, 3};

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            ImGui::SFML::ProcessEvent(window, ev);
            if (ev.type == sf::Event::Closed) window.close();
        }

        ImGui::SFML::Update(window, clk.restart());

        if (show_demo) ImGui::ShowDemoWindow(&show_demo);
        ImGui::Begin("List editor");
        ImGuiX::Widgets::ListEditor("names", names);
        ImGuiX::Widgets::ListEditor("numbers", numbers);
        ImGui::End();

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}
