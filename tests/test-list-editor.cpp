#include <imgui.h>

#include <imguix/widgets/list_editor.hpp>
#include <vector>
#include <string>

int main() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::NewFrame();

    std::vector<std::string> strings {"a", "b"};
    std::vector<int> numbers {1, 2};

    ImGuiX::Widgets::ListEditor("strings", strings);
    ImGuiX::Widgets::ListEditor("numbers", numbers);

    ImGui::Render();
    ImGui::DestroyContext();
    return 0;
}
