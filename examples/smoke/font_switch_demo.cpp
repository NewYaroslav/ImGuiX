#include <imguix/core.hpp>

#include <iostream>

class FontSwitchController : public ImGuiX::Controller {
public:
    using Controller::Controller;

    void drawContent() override {}

    void drawUi() override {
        ImGui::PushID(window().id());
        ImGui::Begin("Font Switch Demo");

        ImGui::SeparatorText("Default Body");
        ImGui::TextUnformatted("This line uses the current (default) Body font.");

        ImGui::SeparatorText("Switch to H1");
        if (ImFont* h1_font = getFont(ImGuiX::Fonts::FontRole::H1)) {
            ImGui::PushFont(h1_font);
            ImGui::TextUnformatted("This line is rendered with H1.");
            ImGui::PopFont();
        } else {
            ImGui::TextUnformatted("H1 font is unavailable. Fallback to current font.");
        }

        ImGui::SeparatorText("Back to Body");
        ImGui::TextUnformatted("After PopFont() we are back to the default Body font.");

        if (ImFont* body_font = getFont(ImGuiX::Fonts::FontRole::Body)) {
            ImGui::PushFont(body_font);
            ImGui::TextUnformatted("This line explicitly pushes Body role font.");
            ImGui::PopFont();
        }

        ImGui::Spacing();
        ImGui::TextUnformatted("Pattern:");
        ImGui::BulletText("ImFont* f = getFont(FontRole::H1)");
        ImGui::BulletText("if (f) { ImGui::PushFont(f); ImGui::Text(...); ImGui::PopFont(); }");

        ImGui::End();
        ImGui::PopID();
    }
};

class FontSwitchWindow : public ImGuiX::WindowInstance {
public:
    FontSwitchWindow(int id, ImGuiX::ApplicationContext& app, std::string name)
        : WindowInstance(id, app, std::move(name)) {}

    void onInit() override {
        createController<FontSwitchController>();
        create(900, 600);

        fontsBeginManual();
        fontsSetRangesPreset("Default+Cyrillic+Punct+LatinExtA");
        fontsAddBody({ "NotoSans-Regular.ttf", 18.0f });
        fontsAddHeadline(ImGuiX::Fonts::FontRole::H1, { "NotoSans-Bold.ttf", 30.0f });
        if (!fontsBuildNow()) {
            std::cerr << "font_switch_demo: warning: failed to build fonts atlas\n";
        }
    }
};

int main() {
    ImGuiX::Application app;
    app.createWindow<FontSwitchWindow>("Font Switch");
    app.run();
    return 0;
}

