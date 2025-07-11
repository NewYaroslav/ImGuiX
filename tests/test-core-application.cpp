#include <imguix/core.hpp>
#include "imgui.h"

#if defined(IMGUIX_USE_SFML_BACKEND)

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

/// \brief Контроллер, рисующий кнопку в ImGui и круг через SFML.
class DemoController : public ImGuiX::Controller {
public:
    using Controller::Controller;

    void drawContent() override {
        sf::CircleShape shape(window().id() == 0 ? 100.f : 50.f);
        shape.setFillColor(window().id() == 0 ? sf::Color::Green : sf::Color::Red);
        sf::RenderWindow& target = window().getRenderTarget();
        target.draw(shape);
    }

    void drawUi() override {
        ImGui::Begin(window().id() == 0 ? "Hello, world!" : "Works in a second window!");
        ImGui::Button(window().id() == 0 ? "Look at this pretty button" : "Example button");
        ImGui::End();

        if (window().id() == 0) {
            ImGui::ShowDemoWindow();
        }
    }
};

/// \brief Окно, к которому привязан DemoController.
class DemoWindow : public ImGuiX::WindowInstance {
public:

    DemoWindow(int id, ImGuiX::ApplicationControl& app, std::string name)
        : WindowInstance(id, app, std::move(name)) {
    }

    void onInit() override {
        createController<DemoController>();
		create(id() == 0 ? 800 : 640, id() == 0 ? 600 : 480U);
    }
    
    virtual ~DemoWindow() {};
};

int main() {
    ImGuiX::Application app;
    app.createWindow<DemoWindow>("Main Window");
    app.createWindow<DemoWindow>("Child Window");
    app.run();
    return 0;
}

#elif defined(IMGUIX_USE_GLFW_BACKEND)

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

class DemoController : public ImGuiX::Controller {
public:
    using Controller::Controller;

    void drawContent() override {}

    void drawUi() override {
        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();
        ImGui::ShowDemoWindow();
    }
};

class DemoWindow : public ImGuiX::WindowInstance {
public:
    DemoWindow(int id, ImGuiX::ApplicationControl& app, std::string name)
        : WindowInstance(id, app, std::move(name)) {}

    void onInit() override {
        createController<DemoController>();
        create(800, 600);
    }
};

int main() {
    ImGuiX::Application app;
    app.createWindow<DemoWindow>("Main Window");
    app.run();
    return 0;
}

#elif defined(IMGUIX_USE_SDL2_BACKEND)

#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL.h>

class DemoController : public ImGuiX::Controller {
public:
    using Controller::Controller;

    void drawContent() override {}

    void drawUi() override {
        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();
        ImGui::ShowDemoWindow();
    }
};

class DemoWindow : public ImGuiX::WindowInstance {
public:
    DemoWindow(int id, ImGuiX::ApplicationControl& app, std::string name)
        : WindowInstance(id, app, std::move(name)) {}

    void onInit() override {
        createController<DemoController>();
        create(800, 600);
    }
};

int main() {
    ImGuiX::Application app;
    app.createWindow<DemoWindow>("Main Window");
    app.run();
    return 0;
}

#else


/// \brief Простой контроллер для теста.
class DummyController : public ImGuiX::Controller {
public:
    using Controller::Controller;

    void drawContent() override {
        // Ничего не рисуем
    }

    void drawUi() override {
        // Ничего не рисуем
    }
};

/// \brief Простое окно с тиком и отрисовкой.
class DummyWindow : public ImGuiX::WindowInstance {
public:
    DummyWindow(int id, std::string name, ImGuiX::ApplicationControl& app)
        : WindowInstance(id, std::move(name), app) {}
};

int main() {
    ImGuiX::Application app;
    app.run(false); // запуск синхронно
    return 0;
}

#endif