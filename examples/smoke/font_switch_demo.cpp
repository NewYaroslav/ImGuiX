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
        ImGui::TextUnformatted("Body: Cafe deja vu, Azuolas, Privet, ---, ...");
        ImGui::TextUnformatted(
            u8"Glyph sample: Caf\u00E9 d\u00E9j\u00E0 vu | \u0104\u017Euolas | \u041F\u0440\u0438\u0432\u0435\u0442 | \u2014 \u2026");
        ImGui::TextUnformatted(u8"extra_glyphs sample: Status \u2713 | 21\u2103");

        ImGui::SeparatorText("Switch to H1");
        if (ImFont* h1_font = getFont(ImGuiX::Fonts::FontRole::H1)) {
            ImGui::PushFont(h1_font);
            ImGui::TextUnformatted(
                u8"H1: Caf\u00E9 d\u00E9j\u00E0 vu | \u0104\u017Euolas | \u041F\u0440\u0438\u0432\u0435\u0442");
            ImGui::PopFont();
        } else {
            ImGui::TextUnformatted("H1 font is unavailable. Fallback to current font.");
        }

        ImGui::SeparatorText("Back to Body");
        ImGui::TextUnformatted(u8"After PopFont(): back to Body with Cyrillic, Latin Extended-A, and punctuation.");

        if (ImFont* body_font = getFont(ImGuiX::Fonts::FontRole::Body)) {
            ImGui::PushFont(body_font);
            ImGui::TextUnformatted(
                u8"Explicit Body role: Caf\u00E9, \u0104\u017Euolas, \u041F\u0440\u0438\u0432\u0435\u0442, \u2014, \u2026");
            ImGui::PopFont();
        }

        ImGui::Spacing();
        ImGui::TextUnformatted("Manual atlas pattern:");
        ImGui::BulletText("fontsSetRangesPreset(\"Default+Cyrillic+LatinExtA+0x2010-0x205E\")");
        ImGui::BulletText("symbols_font.extra_glyphs = u8\"\\u2713\\u2103\"");
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
        fontsSetRangesPreset("Default+Cyrillic+LatinExtA+0x2010-0x205E");

        ImGuiX::Fonts::FontFile body_font{};
        body_font.path = "NotoSans-Regular.ttf";
        body_font.size_px = 18.0f;

        ImGuiX::Fonts::FontFile symbols_font{};
        symbols_font.path = "NotoSansSymbols2-Regular.ttf";
        symbols_font.size_px = 18.0f;
        symbols_font.merge = true;
        symbols_font.extra_glyphs = u8"\u2713\u2103";

        fontsAddBody(body_font);
        fontsAddMerge(ImGuiX::Fonts::FontRole::Emoji, symbols_font);
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
