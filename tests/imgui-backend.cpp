#if defined(IMGUIX_USE_SFML_BACKEND)

#include <imgui.h> // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_freetype.h>
#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({640, 480}), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    
    if (!ImGui::SFML::Init(window)) return -1;
    
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->FontLoader = ImGuiFreeType::GetFontLoader();
    io.Fonts->FontLoaderFlags = ImGuiFreeTypeBuilderFlags_NoHinting;
    
    ImFont* font = io.Fonts->AddFontFromFileTTF("data/resources/fonts/Roboto-Medium.ttf", 18.0f);
    io.FontDefault = font;

    if (!ImGui::SFML::UpdateFontTexture()) return -1;

    std::printf("Fonts count: %d\n", io.Fonts->Fonts.Size);

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::ShowDemoWindow();

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        window.clear();
        window.draw(shape);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

#elif defined(IMGUIX_USE_GLFW_BACKEND)

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_freetype.h>
#include <imgui_stdlib.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

int main() {
    if (!glfwInit())
        return -1;
    GLFWwindow* window = glfwCreateWindow(640, 480, "ImGui GLFW", nullptr, nullptr);
    if (!window)
        return -1;
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

#elif defined(IMGUIX_USE_SDL2_BACKEND)

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL.h>
#include <SDL_opengles2.h>
#include <imgui_freetype.h>
#ifdef __EMSCRIPTEN__
#   include <emscripten.h>
#   ifndef SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR
#       define SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR "SDL_EMSCRIPTEN_CANVAS_SELECTOR"
#   endif
#endif

static SDL_Window* g_window = nullptr;
static SDL_GLContext g_gl_ctx = nullptr;
static bool g_running = true;
static float g_font_scale = 1.0f;

#ifdef __EMSCRIPTEN__
extern "C" void EMSCRIPTEN_KEEPALIVE resize_sdl_window(int width, int height) {
    if (g_window) {
        glViewport(0, 0, width, height);
    }
}
#endif

static void main_loop(void*) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);

        if (event.type == SDL_QUIT) {
#           ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
#           endif
            g_running = false;
        } else 
        if (event.type == SDL_WINDOWEVENT && 
            event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			int w, h, dw, dh;
			SDL_GetWindowSize(g_window, &w, &h); // Физические размеры окна
			SDL_GL_GetDrawableSize(g_window, &dw, &dh); // Реальные размеры для рендеринга
			// Получаем DPI экрана
			float dpi;
			SDL_GetDisplayDPI(0, &dpi, nullptr, nullptr);  // 0 — индекс дисплея, на котором отображается окно
			// Масштабирование шрифта с учётом DPI
			//g_font_scale = static_cast<float>(dh) / static_cast<float>(h) * (dpi / 96.0f);  // 96.0f — стандарт DPI
			g_font_scale = static_cast<float>(dh) / static_cast<float>(h);
			printf("g_font_scale: %f\n", g_font_scale);
		}
    }

    int draw_w, draw_h;
    SDL_GL_GetDrawableSize(g_window, &draw_w, &draw_h);
    glViewport(0, 0, draw_w, draw_h);

    int ww, wh;
    SDL_GetWindowSize(g_window, &ww, &wh);
    ImGuiIO& io = ImGui::GetIO();
    io.DisplayFramebufferScale = ImVec2((float)draw_w / ww, (float)draw_h / wh);

    //printf("Window: %dx%d | Drawable: %dx%d\n", ww, wh, w, h);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    ImGui::GetIO().FontGlobalScale = g_font_scale * 0.5f;
    //ImGui::GetStyle().ScaleAllSizes(1.1);

    ImGui::ShowDemoWindow();
    ImGui::Render();
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(g_window);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return -1;

#   ifdef __EMSCRIPTEN__
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR, "#canvas");
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0"); // включаем high-DPI
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
#   endif

    g_window = SDL_CreateWindow(
        "ImGui SDL2", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        640, 480, 
        SDL_WINDOW_OPENGL | 
        SDL_WINDOW_RESIZABLE | 
        SDL_WINDOW_ALLOW_HIGHDPI);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    
    g_gl_ctx = SDL_GL_CreateContext(g_window);
    SDL_GL_MakeCurrent(g_window, g_gl_ctx);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    int window_w, window_h, drawable_w, drawable_h;
    SDL_GetWindowSize(g_window, &window_w, &window_h);
    SDL_GL_GetDrawableSize(g_window, &drawable_w, &drawable_h);
    io.DisplayFramebufferScale = ImVec2(
        (float)drawable_w / window_w,
        (float)drawable_h / window_h
    );
    
    // Устанавливаем флаг поддержки vtx offset
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    // Настройка шрифта с учётом HiDPI
    float scale = io.DisplayFramebufferScale.y;
    //float font_size = 13.0f * scale * 2.0f;
	float font_size = 13.0f * 2.0f;
    
    ImFontConfig config;
    config.OversampleH = 2;
    config.OversampleV = 2;
    config.PixelSnapH = true;
    //config.FontLoaderFlags |= ImGuiFreeTypeLoaderFlags_Monochrome | ImGuiFreeTypeLoaderFlags_MonoHinting;
    config.SizePixels = font_size;
    io.Fonts->AddFontFromFileTTF("data/resources/fonts/Roboto-Light.ttf", font_size, &config);
    io.FontGlobalScale = 0.5f;

#   ifdef __EMSCRIPTEN__
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_Button];
	style.Colors[ImGuiCol_HeaderHovered] = style.Colors[ImGuiCol_Header];
	style.Colors[ImGuiCol_FrameBgHovered] = style.Colors[ImGuiCol_FrameBg];
	style.Colors[ImGuiCol_SliderGrabActive] = style.Colors[ImGuiCol_SliderGrab];
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = style.Colors[ImGuiCol_ScrollbarGrab];
	style.Colors[ImGuiCol_TabHovered] = style.Colors[ImGuiCol_Tab];
#   endif

    // Имплементации
    ImGui_ImplSDL2_InitForOpenGL(g_window, g_gl_ctx);
    ImGui_ImplOpenGL3_Init("#version 100");

#   ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, nullptr, 0, true);
#   else
    while (g_running) {
        main_loop(nullptr);
    }
#   endif

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(g_gl_ctx);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}

#else

int main() {}

#endif
