#ifdef _WIN32
#include <windowsx.h>
#include <commctrl.h>
#include <Dwmapi.h>     // For window transparency: https://gist.github.com/Alia5/5d8c48941d1f73c1ef14967a5ffe33d5
#endif
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Color.hpp>
#include <imguix/extensions/blend_colors.hpp>
#include <imguix/extensions/color_utils.hpp>

#ifdef _WIN32
#define IMGUIX_WM_USER_ROUNDED_REGION (WM_USER + 201)
#endif

namespace ImGuiX::Windows {

    bool ImGuiFramedWindow::create() {
#       ifdef _WIN32
        if (applyCommonWindowSetup()) {
            HWND hwnd = m_window.getNativeHandle();
            SetWindowSubclass(hwnd, SubclassProc, 1, reinterpret_cast<DWORD_PTR>(this));
            setupWindowEffects(hwnd);
            if (GetForegroundWindow() != hwnd) {
                SetForegroundWindow(hwnd);
                SetFocus(hwnd);
                SetActiveWindow(hwnd);
            }
        }
#       else
        applyCommonWindowSetup();
#       endif
        return m_is_open;
    }

    void ImGuiFramedWindow::tick() {
        setCurrentWindow();
        auto& res = registry().getResource<DeltaClockSfml>();
        ImGui::SFML::Update(m_window, res.delta());
        if (hasFlag(m_flags, WindowFlags::EnableTransparency)) {
            m_window.clear(sf::Color::Transparent);
        } else {
            m_window.clear(Extensions::ColorToSfml(m_config.clear_color));
        }
        updateCurrentTheme();
    }

    void ImGuiFramedWindow::drawUi() {
        setCurrentWindow();
        ImGui::PushID(id());

        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize(ImVec2((float)m_window.getSize().x, (float)m_window.getSize().y));

        ImVec2 char_size = ImGui::CalcTextSize(u8"W");
        ImVec2 padding = ImGui::GetStyle().WindowPadding;
        float title_padding_x = padding.x + char_size.x * 2.0f;

        const ImGuiWindowFlags flags = 
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        if (hasFlag(m_flags, WindowFlags::DisableBackground) || m_disable_background) {
            ImVec4 border_color = ImGui::GetStyle().Colors[ImGuiCol_Border];
            ImVec4 background_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
            ImVec4 new_color = Extensions::BlendColors(border_color, background_color);
            ImGui::PushStyleColor(ImGuiCol_Border, new_color);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(u8"##imguix_framed_window", nullptr, flags);
        ImGui::PopStyleVar();
        
        if (hasFlag(m_flags, WindowFlags::DisableBackground) || m_disable_background) {
            ImGui::PopStyleColor(2);
        }

        // --- Title bar
        ImGui::BeginChild(u8"##imguix_title_bar", ImVec2(0, m_config.title_bar_height), false,
                          ImGuiWindowFlags_NoScrollbar | 
                          ImGuiWindowFlags_NoDecoration);
                 
        {
            ImVec2 p_min = ImGui::GetWindowPos();
            ImVec2 p_max = ImVec2(p_min.x + ImGui::GetWindowWidth(), p_min.y + m_config.title_bar_height);
            ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, ImGui::GetColorU32(ImGuiCol_TitleBgActive));
        }

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + title_padding_x);
        drawTitleBarText();
        
        if (hasFlag(m_flags, WindowFlags::ShowControlButtons)) {
            if (hasFlag(m_flags, WindowFlags::MacStyledControlButtons)) {
                drawMacStyledControlButtons(title_padding_x);
            } else
            if (hasFlag(m_flags, WindowFlags::ImGuiStyledControlButtons)) {
                drawImGuiStyledControlButtons(title_padding_x);
            } else {
                drawControlButtons(title_padding_x);
            }
        }

        ImGui::EndChild();
        
        // --- Menu Bar
        if (hasFlag(m_flags, WindowFlags::HasMenuBar)) {
            ImGui::SetCursorPosY(m_config.title_bar_height);
            ImGui::BeginChild(u8"##imguix_menu_bar", ImVec2(0, 0), false,
                              ImGuiWindowFlags_MenuBar | 
                              ImGuiWindowFlags_NoScrollbar | 
                              ImGuiWindowFlags_NoDecoration |
                              ImGuiWindowFlags_AlwaysAutoResize);
            drawMenuBar();
            ImGui::EndChild();
        }

        // --- Main content
        for (auto& ctrl : m_controllers) {
            ctrl->drawUi();
        }

        ImGui::End();
        
        ImGui::PopID();
    }

#   ifdef _WIN32

    void ImGuiFramedWindow::setupWindowEffects(HWND hwnd) {
        if (hasFlag(m_flags, WindowFlags::EnableTransparency)) {
            // Enable window transparency: https://gist.github.com/Alia5/5d8c48941d1f73c1ef14967a5ffe33d5
            MARGINS margins;
            margins.cxLeftWidth = -1;
            SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
            DwmExtendFrameIntoClientArea(hwnd, &margins);
        }

        int w = static_cast<int>(m_window.getSize().x);
        int h = static_cast<int>(m_window.getSize().y);
        applyRoundedRegion(hwnd, w, h, m_config.frame_corner_radius);
        PostMessage(hwnd, IMGUIX_WM_USER_ROUNDED_REGION, 0, 0);
    }
    
    bool ImGuiFramedWindow::applyCommonWindowSetup() {
        if (m_window.isOpen() || m_is_open) return true;
        m_window.create(sf::VideoMode({static_cast<unsigned int>(width()),  static_cast<unsigned int>(height())}), name(), sf::Style::None);
        m_window.setFramerateLimit(60);
        m_is_open = ImGui::SFML::Init(m_window);

#       ifdef IMGUI_ENABLE_IMPLOT
        m_implot_ctx = ImPlot::CreateContext();
        ImPlot::SetCurrentContext(m_implot_ctx);
#       endif

        return m_is_open;
    }
    
    void ImGuiFramedWindow::applyRoundedRegion(HWND hwnd, int width, int height, int radius) {
        HRGN region = CreateRoundRectRgn(0, 0, width + 1, height + 1, radius * 2, radius * 2);
        SetWindowRgn(hwnd, region, TRUE);
    }
    
    LRESULT CALLBACK ImGuiFramedWindow::SubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                     UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
        ImGuiFramedWindow* self = reinterpret_cast<ImGuiFramedWindow*>(dwRefData);
        if (self) {
            return self->WndProc(hwnd, msg, wParam, lParam);
        }
        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }
    
    LRESULT ImGuiFramedWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (!hasFlag(m_flags, WindowFlags::ShowControlButtons)) {
            return DefSubclassProc(hwnd, msg, wParam, lParam);
        }
        switch (msg) {
            case WM_ENTERSIZEMOVE:
                m_in_manual_sizing = true;
                return DefSubclassProc(hwnd, msg, wParam, lParam);
            case WM_EXITSIZEMOVE:
                m_in_manual_sizing = false;
                return DefSubclassProc(hwnd, msg, wParam, lParam);
            case WM_GETMINMAXINFO: {
                MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
                mmi->ptMinTrackSize.x = m_config.min_width;
                mmi->ptMinTrackSize.y = m_config.min_height;
                return 0;
            }
            case IMGUIX_WM_USER_ROUNDED_REGION: {
                RECT r{};
                if (GetClientRect(hwnd, &r)) {
                    const int new_width = r.right - r.left;
                    const int new_height = r.bottom - r.top;
                    if (new_width != m_prev_width || new_height != m_prev_height) {
                        m_prev_width = new_width;
                        m_prev_height = new_height;
                        applyRoundedRegion(hwnd, new_width, new_height, m_config.frame_corner_radius);
                    }
                }
                return 0;
            }
            case WM_SIZE:
            case WM_SIZING: {
                RECT rect;
                GetClientRect(hwnd, &rect);
                int w = rect.right - rect.left;
                int h = rect.bottom - rect.top;
                m_window.setSize(sf::Vector2u(w, h));
                ImGui::GetIO().DisplaySize = ImVec2((float)w, (float)h);
                if (w != m_prev_width || h != m_prev_height) {
                    m_prev_width = w;
                    m_prev_height = h;
                    applyRoundedRegion(hwnd, w, h, m_config.frame_corner_radius);
                    renderFrameManually();
                }

                return DefSubclassProc(hwnd, msg, wParam, lParam);
            }
            case WM_NCHITTEST: {
                POINT screen_pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                POINT pt = screen_pt;
                ScreenToClient(hwnd, &pt);

                RECT rect;
                GetClientRect(hwnd, &rect);

                bool left   = pt.x < m_config.resize_border;
                bool right  = pt.x >= rect.right - m_config.resize_border;
                bool top    = pt.y < m_config.resize_border;
                bool bottom = pt.y >= rect.bottom - m_config.resize_border;

                if (top && left)     return HTTOPLEFT;
                if (top && right)    return HTTOPRIGHT;
                if (bottom && left)  return HTBOTTOMLEFT;
                if (bottom && right) return HTBOTTOMRIGHT;
                if (left)   return HTLEFT;
                if (right)  return HTRIGHT;
                if (top)    return HTTOP;
                if (bottom) return HTBOTTOM;

                if (PtInRect(&m_minimize_btn_rect, pt)) {
                    return HTCLIENT;
                }
                if (PtInRect(&m_maximize_btn_rect, pt)) {
                    return HTCLIENT;
                }
                if (PtInRect(&m_close_btn_rect, pt)) {
                    return HTCLIENT;
                }

                if (pt.y < m_config.title_bar_height) return HTCAPTION;

                return HTCLIENT;
            }
            default:
                return DefSubclassProc(hwnd, msg, wParam, lParam);
        }
    }
#   elif defined(__linux__)

    bool ImGuiFramedWindow::applyCommonWindowSetup() {
        if (m_window.isOpen() || m_is_open) return true;
        m_window.create(
            sf::VideoMode({static_cast<unsigned int>(width()), static_cast<unsigned int>(height())}),
            name(),
            sf::Style::None);
        m_window.setFramerateLimit(60);
        m_is_open = ImGui::SFML::Init(m_window);

#       ifdef IMGUI_ENABLE_IMPLOT
        m_implot_ctx = ImPlot::CreateContext();
        ImPlot::SetCurrentContext(m_implot_ctx);
#       endif

        return m_is_open;
    }

#   endif

} // namespace ImGuiX::Windows
