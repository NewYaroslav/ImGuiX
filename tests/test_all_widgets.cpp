#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include <imguix/core.hpp>

#define main widgets_demo_main
#include "../examples/smoke/widgets_demo.cpp"
#undef main

static bool g_is_closed = false;

class CloseAfterFirstFrameController : public ImGuiX::Controller {
public:
    explicit CloseAfterFirstFrameController(ImGuiX::WindowInterface& window)
        : Controller(window) {}

    void drawContent() override {
        if (!g_is_closed) {
            g_is_closed = true;
            window().close();
        }
    }

    void drawUi() override {}
};

class TestWindow : public ImGuiX::WindowInstance {
public:
    TestWindow(int id, ImGuiX::ApplicationContext& app, std::string name)
        : WindowInstance(id, app, std::move(name)) {}

    void onInit() override {
        createController<WidgetsController>();
        createController<CloseAfterFirstFrameController>();
        create(640, 480);
    }
};

int main() {
    try {
        ImGuiX::Application app;
        app.createWindow<TestWindow>("Widgets Test");
        app.run();
        if (!g_is_closed) {
            std::cerr << "window did not close\n";
            return 1;
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    } catch (...) {
        return 1;
    }
}
