#include <iostream>
#include <imguix/core.hpp>

namespace i18n = ImGuiX::I18N;

// Простая отрисовка многострочного текста по ключу (с фолбэком)
// Показываем заголовок (локализованный short-string) и содержимое .md
static void DrawMultilineDoc(const i18n::LangStore& store, const char* key, float height = 180.0f) {
    // Заголовок (короткая строка)
    ImGui::SeparatorText(store.text(key).c_str());
    // Тело (длинный текст, может быть большим) — в скроллируемом Child
    const std::string doc = store.doc(key);
    ImGui::BeginChild((std::string("doc_") + key).c_str(), ImVec2(0, height), ImGuiChildFlags_Border);
    ImGui::PushTextWrapPos(0.0f);
    ImGui::TextUnformatted(doc.c_str());
    ImGui::PopTextWrapPos();
    ImGui::EndChild();
}

bool LoadFonts(float px) {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.FontDefault = nullptr;

#ifdef IMGUI_ENABLE_FREETYPE
# if IMGUI_VERSION_NUM >= 19200
    io.Fonts->FontLoader = ImGuiFreeType::GetFontLoader();
    io.Fonts->FontLoaderFlags = 0;
# else
    io.Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
    io.Fonts->FontBuilderFlags = 0;
# endif
#endif

    const std::string path = ImGuiX::Utils::resolveExecPath("data/resources/fonts/Roboto-Medium.ttf");

    ImFontGlyphRangesBuilder b;
    b.AddRanges(io.Fonts->GetGlyphRangesDefault());
    b.AddText(u8"–—…•“”‘’");
    b.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    b.AddRanges(io.Fonts->GetGlyphRangesVietnamese());
    ImVector<ImWchar> ranges; 
    b.BuildRanges(&ranges);

    ImFontConfig cfg{}; 
    cfg.SizePixels = 0.0f;
    ImFont* base = io.Fonts->AddFontFromFileTTF(path.c_str(), px, &cfg, ranges.Data);
    if (!base) return false;
    io.FontDefault = base;

    if (!ImGui::SFML::UpdateFontTexture()) return false;
    return true;
}

class I18nController : public ImGuiX::Controller {
public:
    I18nController(ImGuiX::WindowControl& window)
        : Controller(window) {}

    void drawContent() override {

    }

    // --- UI demo fully using the new keys (generic settings) ---
    void drawUi() override {
        ImGui::PushID(window().id());
        ImGui::PushFont(nullptr, 18.0f);
        ImGui::Begin(window().id() == 0 ? "i18n main" : "i18n child");
        

        // Languages to test fonts and fallback:
        // ru, uk, be, pt-BR, es, vi, en
        static int s_selected_lang = 0;
        static const char* langs[] = {"en","ru","uk","be","pt-BR","es","vi"};

        if (window().id() == 0) {
            if (ImGui::Combo("Language", &s_selected_lang, langs, IM_ARRAYSIZE(langs))) {
                const char* new_lang = langs[s_selected_lang];
                // Reload font atlas only if language actually changed
                if (m_last_font_lang != new_lang) {
                    postLanguageChange(new_lang);
                    m_last_font_lang = new_lang;
                }
            }
        } else {
            ImGui::Text("Language: %s", langStore().language().c_str());
        }
        ImGui::Separator();

        // Short strings
        ImGui::Text("Short strings:");

        ImGui::BulletText("%s", langStore().text("Menu.File").c_str());
        ImGui::BulletText("%s", langStore().text("Menu.Edit").c_str());
        ImGui::BulletText("%s", langStore().text("Polyglot.Example").c_str()); // in both
        ImGui::BulletText("%s", langStore().text("Polyglot.OnlyEn").c_str());  // en-only

        // Settings (more common than named pipes)
        ImGui::Separator();
        ImGui::Text("Settings:");
        static char username[64] = "guest";
        ImGui::InputText(langStore().label("Settings.General.Username"), username, IM_ARRAYSIZE(username));

        static int server_idx = 0; // 0: demo.local, 1: test.example.com, 2: Custom
        const char* presets[] = {"demo.local", "test.example.com", "Custom"};
        if (ImGui::Combo(langStore().label("Settings.Network.Server"), &server_idx, presets, IM_ARRAYSIZE(presets))) {
            // no-op
        }
        static char custom_server[128] = "";
        if (server_idx == 2) {
            ImGui::InputText(langStore().label("Settings.Network.Server.Custom"), custom_server, IM_ARRAYSIZE(custom_server));
        }

        // Formatting (errors + version)
        ImGui::Separator();
        ImGui::Text("Formatting:");
        const char* host = (server_idx == 2 && custom_server[0]) ? custom_server : presets[server_idx];
        ImGui::BulletText("%s",
            langStore().textf_key("Errors.ServerUnreachable", fmt::arg("host", host), 503).c_str());
        ImGui::BulletText("%s",
            langStore().textf_key("Build.Version", 1, 2, 3).c_str());

        // Pluralization
        ImGui::Separator();
        ImGui::Text("Pluralization:");
        ImGui::InputInt("items n", &m_items_count);   if (m_items_count < 0) m_items_count = 0;
        ImGui::SameLine();
        ImGui::Text("%s",
            langStore().textf_plural("Items", m_items_count, fmt::arg("n", m_items_count)).c_str());
        ImGui::InputInt("notifications n", &m_notif_count); if (m_notif_count < 0) m_notif_count = 0;
        ImGui::SameLine();
        ImGui::Text("%s",
            langStore().textf_plural("Notifications", m_notif_count, fmt::arg("n", m_notif_count)).c_str());

        // Markdown docs (headings from JSON, body from md/)
        ImGui::Separator();
        DrawMultilineDoc(langStore(), "Docs.GettingStarted", 180.0f);
        ImGui::Dummy(ImVec2(0, 6));
        DrawMultilineDoc(langStore(), "About.App", 140.0f);

        // Button/label caching demo
        ImGui::Separator();
        if (ImGui::Button(langStore().label("Buttons.ResetDefaults"))) {
            // no-op
        }
        ImGui::Text("Label ptr: %p", (const void*)langStore().label("Menu.File"));
        ImGui::SameLine();
        ImGui::Text("again: %p", (const void*)langStore().label("Menu.File"));

        ImGui::End();
        ImGui::PopFont();
        //if (window().id() == 0) ImGui::ShowDemoWindow();
        ImGui::PopID();
    }

private:
    std::string                          m_last_font_lang;
    int                                  m_items_count{1};
    int                                  m_notif_count{2};
    
    void postLanguageChange(const std::string& lang, bool apply_to_all = true) {
        ImGuiX::Events::LangChangeEvent evt(lang, apply_to_all, window().id());
        notify(evt);
    }
};

class I18nWindow : public ImGuiX::WindowInstance {
public:
    I18nWindow(int id, 
               ImGuiX::ApplicationControl& app, 
               std::string name)
        : WindowInstance(id, app, std::move(name)) {}

    void onInit() override {
        createController<I18nController>();
        create(id() == 0 ? 800 : 640, id() == 0 ? 600 : 480);
        setWindowIcon("data/resources/icons/icon.png");
		fontsBeginManual();
		//fontsSetLocale("ru");
		fontsSetRangesPreset("Default+Cyrillic+Vietnamese+Punct");
		fontsAddBody({ "Roboto-Medium.ttf", 18.0f });
		fontsAddMerge(ImGuiX::Fonts::FontRole::Icons, { "forkawesome-webfont.ttf", 18.0f, true });
		fontsAddHeadline(ImGuiX::Fonts::FontRole::H1, { "Roboto-Bold.ttf", 24.0f });
		fontsBuildNow();
    }

private:
    std::string m_pending_lang;
    
    void onBeforeLanguageApply(const std::string& lang) override {
        if (m_pending_lang != lang && !lang.empty()) {
            m_pending_lang = lang;
        } else
        if (m_pending_lang.empty()) {
            m_pending_lang = "en";
        }
    };
};

int main() {
    ImGuiX::Application app;
    app.createWindow<I18nWindow>("Main Window");
    app.run();
    return 0;
}
