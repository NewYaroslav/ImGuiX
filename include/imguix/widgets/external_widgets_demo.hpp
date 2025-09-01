#ifndef _IMGUIX_WIDGETS_EXTERNAL_WIDGETS_DEMO_HPP_INCLUDED
#define _IMGUIX_WIDGETS_EXTERNAL_WIDGETS_DEMO_HPP_INCLUDED

#ifdef IMGUIX_DEMO

#include <imgui.h>

#ifdef IMGUI_ENABLE_IMPLOT
#include <implot.h>
#endif

#ifdef IMGUI_ENABLE_IMPLOT3D
#include <implot3d.h>
#endif

#ifdef IMGUI_ENABLE_IMNODEFLOW
#include <ImNodeFlow.h>
#endif

#ifdef IMGUI_ENABLE_IMGUIFILEDIALOG
#include <ImGuiFileDialog.h>
#endif

#ifdef IMGUI_ENABLE_PFD
#include <portable-file-dialogs.h>
#endif

#ifdef IMGUI_ENABLE_IMCMD
#include <imcmd_command_palette.h>
#endif

#ifdef IMGUI_ENABLE_IMSPINNER
#include <imspinner.h>
#endif

#ifdef IMGUI_ENABLE_IMTEXTEDITOR
#include <ImGuiColorTextEdit.h>
#endif

namespace ImGuiX {
namespace Widgets {

#ifdef IMGUI_ENABLE_IMNODEFLOW
class SimpleSum final : public ImFlow::BaseNode {
public:
    SimpleSum() {
        setTitle("Simple sum");
        setStyle(ImFlow::NodeStyle::green());
        addIN<int>("A", 0, ImFlow::ConnectionFilter::SameType());
        addOUT<int>("Result", nullptr)
            ->behaviour([this]() { return getInVal<int>("A") + b_; });
    }
    void draw() override {
        ImGui::SetNextItemWidth(100.0f);
        ImGui::InputInt("B", &b_);
    }
private:
    int b_ = 0;
};

static ImFlow::ImNodeFlow g_editor;

inline void DrawNodeEditorFrame() {
    static bool inited = false;
    if (!inited) {
        g_editor.placeNodeAt<SimpleSum>(ImVec2(120, 80));
        inited = true;
    }
    ImGui::Begin("Node Editor");
    g_editor.update();
    ImGui::End();
}
#endif

inline void DemoExternalWidgets() {
    static bool show_imgui_demo = false;
#ifdef IMGUI_ENABLE_IMPLOT
    static bool show_implot_demo = false;
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
    static bool show_implot3d_demo = false;
#endif
#ifdef IMGUI_ENABLE_IMNODEFLOW
    static bool show_imnodeflow_demo = false;
#endif
#ifdef IMGUI_ENABLE_IMGUIFILEDIALOG
    static bool show_igfd = false;
#endif
#ifdef IMGUI_ENABLE_PFD
    static bool show_pfd = false;
#endif
#ifdef IMGUI_ENABLE_IMSPINNER
    static bool show_imspinner_demo = false;
#endif
#ifdef IMGUI_ENABLE_IMTEXTEDITOR
    static bool show_text_editor = false;
#endif

#ifdef IMGUI_ENABLE_IMCMD
    static bool show_cmd_palette = false;
#endif

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Demo")) {
            if (ImGui::MenuItem("ImGui", nullptr, false, !show_imgui_demo)) show_imgui_demo = true;
#ifdef IMGUI_ENABLE_IMPLOT
            if (ImGui::MenuItem("ImPlot", nullptr, false, !show_implot_demo)) show_implot_demo = true;
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
            if (ImGui::MenuItem("ImPlot3D", nullptr, false, !show_implot3d_demo)) show_implot3d_demo = true;
#endif
#ifdef IMGUI_ENABLE_IMNODEFLOW
            if (ImGui::MenuItem("ImNodeFlow", nullptr, false, !show_imnodeflow_demo)) show_imnodeflow_demo = true;
#endif
#ifdef IMGUI_ENABLE_IMGUIFILEDIALOG
            if (ImGui::MenuItem("File Dialog", nullptr, false, !show_igfd)) show_igfd = true;
#endif
#ifdef IMGUI_ENABLE_PFD
            if (ImGui::MenuItem("File Dialog (pfd)", nullptr, false, !show_pfd)) show_pfd = true;
#endif
#ifdef IMGUI_ENABLE_IMCMD
            if (ImGui::MenuItem("Command Palette", "Ctrl+Shift+P", false, !show_cmd_palette)) show_cmd_palette = true;
#endif
#ifdef IMGUI_ENABLE_IMSPINNER
            if (ImGui::MenuItem("ImSpinner", nullptr, false, !show_imspinner_demo)) show_imspinner_demo = true;
#endif
#ifdef IMGUI_ENABLE_IMTEXTEDITOR
            if (ImGui::MenuItem("Code Editor", nullptr, false, !show_text_editor)) show_text_editor = true;
#endif
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (show_imgui_demo) ImGui::ShowDemoWindow(&show_imgui_demo);
#ifdef IMGUI_ENABLE_IMPLOT
    if (show_implot_demo) ImPlot::ShowDemoWindow(&show_implot_demo);
#endif
#ifdef IMGUI_ENABLE_IMPLOT3D
    if (show_implot3d_demo) ImPlot3D::ShowDemoWindow(&show_implot3d_demo);
#endif
#ifdef IMGUI_ENABLE_IMNODEFLOW
    if (show_imnodeflow_demo) {
        ImGui::Begin("ImNodeFlow Demo", &show_imnodeflow_demo);
        DrawNodeEditorFrame();
        ImGui::End();
    }
#endif
#ifdef IMGUI_ENABLE_IMGUIFILEDIALOG
    if (show_igfd) {
        ImGui::Begin("ImGuiFileDialog", &show_igfd);
        if (ImGui::Button("Open...")) {
            IGFD::FileDialogConfig cfg;
            cfg.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("OpenDlg", "Open File", ".*", cfg);
        }
        if (ImGuiFileDialog::Instance()->Display("OpenDlg")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                const std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
                (void)path;
            }
            ImGuiFileDialog::Instance()->Close();
        }
        ImGui::End();
    }
#endif
#ifdef IMGUI_ENABLE_PFD
    if (show_pfd) {
        ImGui::Begin("portable-file-dialogs", &show_pfd);
        static std::string last_open, last_save;
        if (ImGui::Button("Open...")) {
            auto sel = pfd::open_file("Open file",
                                      ".",
                                      { "All files", "*" },
                                      pfd::opt::multiselect).result();
            if (!sel.empty()) last_open = sel.front();
        }
        ImGui::SameLine();
        ImGui::TextDisabled("%s", last_open.c_str());
        if (ImGui::Button("Save As...")) {
            last_save = pfd::save_file("Save file", ".", { "Text", "*.txt", "All files", "*" }).result();
        }
        ImGui::SameLine();
        ImGui::TextDisabled("%s", last_save.c_str());
        if (ImGui::Button("Message box")) {
            pfd::message("Notice", "Operation finished.", pfd::choice::ok, pfd::icon::info).result();
        }
        ImGui::End();
    }
#endif
#ifdef IMGUI_ENABLE_IMSPINNER
    if (show_imspinner_demo) {
        ImGui::Begin("ImSpinner Demo", &show_imspinner_demo);
#ifdef IMSPINNER_DEMO
        ImSpinner::demoSpinners();
#else
        ImGui::TextDisabled("Build without IMSPINNER_DEMO");
#endif
        ImGui::End();
    }
#endif
#ifdef IMGUI_ENABLE_IMTEXTEDITOR
    static ImTextEdit::TextEditor s_editor;
    if (show_text_editor) {
        static bool init = false;
        if (!init) {
            s_editor.SetLanguageDefinition(ImTextEdit::CPlusPlus());
            s_editor.SetPalette(ImTextEdit::GetDarkPalette());
            s_editor.SetShowWhitespaces(false);
            s_editor.SetText("// Hello from ImGuiColorTextEdit\nint main(){return 0;}\n");
            init = true;
        }
        ImGui::Begin("Code Editor", &show_text_editor);
        s_editor.Render("TextEditor");
        ImGui::End();
    }
#endif
#ifdef IMGUI_ENABLE_IMCMD
    if (show_cmd_palette) {
        // 1) Создай/держи состояние палитры (объект/синглтон) где-то вне кадра
        // 2) Зарегистрируй команды (один раз)
        // 3) Вызови функцию отрисовки палитры внутри окна/поверх всего
        //    (см. examples/ в репо — там готовая схема регистрации и отображения)
        // Примерно: palette.OpenOnShortcut(Ctrl+Shift+P); palette.Render(...);
    }
#endif
}

} // namespace Widgets
} // namespace ImGuiX

#endif // IMGUIX_DEMO

#endif
