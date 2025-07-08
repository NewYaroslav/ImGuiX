#define IMGUI_ENABLE_FREETYPE
#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads
#include <misc/freetype/imgui_freetype.h>

#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({640, 480}), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    
    if (!ImGui::SFML::Init(window)) return -1;
    
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->FontLoader = ImGuiFreeType::GetFontLoader();
    io.Fonts->FontLoaderFlags = ImGuiFreeTypeBuilderFlags_NoHinting;
    
    ImFont* font = io.Fonts->AddFontFromFileTTF("data/fonts/Roboto-Medium.ttf", 18.0f);
    io.FontDefault = font;

    if (!ImGui::SFML::UpdateFontTexture()) return -1;

    std::printf("Fonts count: %d\n", io.Fonts->Fonts.Size);

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::ShowDemoWindow();

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        window.clear();
        window.draw(shape);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}