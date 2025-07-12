#pragma once
#ifndef _IMGUIX_WINDOWS_IMGUI_FRAMED_WINDOW_HPP_INCLUDED
#define _IMGUIX_WINDOWS_IMGUI_FRAMED_WINDOW_HPP_INCLUDED

#include "window_flags.hpp"
#include <imgui.h>

namespace ImGuiX::Windows {
    
    /// \brief
    struct ImGuiFramedWindowConfig {
        int min_width = 640;         ///<
        int min_height = 480;        ///<
        int frame_corner_radius = 8; ///<
        int resize_border = 8;       ///<
        int title_bar_height = 32;   ///<
        const char* close_button_text = "X##imguix_btn_close";
        const char* minimize_button_text = "_##imguix_btn_minimize";
        const char* maximize_button_text = "[]##imguix_btn_maximize";
    };

    /// \brief Window with custom ImGui-styled title bar and buttons.
    class ImGuiFramedWindow : public WindowInstance {
    public:
        ImGuiFramedWindow(
            int id, 
            ApplicationControl& app, 
            std::string name, 
            std::string title,
            WindowFlags flags = 
                WindowFlags::ShowControlButtons |
                WindowFlags::ImGuiStyledControlButtons, 
            ImGuiFramedWindowConfig config = {});
        ~ImGuiFramedWindow() override = default;
        bool create() override;
        bool create(int w, int h) override;
        virtual void drawMenuBar() {};
        void tick() override;
        void drawUi() override;

    protected:
        std::string m_title; ///< Text displayed on the title bar
        WindowFlags m_flags = WindowFlags::None;
        ImGuiFramedWindowConfig m_config;
#       ifdef IMGUIX_USE_SFML_BACKEND
        int m_prev_width = -1;
        int m_prev_height = -1;
#       ifdef _WIN32
        RECT m_minimize_btn_rect = {0};
        RECT m_maximize_btn_rect = {0};
        RECT m_close_btn_rect = {0};
        bool m_in_manual_sizing = false;
        void setupWindowEffects(HWND hwnd);
        bool applyCommonWindowSetup();
        void applyRoundedRegion(HWND hwnd, int width, int height, int radius);
        static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                     UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
#       endif
    public:
        LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#       endif
    
    protected:
        /// \brief Draws the title bar text. Can be overridden to add icons or change alignment.
        virtual void drawTitleBarText();
        void drawControlButtons(float title_padding_x);
        void drawMacStyledControlButtons(float title_padding_x);
        void drawImGuiStyledControlButtons(float title_padding_x);
        void renderFrameManually();
    };

} // namespace ImGuiX::Windows


#ifdef IMGUIX_HEADER_ONLY
#   ifdef IMGUIX_USE_SFML_BACKEND
#       include "SfmlImGuiFramedWindow.ipp"
#   elif defined(IMGUIX_USE_GLFW_BACKEND)

#   elif defined(IMGUIX_USE_SDL2_BACKEND)

#   endif
#include "ImGuiFramedWindow.ipp"
#endif

#endif // _IMGUIX_WINDOWS_IMGUI_FRAMED_WINDOW_HPP_INCLUDED