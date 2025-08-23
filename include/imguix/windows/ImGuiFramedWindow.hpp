#pragma once
#ifndef _IMGUIX_WINDOWS_IMGUI_FRAMED_WINDOW_HPP_INCLUDED
#define _IMGUIX_WINDOWS_IMGUI_FRAMED_WINDOW_HPP_INCLUDED

/// \file ImGuiFramedWindow.hpp
/// \brief Window implementation with custom title bar and control buttons.

#include "window_flags.hpp"
#include <imgui.h>

namespace ImGuiX::Windows {

    /// \brief Configuration parameters for ImGuiFramedWindow.
    struct ImGuiFramedWindowConfig {
        int min_width = 640;         ///< Minimum window width in pixels.
        int min_height = 480;        ///< Minimum window height in pixels.
        int frame_corner_radius = 8; ///< Radius of the outer window corners.
        int resize_border = 8;       ///< Thickness of the manual resize border.
        int title_bar_height = 32;   ///< Height of the custom title bar.
        const char* close_button_text = u8"X##imguix_btn_close"; ///< Label for the close button.
        const char* minimize_button_text = u8"_##imguix_btn_minimize"; ///< Label for the minimize button.
        const char* maximize_button_text = u8"[]##imguix_btn_maximize"; ///< Label for the maximize button.
        ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); ///< Background clear color.
    };

    /// \brief Window with custom ImGui-styled title bar and buttons.
    class ImGuiFramedWindow : public WindowInstance {
    public:
        /// \brief Constructs a framed window instance.
        /// \param id        Unique window identifier.
        /// \param app       Reference to the owning application.
        /// \param name      Internal window name.
        /// \param title     Title displayed in the custom frame.
        /// \param flags     Window behavior flags.
        /// \param config    Configuration parameters.
        ImGuiFramedWindow(
            int id,
            ApplicationContext& app,
            std::string name,
            std::string title,
            WindowFlags flags =
                WindowFlags::ShowControlButtons |
                WindowFlags::ImGuiStyledControlButtons,
            ImGuiFramedWindowConfig config = {});

        ~ImGuiFramedWindow() override = default;

        /// \brief Create backend resources and open the window.
        /// \return True if creation succeeds.
        bool create() override;

        /// \brief Create the window with the specified size.
        /// \param w Width in pixels.
        /// \param h Height in pixels.
        /// \return True if creation succeeds.
        bool create(int w, int h) override;

        /// \brief Optional override for drawing a custom menu bar.
        virtual void drawMenuBar() {};

        /// \brief Updates window logic each frame.
        void tick() override;

        /// \brief Draws the ImGui interface for this window.
        void drawUi() override;

        /// \brief Enable or disable clearing the background between frames.
        /// \param disable True to skip clearing the background.
        void setDisableBackground(bool disable) override;

    protected:
        std::string m_title;                  ///< Text displayed on the title bar.
        WindowFlags m_flags = WindowFlags::None; ///< Current window flags.
        ImGuiFramedWindowConfig m_config;        ///< Runtime configuration values.
        bool m_disable_background = false;       ///< Skip clearing the background when true.
#       ifdef IMGUIX_USE_SFML_BACKEND
        int m_prev_width = -1;   ///< Width before entering fullscreen.
        int m_prev_height = -1;  ///< Height before entering fullscreen.
#       ifdef _WIN32
        RECT m_minimize_btn_rect = {0}; ///< Rectangle of the minimize button.
        RECT m_maximize_btn_rect = {0}; ///< Rectangle of the maximize button.
        RECT m_close_btn_rect = {0};    ///< Rectangle of the close button.
        bool m_in_manual_sizing = false; ///< True while resizing manually.
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

        /// \brief Draw control buttons in the title bar.
        /// \param title_padding_x Horizontal title padding.
        void drawControlButtons(float title_padding_x);

        /// \brief Draw macOS-styled control buttons.
        /// \param title_padding_x Horizontal title padding.
        void drawMacStyledControlButtons(float title_padding_x);

        /// \brief Draw ImGui-styled control buttons.
        /// \param title_padding_x Horizontal title padding.
        void drawImGuiStyledControlButtons(float title_padding_x);

        /// \brief Render frame manually when required by backend.
        void renderFrameManually();
    };

} // namespace ImGuiX::Windows


#ifdef IMGUIX_HEADER_ONLY
#   ifdef IMGUIX_USE_SFML_BACKEND
#       include "SfmlImGuiFramedWindow.ipp"
#   elif defined(IMGUIX_USE_GLFW_BACKEND)
#       include "GlfwImGuiFramedWindow.ipp"
#   elif defined(IMGUIX_USE_SDL2_BACKEND)
#       include "Sdl2ImGuiFramedWindow.ipp"
#   endif
#include "ImGuiFramedWindow.ipp"
#endif

#endif // _IMGUIX_WINDOWS_IMGUI_FRAMED_WINDOW_HPP_INCLUDED
