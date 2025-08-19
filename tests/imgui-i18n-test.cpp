#if defined(IMGUIX_USE_SFML_BACKEND)
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#elif defined(IMGUIX_USE_GLFW_BACKEND)
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#elif defined(IMGUIX_USE_SDL2_BACKEND)
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL.h>
#elif
// Без бэкенда
#endif

#ifdef IMGUI_ENABLE_FREETYPE
#include "imgui_freetype.h"
#endif

#include <iostream>
#include <imguix/core.hpp>
#include <imgui.h>
#include <memory>
#include <fmt/core.h>
#include <imguix/utils/i18n/LangStore.hpp>

namespace i18n = ImGuiX::Utils::I18N;

// Простая отрисовка многострочного текста по ключу (с фолбэком)
// Показываем заголовок (локализованный short-string) и содержимое .md
static void DrawMultilineDoc(i18n::LangStore& store, const char* key, float height = 180.0f) {
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

bool LoadFontsForLanguage(const char* lang, float px) {
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
    auto eq=[](const char*a,const char*b){for(;*a&&*b;++a,++b) if((unsigned char)std::tolower(*a)!=(unsigned char)std::tolower(*b)) return false; return *a==*b;};
    if (eq(lang,"ru")||eq(lang,"uk")||eq(lang,"be")) {
        b.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    } else 
    if (eq(lang,"vi")) {
        b.AddRanges(io.Fonts->GetGlyphRangesVietnamese());
    }
    ImVector<ImWchar> ranges; b.BuildRanges(&ranges);

    ImFontConfig cfg{}; cfg.SizePixels = 0.0f;        // не перетирать px
    ImFont* base = io.Fonts->AddFontFromFileTTF(path.c_str(), px, &cfg, ranges.Data);
    if (!base) return false;
    io.FontDefault = base;

    if (!ImGui::SFML::UpdateFontTexture()) return false;
    return true;
}

class I18nController : public ImGuiX::Controller {
public:
    I18nController(ImGuiX::WindowControl& window,
                   ImGuiX::Application& app,
                   std::shared_ptr<i18n::LangStore> lang_store)
        : Controller(window)
        , m_app(app)
        , m_lang_store(std::move(lang_store)) {}

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
            ImGui::Text("Language: %s", m_lang_store->language().c_str());
        }
        ImGui::Separator();

        // Short strings
        ImGui::Text("Short strings:");

        ImGui::BulletText("%s", m_lang_store->text("Menu.File").c_str());
        ImGui::BulletText("%s", m_lang_store->text("Menu.Edit").c_str());
        ImGui::BulletText("%s", m_lang_store->text("Polyglot.Example").c_str()); // in both
        ImGui::BulletText("%s", m_lang_store->text("Polyglot.OnlyEn").c_str());  // en-only

        // Settings (more common than named pipes)
        ImGui::Separator();
        ImGui::Text("Settings:");
        static char username[64] = "guest";
        ImGui::InputText(m_lang_store->label("Settings.General.Username"), username, IM_ARRAYSIZE(username));

        static int server_idx = 0; // 0: demo.local, 1: test.example.com, 2: Custom
        const char* presets[] = {"demo.local", "test.example.com", "Custom"};
        if (ImGui::Combo(m_lang_store->label("Settings.Network.Server"), &server_idx, presets, IM_ARRAYSIZE(presets))) {
            // no-op
        }
        static char custom_server[128] = "";
        if (server_idx == 2) {
            ImGui::InputText(m_lang_store->label("Settings.Network.Server.Custom"), custom_server, IM_ARRAYSIZE(custom_server));
        }

        // Formatting (errors + version)
        ImGui::Separator();
        ImGui::Text("Formatting:");
        const char* host = (server_idx == 2 && custom_server[0]) ? custom_server : presets[server_idx];
        ImGui::BulletText("%s",
            m_lang_store->textf_key("Errors.ServerUnreachable", fmt::arg("host", host), 503).c_str());
        ImGui::BulletText("%s",
            m_lang_store->textf_key("Build.Version", 1, 2, 3).c_str());

        // Pluralization
        ImGui::Separator();
        ImGui::Text("Pluralization:");
        ImGui::InputInt("items n", &m_items_count);   if (m_items_count < 0) m_items_count = 0;
        ImGui::SameLine();
        ImGui::Text("%s",
            m_lang_store->textf_plural("Items", m_items_count, fmt::arg("n", m_items_count)).c_str());
        ImGui::InputInt("notifications n", &m_notif_count); if (m_notif_count < 0) m_notif_count = 0;
        ImGui::SameLine();
        ImGui::Text("%s",
            m_lang_store->textf_plural("Notifications", m_notif_count, fmt::arg("n", m_notif_count)).c_str());

        // Markdown docs (headings from JSON, body from md/)
        ImGui::Separator();
        DrawMultilineDoc(*m_lang_store, "Docs.GettingStarted", 180.0f);
        ImGui::Dummy(ImVec2(0, 6));
        DrawMultilineDoc(*m_lang_store, "About.App", 140.0f);

        // Button/label caching demo
        ImGui::Separator();
        if (ImGui::Button(m_lang_store->label("Buttons.ResetDefaults"))) {
            // no-op
        }
        ImGui::Text("Label ptr: %p", (const void*)m_lang_store->label("Menu.File"));
        ImGui::SameLine();
        ImGui::Text("again: %p", (const void*)m_lang_store->label("Menu.File"));

        ImGui::End();
        ImGui::PopFont();
        //if (window().id() == 0) ImGui::ShowDemoWindow();
        ImGui::PopID();
    }

private:
    ImGuiX::Application&                 m_app;
    std::shared_ptr<i18n::LangStore>     m_lang_store;
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
    I18nWindow(int id, ImGuiX::ApplicationControl& app, std::string name,
               std::shared_ptr<i18n::LangStore> lang_store)
        : WindowInstance(id, app, std::move(name))
        , m_lang_store(std::move(lang_store)) {}

    void onInit() override {
        createController<I18nController>(static_cast<ImGuiX::Application&>(m_application), m_lang_store);
        create(id() == 0 ? 800 : 640, id() == 0 ? 600 : 480);
        setWindowIcon("data/resources/icons/icon.png");
        LoadFontsForLanguage("ru", 32.0f);
    }

private:
    std::shared_ptr<i18n::LangStore> m_lang_store;
    std::string m_pending_lang;
    
    void requestLanguageChange(const std::string& lang) override {
        if (m_pending_lang != lang && !lang.empty()) {
            m_pending_lang = lang;
            m_lang_store->set_language(lang);
        } else
        if (m_pending_lang.empty()) {
            m_pending_lang = "en";
            m_lang_store->set_language("en");
        }
    };
};

int main() {
    auto lang_store = std::make_shared<i18n::LangStore>();
    lang_store->set_language("en");
    
    ImGuiX::Application app;
    app.createWindow<I18nWindow>("Main Window", lang_store);
    app.run();

    return 0;
}
