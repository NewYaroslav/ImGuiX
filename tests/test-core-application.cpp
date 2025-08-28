#include <iostream>
#include <imguix/core.hpp>
#include "imgui.h"

#if defined(IMGUIX_USE_SFML_BACKEND)

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <atomic>
#include <chrono>
#include <thread>

class DemoWindow;

/// \brief Event carrying seconds elapsed since application start.
class SecondsElapsedEvent : public ImGuiX::Pubsub::Event {
public:
    int value;

    explicit SecondsElapsedEvent(int seconds) : value(seconds) {}

    std::type_index type() const override { return typeid(SecondsElapsedEvent); }
    const char* name() const override { return "SecondsElapsedEvent"; }
};

/// \brief Model running a timer thread that posts SecondsElapsedEvent.
class TimerModel : public ImGuiX::Model {
public:
    using Model::Model;

    void onInit() override {
        m_thread = std::thread([this]() { timerThread(); });
    }

    void process(ImGuiX::Pubsub::SyncNotifier& /*notifier*/) override {}

    ~TimerModel() override {
        m_stop = true;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

private:
    void timerThread() {
        auto start = std::chrono::steady_clock::now();
        while (!m_stop) {
            int secs = static_cast<int>(
                std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::steady_clock::now() - start)
                    .count());
            auto evt = std::make_unique<SecondsElapsedEvent>(secs);
            notifyAsync(std::move(evt));
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (isClosing()) m_stop = true;
        }
    }

    std::thread m_thread;
    std::atomic<bool> m_stop{false};
};

/// \brief Controller that draws a button in ImGui and a circle using SFML.
class DemoController : public ImGuiX::Controller {
public:
    DemoController(ImGuiX::WindowInterface& window, ImGuiX::Application& app)
        : Controller(window), m_app(app) {
        subscribe<SecondsElapsedEvent>();
    }
    
    void onEvent(const ImGuiX::Pubsub::Event* const event) override {
        if (event->is<SecondsElapsedEvent>()) {
            if (window().id() == 0) {
                m_seconds = event->asRef<SecondsElapsedEvent>().value;
            }
        }
    }

    void drawContent() override {
        sf::CircleShape shape(window().id() == 0 ? 100.f : 50.f);
        shape.setFillColor(window().id() == 0 ? sf::Color::Green : sf::Color::Red);
        sf::RenderWindow& target = window().getRenderTarget();
        target.draw(shape);
    }

    void drawUi() override {
        ImGui::PushID(window().id());

        ImGui::Begin(window().id() == 0 ? "Hello, world!" : "Works in a second window!");
        if (ImGui::Button(window().id() == 0 ? "Open new window" : "Example button")) {
            if (window().id() == 0) {
                m_app.createWindow<DemoWindow>("Additional Window");
            }
        }
        if (window().id() == 0) {
            ImGui::Text("Seconds elapsed: %d", m_seconds);
        }
        ImGui::End();

        if (window().id() == 0) {
            ImGui::ShowDemoWindow();
        }
        
        ImGui::PopID();
    }

private:
    ImGuiX::Application& m_app;
    int m_seconds{0};
};

/// \brief Window that hosts the DemoController.
class DemoWindow : public ImGuiX::WindowInstance {
public:

    DemoWindow(int id, ImGuiX::ApplicationContext& app, std::string name)
        : WindowInstance(id, app, std::move(name)) {
    }

    void onInit() override {
        createController<DemoController>(static_cast<ImGuiX::Application&>(m_application));
        create(id() == 0 ? 800 : 640, id() == 0 ? 600 : 480);
        setWindowIcon("data/resources/icons/icon.png");
    }
    
    virtual ~DemoWindow() {};
};

int main() {
    ImGuiX::Application app;
    app.createWindow<DemoWindow>("Main Window");
    app.createModel<TimerModel>();
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
    DemoWindow(int id, ImGuiX::ApplicationContext& app, std::string name)
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
    DemoWindow(int id, ImGuiX::ApplicationContext& app, std::string name)
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


/// \brief Simple controller for testing.
class DummyController : public ImGuiX::Controller {
public:
    using Controller::Controller;

    void drawContent() override {
        // Draw nothing
    }

    void drawUi() override {
        // Draw nothing
    }
};

/// \brief Simple window with ticking and rendering.
class DummyWindow : public ImGuiX::WindowInstance {
public:
    DummyWindow(int id, std::string name, ImGuiX::ApplicationContext& app)
        : WindowInstance(id, std::move(name), app) {}
};

int main() {
    ImGuiX::Application app;
    app.run(false); // run synchronously
    return 0;
}

#endif
