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

#ifdef IMGUI_ENABLE_IMCOOLBAR
#include <ImCoolBar.h>
#endif

#ifdef IMGUI_ENABLE_IMSPINNER
#include <imspinner.h>
#endif

#ifdef IMGUI_ENABLE_IMTEXTEDITOR
#include <ImGuiColorTextEdit.h>
#endif

#ifdef IMGUI_ENABLE_IMGUI_MD
#include <imgui_md.h>
#endif


namespace ImGuiX::Widgets {

#   ifdef IMGUI_ENABLE_IMNODEFLOW
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
#   endif

#   ifdef IMGUI_ENABLE_IMCMD
namespace {
    inline bool show_cmd_palette = false;
    // Ensure context exists (CreateContext also binds it as current if none)
    inline ImCmd::Context* ImCmdCtx() {
        ImCmd::Context* ctx = ImCmd::GetCurrentContext();
        if (!ctx) ctx = ImCmd::CreateContext();
        return ctx;
    }

    // Register commands once
    inline void ImCmdRegisterCommands() {
        static bool inited = false;
        if (inited) return;
        inited = true;

        (void)ImCmdCtx(); // make sure context is ready

        ImCmd::AddCommand(ImCmd::Command{
            "Open Settings",
            [] { /* open your settings panel */ },
            {}, {}, // SubsequentCallback / TerminatingCallback not used here
        });

        ImCmd::AddCommand(ImCmd::Command{
            "Toggle Theme",
            [] { /* switch theme */ },
            {}, {},
        });

        ImCmd::AddCommand(ImCmd::Command{
            "Help: About",
            [] { /* show about modal */ },
            {}, {},
        });

        // Example with options via Prompt + SubsequentCallback
        ImCmd::AddCommand(ImCmd::Command{
            "Switch Layout",
            [] {
                ImCmd::Prompt({"Classic", "Dark", "Cyber-Y2K"}); // shows options
            },
            [](int idx) {
                // idx is 0..N-1 depending on selection
                // apply layout by idx
            },
            {}, // optional TerminatingCallback
        });
    }
} // namespace

    inline void DrawImCmdDemo() {
        ImCmdRegisterCommands();

        // VSCode-like hotkey: Ctrl+Shift+P
        if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_P))
            show_cmd_palette = true;

        if (show_cmd_palette) {
            // Window helper with close toggle
            ImCmd::CommandPaletteWindow("Command Palette", &show_cmd_palette);
            // Or bare widget:
            // ImCmd::CommandPalette("Command Palette");
        }
    }
#   endif

#ifdef IMGUI_ENABLE_IMGUI_MD
namespace {
    
    // Минимальный принтер: один шрифт и открытие ссылок через платформенный вызов при желании
    struct MarkdownPrinter : imgui_md {
        ImFont* get_font() const override { return ImGui::GetFont(); }
        void open_url() const override {
            // Опционально: открыть m_href. Для SDL2 есть SDL_OpenURL; иначе - по платформе.
            // ImGuiX окружение: можешь прокинуть свой callback/сервис.
        }
    };

    inline bool show_md_demo = false;
    inline MarkdownPrinter s_md;
} // namespace
#endif


    // --- demo helpers ---
    static bool CoolBarDrawButton(const char* label) {
        // Размер берём из бара — квадрат w×w
        const float w = ImGui::GetCoolBarItemWidth();
        const ImVec2 size(w, w);

        ImGui::PushID(label);
        ImGui::InvisibleButton("btn", size);           // лэйаут + инпут (без собственной отрисовки)
        const bool pressed = ImGui::IsItemClicked();

        // Кастомная отрисовка: без округления координат => субпиксельное движение видно
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();

        const ImU32 col_bg = ImGui::GetColorU32(ImGui::IsItemActive()   ? ImGuiCol_ButtonActive :
                                                ImGui::IsItemHovered()  ? ImGuiCol_ButtonHovered :
                                                                           ImGuiCol_Button);
        const ImU32 col_bd = ImGui::GetColorU32(ImGuiCol_Border);
        const float r = ImGui::GetStyle().FrameRounding;

        dl->AddRectFilled(p0, p1, col_bg, r);
        dl->AddRect(p0, p1, col_bd, r);

        // Текст по центру (шрифт у ImGui обычно снапится к пикселю — это нормально)
        const ImVec2 ts = ImGui::CalcTextSize(label);
        const ImVec2 c  = (p0 + p1) * 0.5f;
        dl->AddText(ImVec2(c.x - ts.x * 0.5f, c.y - ts.y * 0.5f), ImGui::GetColorU32(ImGuiCol_Text), label);

        ImGui::PopID();
        return pressed;
    }


    /// \berif
    inline void DemoExternalWidgets() {
        static bool show_imgui_demo = false;
#   ifdef IMGUI_ENABLE_IMPLOT
        static bool show_implot_demo = false;
#   endif
#   ifdef IMGUI_ENABLE_IMPLOT3D
        static bool show_implot3d_demo = false;
#   endif
#   ifdef IMGUI_ENABLE_IMNODEFLOW
        static bool show_imnodeflow_demo = false;
#   endif
#   ifdef IMGUI_ENABLE_IMGUIFILEDIALOG
        static bool show_igfd = false;
#   endif
#   ifdef IMGUI_ENABLE_PFD
        static bool show_pfd = false;
#   endif
#   ifdef IMGUI_ENABLE_IMSPINNER
        static bool show_imspinner_demo = false;
#   endif
#   ifdef IMGUI_ENABLE_IMTEXTEDITOR
        static bool show_text_editor = false;
#   endif

#   ifdef IMGUI_ENABLE_IMCOOLBAR
        static bool show_coolbar_demo = false;
#   endif

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Demo")) {
                if (ImGui::MenuItem("ImGui", nullptr, false, !show_imgui_demo)) show_imgui_demo = true;
#               ifdef IMGUI_ENABLE_IMPLOT
                if (ImGui::MenuItem("ImPlot", nullptr, false, !show_implot_demo)) show_implot_demo = true;
#               endif
#               ifdef IMGUI_ENABLE_IMPLOT3D
                if (ImGui::MenuItem("ImPlot3D", nullptr, false, !show_implot3d_demo)) show_implot3d_demo = true;
#               endif
#               ifdef IMGUI_ENABLE_IMNODEFLOW
                if (ImGui::MenuItem("ImNodeFlow", nullptr, false, !show_imnodeflow_demo)) show_imnodeflow_demo = true;
#               endif
#               ifdef IMGUI_ENABLE_IMGUIFILEDIALOG
                if (ImGui::MenuItem("File Dialog", nullptr, false, !show_igfd)) show_igfd = true;
#               endif
#               ifdef IMGUI_ENABLE_PFD
                if (ImGui::MenuItem("File Dialog (pfd)", nullptr, false, !show_pfd)) show_pfd = true;
#               endif
#               ifdef IMGUI_ENABLE_IMCMD
                if (ImGui::MenuItem("Command Palette", "Ctrl+Shift+P", false, !show_cmd_palette)) show_cmd_palette = true;
#               endif
#               ifdef IMGUI_ENABLE_IMSPINNER
                if (ImGui::MenuItem("ImSpinner", nullptr, false, !show_imspinner_demo)) show_imspinner_demo = true;
#               endif
#               ifdef IMGUI_ENABLE_IMTEXTEDITOR
                if (ImGui::MenuItem("Code Editor", nullptr, false, !show_text_editor)) show_text_editor = true;
#               endif
#               ifdef IMGUI_ENABLE_IMCOOLBAR
                if (ImGui::MenuItem("ImCoolBar", nullptr, false, !show_coolbar_demo)) show_coolbar_demo = true;
#               endif
#               ifdef IMGUI_ENABLE_IMGUI_MD
                if (ImGui::MenuItem("Markdown (imgui_md)", nullptr, false, !show_md_demo)) show_md_demo = true;
#               endif

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (show_imgui_demo) ImGui::ShowDemoWindow(&show_imgui_demo);

#       ifdef IMGUI_ENABLE_IMPLOT
        if (show_implot_demo) ImPlot::ShowDemoWindow(&show_implot_demo);
#       endif

#       ifdef IMGUI_ENABLE_IMPLOT3D
        if (show_implot3d_demo) ImPlot3D::ShowDemoWindow(&show_implot3d_demo);
#       endif

#       ifdef IMGUI_ENABLE_IMNODEFLOW
        if (show_imnodeflow_demo) {
            ImGui::Begin("ImNodeFlow Demo", &show_imnodeflow_demo);
            DrawNodeEditorFrame();
            ImGui::End();
        }
#       endif

#       ifdef IMGUI_ENABLE_IMGUIFILEDIALOG
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
#       endif

#       ifdef IMGUI_ENABLE_PFD
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
#       endif

#       ifdef IMGUI_ENABLE_IMSPINNER
        if (show_imspinner_demo) {
            ImGui::Begin("ImSpinner Demo", &show_imspinner_demo);
#           ifdef IMSPINNER_DEMO
            ImSpinner::demoSpinners();
#           else
            ImGui::TextDisabled("Build without IMSPINNER_DEMO");
#           endif
            ImGui::End();
        }
#       endif

#       ifdef IMGUI_ENABLE_IMTEXTEDITOR
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
#       endif

#       ifdef IMGUI_ENABLE_IMCMD
        DrawImCmdDemo();
#       endif

#       ifdef IMGUI_ENABLE_IMCOOLBAR
        if (show_coolbar_demo) {
            ImGui::Begin("ImCoolBar Demo", &show_coolbar_demo);

            auto coolbar_button = [] (const char* label) {
                //float w = ImGui::GetCoolBarItemWidth();
                //bool pressed = ImGui::Button(label, ImVec2(w, w));
                bool pressed = CoolBarDrawButton(label);
                return pressed;
            };

            // Общие настройки сглаживания
            ImCoolBarConfig common;
            //common.mouse_ema_half_life_ms = 40.0f;
            //common.anim_ema_half_life_ms  = 40.0f;

            // 1) Горизонтальный бар снизу (центр)
            {
                ImCoolBarConfig cfg = common;
                cfg.anchor = ImVec2(0.5f, 1.0f); // bottom-center of viewport
                if (ImGui::BeginCoolBar("##CoolBarBottom", ImCoolBarFlags_Horizontal, cfg)) {
                    const char* labels = "ABCDEFGHIJKL";
                    for (const char* p = labels; *p; ++p)
                        if (ImGui::CoolBarItem())
                            (void)coolbar_button(std::string(1, *p).c_str());
                    ImGui::EndCoolBar();
                }
            }

            // 2) Вертикальный бар слева (по центру)
            {
                ImCoolBarConfig cfg = common;
                cfg.anchor = ImVec2(0.0f, 0.5f); // left-center
                if (ImGui::BeginCoolBar("##CoolBarLeft", ImCoolBarFlags_Vertical, cfg)) {
                    const char* labels = "123456789";
                    for (const char* p = labels; *p; ++p)
                        if (ImGui::CoolBarItem())
                            (void)coolbar_button(std::string(1, *p).c_str());
                    ImGui::EndCoolBar();
                }
            }

            // 3) Вертикальный бар справа (по центру)
            {
                ImCoolBarConfig cfg = common;
                cfg.anchor = ImVec2(1.0f, 0.5f); // right-center
                if (ImGui::BeginCoolBar("##CoolBarRight", ImCoolBarFlags_Vertical, cfg)) {
                    const char* labels = "!@#$%^&*";
                    for (const char* p = labels; *p; ++p)
                        if (ImGui::CoolBarItem())
                            (void)coolbar_button(std::string(1, *p).c_str());
                    ImGui::EndCoolBar();
                }
            }

            ImGui::End();
        }
#       endif

#       ifdef IMGUI_ENABLE_IMGUI_MD
        if (show_md_demo) {
            ImGui::Begin("Markdown Demo", &show_md_demo);
            static const char* kText =
                "# Header 1\n"
                "Some **bold**, _italic_, ~~strike~~, and a [link](https://example.com).\n"
                "\n"
                "| Col1 | Col2 |\n"
                "| ---  | ---: |\n"
                "| A    |  42  |\n";
            s_md.print(kText, kText + strlen(kText));  // print(str, str_end)
            ImGui::End();
        }
#       endif

    } // DemoExternalWidgets()

} // namespace ImGuiX::Widgets

#endif // IMGUIX_DEMO

#endif
