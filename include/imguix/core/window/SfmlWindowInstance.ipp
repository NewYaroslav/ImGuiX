#ifdef _WIN32
#   include <windows.h>
#   include <SFML/Window/WindowHandle.hpp>
#endif
#ifdef __linux__
#   include <X11/Xlib.h>
#   include <X11/Xutil.h>
#endif

namespace ImGuiX {

    bool WindowInstance::create() {
        if (m_window.isOpen() || m_is_open) return true;
        m_window.create(sf::VideoMode({static_cast<unsigned int>(width()),  static_cast<unsigned int>(height())}), name());
        m_window.setFramerateLimit(60);
        m_is_open = ImGui::SFML::Init(m_window);
        return m_is_open;
    }
    
    bool WindowInstance::create(int w, int h) {
        m_width = w;
        m_height = h;
        if (m_window.isOpen() || m_is_open) return true;
        m_window.create(sf::VideoMode({static_cast<unsigned int>(width()),  static_cast<unsigned int>(height())}), name());
        m_window.setFramerateLimit(60);
        m_is_open = ImGui::SFML::Init(m_window);
        return m_is_open;
    }

    void WindowInstance::handleEvents() {
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
        auto& res = registry().getResource<DeltaClockSfml>();
        ImGui::SFML::Update(m_window, res.delta());
        m_window.clear();
    }
    
    void WindowInstance::present() {
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
    
// --- WindowControl interface ---

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

        Atom wmState = XInternAtom(display, "_NET_WM_STATE", False);
        Atom maxHorz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
        Atom maxVert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);

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
        }
    }
    
    bool WindowInstance::isMaximized() const {
#       ifdef _WIN32
        HWND hwnd = m_window.getNativeHandle();
        if (::IsZoomed(hwnd)) return true;
#       endif
        return false;
    }
    
    void WindowInstance::toggleMaximizeRestore() {
#       ifdef _WIN32
        HWND hwnd = m_window.getNativeHandle();
        if (isMaximized()) {
            ::ShowWindow(hwnd, SW_RESTORE);
        } else {
            ::ShowWindow(hwnd, SW_MAXIMIZE);
        }
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
}