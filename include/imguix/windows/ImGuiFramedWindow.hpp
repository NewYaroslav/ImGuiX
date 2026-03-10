#pragma once
#ifndef _IMGUIX_WINDOWS_IMGUI_FRAMED_WINDOW_HPP_INCLUDED
#define _IMGUIX_WINDOWS_IMGUI_FRAMED_WINDOW_HPP_INCLUDED

/// \file ImGuiFramedWindow.hpp
/// \brief Framed ImGui window with custom title bar, optional side panel, and backend-specific chrome.
/// \details
/// Typical setup flow:
/// - pass \ref WindowFlags to enable menu bar, control buttons, transparency, or background override;
/// - configure layout via \ref ImGuiFramedWindowConfig (title bar height, side panel width, resize border);
/// - override drawing hooks for custom content:
///   - \ref drawTitleBarText for left title area content;
///   - \ref drawSidePanel for optional side panel content;
///   - \ref drawMenuBar for menu bar actions when \ref WindowFlags::HasMenuBar is set.
///
/// Minimal customization example:
/// \code{.cpp}
/// class MainWindow final : public ImGuiX::Windows::ImGuiFramedWindow {
/// public:
///     MainWindow(int id, ImGuiX::ApplicationContext& app)
///         : ImGuiFramedWindow(
///               id,
///               app,
///               "main_window",
///               "Main Window",
///               ImGuiX::Windows::WindowFlags::HasMenuBar |
///                   ImGuiX::Windows::WindowFlags::DefaultControlButtons,
///               [] {
///                   ImGuiX::Windows::ImGuiFramedWindowConfig cfg{};
///                   cfg.side_panel_width = 56;
///                   cfg.title_bar_height = 34;
///                   return cfg;
///               }()) {}
///
/// protected:
///     void drawMenuBar() override;
///     void drawSidePanel() override;
/// };
/// \endcode

#ifdef _WIN32
#   include <windows.h>
#endif

#include <imgui.h>

#include "window_flags.hpp"

namespace ImGuiX::Windows {

    /// \brief Runtime configuration for framed window geometry and chrome labels.
    /// \details
    /// `side_panel_width <= 0` disables side panel region completely.
    /// `title_bar_height` controls both title region height and default side panel button sizing.
    struct ImGuiFramedWindowConfig {
        int min_width = 640;           ///< Minimum client width in pixels.
        int min_height = 480;          ///< Minimum client height in pixels.
        int frame_corner_radius = 8;   ///< Outer corner radius for native frame integration (where supported).
        int resize_border = 8;         ///< Manual resize hit-test thickness in pixels.
        int title_bar_height = 32;     ///< Custom title bar height in pixels.
        int side_panel_width = 0;      ///< Left side panel width in pixels (`<= 0` disables panel).
        const char* close_button_text = u8"X##imguix_btn_close"; ///< Close button label + hidden ImGui ID suffix.
        const char* minimize_button_text = u8"_##imguix_btn_minimize"; ///< Minimize button label + hidden ImGui ID suffix.
        const char* maximize_button_text = u8"[]##imguix_btn_maximize"; ///< Maximize button label + hidden ImGui ID suffix.
        ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); ///< Clear color used when transparency is disabled.
    };

    /// \brief Custom framed host window rendered through Dear ImGui.
    /// \details
    /// Feature switches are controlled by \ref WindowFlags:
    /// - \ref WindowFlags::HasMenuBar enables menu bar region and calls \ref drawMenuBar.
    /// - \ref WindowFlags::ShowControlButtons shows system controls in title bar.
    /// - \ref WindowFlags::MacStyledControlButtons or \ref WindowFlags::ImGuiStyledControlButtons selects button style.
    /// - \ref WindowFlags::DisableBackground makes root ImGui host transparent while keeping a visible border.
    /// - \ref WindowFlags::EnableTransparency requests transparent native surface on supported backends.
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

        /// \brief Draw custom menu bar content.
        /// \details Called only when \ref WindowFlags::HasMenuBar is enabled.
        virtual void drawMenuBar() {};

        /// \brief Updates window logic each frame.
        void tick() override;

        /// \brief Draws the ImGui interface for this window.
        void drawUi() override;

        /// \brief Enable or disable clearing the backend render target background between frames.
        /// \param disable True to skip clearing the background.
        void setDisableBackground(bool disable) override;

    protected:
        std::string m_title;                  ///< Text displayed on the title bar.
        WindowFlags m_flags = WindowFlags::NoFlags; ///< Current window flags.
        ImGuiFramedWindowConfig m_config;        ///< Runtime configuration values.
        bool m_disable_background = false;       ///< Skip clearing the background when true.
#       ifdef IMGUIX_USE_SFML_BACKEND
        int m_prev_width = -1;   ///< Width before entering fullscreen.
        int m_prev_height = -1;  ///< Height before entering fullscreen.
        bool applyCommonWindowSetup();
#       ifdef _WIN32
        RECT m_minimize_btn_rect = {0}; ///< Rectangle of the minimize button.
        RECT m_maximize_btn_rect = {0}; ///< Rectangle of the maximize button.
        RECT m_close_btn_rect = {0};    ///< Rectangle of the close button.
        bool m_in_manual_sizing = false; ///< True while resizing manually.
        void setupWindowEffects(HWND hwnd);
        void applyRoundedRegion(HWND hwnd, int width, int height, int radius);
        static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                     UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    public:
        LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#       endif
#       endif
    
    protected:
        /// \brief Draw content in the left area of title bar.
        /// \details Override for logo/title text customization. Right-side control buttons are drawn separately.
        virtual void drawTitleBarText();

        /// \brief Draw content inside optional left side panel.
        /// \details Called only when `m_config.side_panel_width > 0`.
        virtual void drawSidePanel() {}

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
