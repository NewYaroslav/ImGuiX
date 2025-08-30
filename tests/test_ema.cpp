#include <imgui.h>
#include <imguix/extensions/ema.hpp>
#include <iostream>

int main() {
    ImGuiContext* ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(ctx);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(100, 100);
    io.Fonts->AddFontDefault();
    io.Fonts->Build();

    ImGui::NewFrame();
    ImGui::Begin("test");

    ImGuiID id = ImGui::GetID("ema");
    ImGuiX::Extensions::Ema::reset(id, 0.0f);
    ImGuiX::Extensions::Ema::update(id, 1.0f);

    float v = ImGuiX::Extensions::Ema::value(id);
    if (!(v > 0.0f && v < 1.0f)) {
        std::cerr << "EMA not between 0 and 1\n";
        return 1;
    }

    ImGuiX::Extensions::Ema::reset(id, 5.0f);
    ImGuiX::Extensions::Ema::update(id, 5.0f);
    if (ImGuiX::Extensions::Ema::value(id) != 5.0f) {
        std::cerr << "EMA reset failed\n";
        return 1;
    }

    ImGui::End();
    ImGui::Render();
    ImGui::DestroyContext(ctx);
    return 0;
}
