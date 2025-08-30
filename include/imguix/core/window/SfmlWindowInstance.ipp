#ifdef _WIN32
#   include <windows.h>
#   include <SFML/Window/WindowHandle.hpp>
#endif
#ifdef __linux__
#   include <X11/Xlib.h>
#   include <X11/Xutil.h>
#   ifdef None
#       undef None // prevents conflicts with enums and sf::Style::None
#   endif
#   ifdef Bool
#       undef Bool
#   endif
#   ifdef Status
#       undef Status
#   endif
#endif

#include <imguix/utils/path_utils.hpp>

namespace ImGuiX {

    WindowInstance::~WindowInstance() noexcept  {
        saveIniNow();
#       ifdef IMGUI_ENABLE_IMPLOT
        if (m_implot_ctx) {
            ImPlot::SetCurrentContext(m_implot_ctx);
            ImPlot::DestroyContext(m_implot_ctx);
            m_implot_ctx = nullptr;
        }
#       endif

#       ifdef IMGUI_ENABLE_IMPLOT3D
        if (m_implot3d_ctx) {
            ImPlot3D::SetCurrentContext(m_implot3d_ctx);
            ImPlot3D::DestroyContext(m_implot3d_ctx);
            m_implot3d_ctx = nullptr;
        }
#       endif
        ImGui::SFML::Shutdown(m_window);
    }

    bool WindowInstance::create() {
        if (m_window.isOpen() || m_is_open) return true;
        m_window.create(sf::VideoMode({static_cast<unsigned int>(width()),  static_cast<unsigned int>(height())}), name());
        m_window.setFramerateLimit(60);
        m_is_open = ImGui::SFML::Init(m_window);
        ImGui::SFML::SetCurrentWindow(m_window);

#       ifdef IMGUI_ENABLE_IMPLOT
        m_implot_ctx = ImPlot::CreateContext();
        ImPlot::SetCurrentContext(m_implot_ctx);
#       endif

#       ifdef IMGUI_ENABLE_IMPLOT3D
        m_implot3d_ctx = ImPlot3D::CreateContext();
        ImPlot3D::SetCurrentContext(m_implot3d_ctx);
#       endif

#       ifdef _WIN32
        HWND hwnd = m_window.getNativeHandle();
        if (GetForegroundWindow() != hwnd) {
            SetForegroundWindow(hwnd);
            SetFocus(hwnd);
            SetActiveWindow(hwnd);
        }
#       endif

        return m_is_open;
    }
    
    bool WindowInstance::create(int w, int h) {
        m_width = w;
        m_height = h;
        return create();
    }
    
    bool WindowInstance::setWindowIcon(const std::string& path) {
        std::string icon_path = Utils::resolveExecPath(path);
        sf::Image icon;
        if (!icon.loadFromFile(icon_path))
            return false;
        m_window.setIcon(icon);
        return true;
    }

    void WindowInstance::handleEvents() {
        setCurrentWindow();
        while (const auto event = m_window.pollEvent()) {
            ImGui::SFML::ProcessEvent(m_window, *event);
            if (event->is<sf::Event::Closed>() && m_window.isOpen()) {
                Events::WindowClosedEvent evt(id(), name());
                notify(evt);
                m_window.close();
            }
        }
    }

    void WindowInstance::tick() {
        setCurrentWindow();
        auto& res = registry().getResource<DeltaClockSfml>();
        ImGui::SFML::Update(m_window, res.delta());
        m_window.clear();
        updateCurrentTheme();
    }
    
    void WindowInstance::present() {
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
    
// --- WindowInterface interface ---

    void WindowInstance::setSize(int w, int h) {
        m_width = w;
        m_height = h;
#       ifdef IMGUIX_USE_SFML_BACKEND
        if (m_window.isOpen()) {
            m_window.setSize(sf::Vector2u(w, h));
        }
#       endif
    }
    
    void WindowInstance::restore() {
        if (!m_window.isOpen()) return;
#       ifdef _WIN32
        HWND hwnd = m_window.getNativeHandle();
        if (hwnd) ShowWindow(hwnd, SW_RESTORE);
#       endif
#       ifdef __linux__
        auto handle = m_window.getNativeHandle();
        Display* display = XOpenDisplay(nullptr);
        if (display && handle) {
            XMapRaised(display, handle);
            XFlush(display);
            XCloseDisplay(display);
        }
#       endif
    }

    void WindowInstance::minimize() {
        if (!m_window.isOpen()) return;
#       ifdef _WIN32
        HWND hwnd = m_window.getNativeHandle();
        if (hwnd) ShowWindow(hwnd, SW_MINIMIZE);
#       endif
#       ifdef __linux__
        auto handle = m_window.getNativeHandle();
        Display* display = XOpenDisplay(nullptr);
        if (display && handle) {
            XIconifyWindow(display, handle, DefaultScreen(display));
            XFlush(display);
            XCloseDisplay(display);
        }
#       endif
    }

    void WindowInstance::maximize() {
        if (!m_window.isOpen()) return;
#       ifdef _WIN32
        HWND hwnd = m_window.getNativeHandle();
        if (hwnd) ShowWindow(hwnd, SW_MAXIMIZE);
#       endif
#       ifdef __linux__
        auto handle = m_window.getNativeHandle();
        Display* display = XOpenDisplay(nullptr);
        if (!display || !handle)
            return;

        Atom wmState = XInternAtom(display, u8"_NET_WM_STATE", False);
        Atom maxHorz = XInternAtom(display, u8"_NET_WM_STATE_MAXIMIZED_HORZ", False);
        Atom maxVert = XInternAtom(display, u8"_NET_WM_STATE_MAXIMIZED_VERT", False);

        XEvent xev{};
        xev.type = ClientMessage;
        xev.xclient.window = handle;
        xev.xclient.message_type = wmState;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = 1; // 1 = add, 0 = remove
        xev.xclient.data.l[1] = maxHorz;
        xev.xclient.data.l[2] = maxVert;
        xev.xclient.data.l[3] = 0; // source indication (0 = application)
        xev.xclient.data.l[4] = 0;

        XSendEvent(display, DefaultRootWindow(display), False,
                   SubstructureRedirectMask | SubstructureNotifyMask, &xev);

        XFlush(display);
        XCloseDisplay(display);
#       endif
    }
    
    void WindowInstance::close() {
        m_is_open = false;
        if (m_window.isOpen()) {
            Events::WindowClosedEvent evt(id(), name());
            notify(evt);
            m_window.close();
            //ImGui::SFML::Shutdown(m_window);
        }
    }
    
    bool WindowInstance::isMaximized() const {
#       ifdef _WIN32
        HWND hwnd = m_window.getNativeHandle();
        if (::IsZoomed(hwnd)) return true;
        return false;
#       elif defined(__linux__)
        auto handle = m_window.getNativeHandle();
        Display* display = XOpenDisplay(nullptr);
        if (!display || !handle)
            return false;

        Atom wmState = XInternAtom(display, u8"_NET_WM_STATE", False);
        Atom maxHorz = XInternAtom(display, u8"_NET_WM_STATE_MAXIMIZED_HORZ", False);
        Atom maxVert = XInternAtom(display, u8"_NET_WM_STATE_MAXIMIZED_VERT", False);
        Atom actualType;
        int actualFormat;
        unsigned long nitems, bytesAfter;
        unsigned char* prop = nullptr;
        bool maximized = false;

        if (Success == XGetWindowProperty(display, handle, wmState, 0, 1024, False,
                                          AnyPropertyType, &actualType, &actualFormat,
                                          &nitems, &bytesAfter, &prop)) {
            if (prop) {
                Atom* atoms = reinterpret_cast<Atom*>(prop);
                for (unsigned long i = 0; i < nitems; ++i) {
                    if (atoms[i] == maxHorz || atoms[i] == maxVert) {
                        maximized = true;
                        break;
                    }
                }
                XFree(prop);
            }
        }
        XCloseDisplay(display);
        return maximized;
#       else
        return false;
#       endif
    }

    void WindowInstance::toggleMaximizeRestore() {
#       ifdef _WIN32
        HWND hwnd = m_window.getNativeHandle();
        if (isMaximized()) {
            ::ShowWindow(hwnd, SW_RESTORE);
        } else {
            ::ShowWindow(hwnd, SW_MAXIMIZE);
        }
#       elif defined(__linux__)
        auto handle = m_window.getNativeHandle();
        Display* display = XOpenDisplay(nullptr);
        if (!display || !handle)
            return;

        Atom wmState = XInternAtom(display, u8"_NET_WM_STATE", False);
        Atom maxHorz = XInternAtom(display, u8"_NET_WM_STATE_MAXIMIZED_HORZ", False);
        Atom maxVert = XInternAtom(display, u8"_NET_WM_STATE_MAXIMIZED_VERT", False);

        XEvent xev{};
        xev.type = ClientMessage;
        xev.xclient.window = handle;
        xev.xclient.message_type = wmState;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = isMaximized() ? 0 : 1;
        xev.xclient.data.l[1] = maxHorz;
        xev.xclient.data.l[2] = maxVert;
        xev.xclient.data.l[3] = 0;
        xev.xclient.data.l[4] = 0;

        XSendEvent(display, DefaultRootWindow(display), False,
                   SubstructureRedirectMask | SubstructureNotifyMask, &xev);
        XFlush(display);
        XCloseDisplay(display);
#       endif
    }
    
    bool WindowInstance::setActive(bool active) {
        if (m_window.isOpen()) {
            m_is_active = m_window.setActive(active);
        }
        return m_is_active;
    }

    bool WindowInstance::isActive() const {
        if (m_window.isOpen()) {
            return m_window.hasFocus();
        }
        return m_is_active;
    }
    
    void WindowInstance::setVisible(bool visible) {
        m_is_visible = visible;
        if (m_window.isOpen()) {
            m_window.setVisible(visible);
        }
    }

    bool WindowInstance::isOpen() const {
        return m_window.isOpen() && m_is_open;
    }

    void WindowInstance::setCurrentWindow() {
        if (m_window.setActive(true)) {
            ImGui::SFML::SetCurrentWindow(m_window);
#           ifdef IMGUI_ENABLE_IMPLOT
            if (m_implot_ctx) {
                ImPlot::SetCurrentContext(m_implot_ctx);
            }
#           endif
#           ifdef IMGUI_ENABLE_IMPLOT3D
            if (m_implot3d_ctx) {
                ImPlot3D::SetCurrentContext(m_implot3d_ctx);
            }
#           endif
        }
    }
}
