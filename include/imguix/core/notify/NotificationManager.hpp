#pragma once
#ifndef _IMGUIX_CORE_NOTIFY_NOTIFICATION_MANAGER_HPP_INCLUDED
#define _IMGUIX_CORE_NOTIFY_NOTIFICATION_MANAGER_HPP_INCLUDED

/// \file NotificationManager.hpp
/// \brief Toast notification system for ImGuiX windows.
/// \author Original implementation by TyomaVader (https://github.com/TyomaVader/ImGuiNotify)
/// \author Modified by NewYaroslav

#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <cstdarg>
#include <cstdint>

#include <imgui.h>
#include <imgui_internal.h>

#include <imguix/config/notifications.hpp>

namespace ImGuiX::Notify {

    constexpr int kMaxMsgLength       = 4096;
    constexpr float kPaddingX         = 20.0f;
    constexpr float kPaddingY         = 20.0f;
    constexpr float kPaddingMessageY  = 10.0f;
    constexpr int kFadeInOutTimeMs    = 150;
    constexpr int kDefaultDismissMs   = 3000;
    constexpr float kOpacity          = 0.8f;
    constexpr bool kUseSeparator      = false;
    constexpr bool kUseDismissButton  = true;
    constexpr int kRenderLimit        = 5;
    constexpr bool kRenderOutsideMainWindow = true; ///< requires docking multi-viewport

    constexpr ImGuiWindowFlags kDefaultWindowFlags =
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoFocusOnAppearing;

    enum class Type : std::uint8_t {
        None,
        Success,
        Warning,
        Error,
        Info,
        COUNT
    };

    enum class Phase : std::uint8_t {
        FadeIn,
        Wait,
        FadeOut,
        Expired,
        COUNT
    };

    /// \brief Toast notification entity.
    class Notification {
    public:
        Notification(Type type, int dismiss_ms = kDefaultDismissMs);
        Notification(Type type, const char* fmt, ...);
        Notification(Type type, int dismiss_ms, const char* fmt, ...);

        void setTitle(const char* fmt, ...);
        void setContent(const char* fmt, ...);
        void setType(Type type) { m_type = type; }
        void setWindowFlags(ImGuiWindowFlags flags) { m_flags = flags; }
        void setOnButtonPress(const std::function<void()>& fn) { m_on_button = fn; }
        void setButtonLabel(const char* fmt, ...);

        const char* title() const;
        const char* defaultTitle() const;
        Type type() const { return m_type; }
        ImVec4 color() const;
        char* content();
        Phase phase() const;
        float fadePercent() const;
        ImGuiWindowFlags windowFlags() const { return m_flags; }
        const std::function<void()>& onButtonPress() const { return m_on_button; }
        const char* buttonLabel() const { return m_button_label; }

    private:
        ImGuiWindowFlags m_flags = kDefaultWindowFlags;
        Type m_type = Type::None;
        char m_title[kMaxMsgLength]{};
        char m_content[kMaxMsgLength]{};
        int m_dismiss_ms = kDefaultDismissMs;
        std::chrono::system_clock::time_point m_created = std::chrono::system_clock::now();
        std::function<void()> m_on_button = nullptr;
        char m_button_label[kMaxMsgLength]{};
    };

    /// \brief Configuration for NotificationManager icons.
    struct IconConfig {
        const char* icon_success = IMGUIX_NOTIFY_ICON_SUCCESS;
        const char* icon_warning = IMGUIX_NOTIFY_ICON_WARNING;
        const char* icon_error   = IMGUIX_NOTIFY_ICON_ERROR;
        const char* icon_info    = IMGUIX_NOTIFY_ICON_INFO;
        const char* icon_close   = IMGUIX_NOTIFY_ICON_CLOSE;
    };

    /// \brief Manager that queues and renders notifications.
    class NotificationManager {
    public:
        NotificationManager() = default;
        explicit NotificationManager(IconConfig cfg) : m_icons(cfg) {}

        void push(Notification toast);
        void render();
        void clear();

        IconConfig& icons() { return m_icons; }
        const IconConfig& icons() const { return m_icons; }

    private:
        void remove(std::size_t index);

        std::vector<Notification> m_notifications{};
        IconConfig m_icons{};
    };

    // ---- Inline implementations ----

    inline Notification::Notification(Type type, int dismiss_ms)
        : m_type(type), m_dismiss_ms(dismiss_ms) {}

    inline Notification::Notification(Type type, const char* fmt, ...)
        : Notification(type) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(m_content, sizeof(m_content), fmt, args);
        va_end(args);
    }

    inline Notification::Notification(Type type, int dismiss_ms, const char* fmt, ...)
        : Notification(type, dismiss_ms) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(m_content, sizeof(m_content), fmt, args);
        va_end(args);
    }

    inline void Notification::setTitle(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(m_title, sizeof(m_title), fmt, args);
        va_end(args);
    }

    inline void Notification::setContent(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(m_content, sizeof(m_content), fmt, args);
        va_end(args);
    }

    inline void Notification::setButtonLabel(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(m_button_label, sizeof(m_button_label), fmt, args);
        va_end(args);
    }

    inline const char* Notification::title() const {
        return m_title;
    }

    inline const char* Notification::defaultTitle() const {
        if (!m_title[0]) {
            switch (m_type) {
            case Type::Success: return "Success";
            case Type::Warning: return "Warning";
            case Type::Error:   return "Error";
            case Type::Info:    return "Info";
            default:            return nullptr;
            }
        }
        return m_title;
    }

    inline ImVec4 Notification::color() const {
        switch (m_type) {
        case Type::Success: return IMGUIX_NOTIFY_COLOR_SUCCESS;
        case Type::Warning: return IMGUIX_NOTIFY_COLOR_WARNING;
        case Type::Error:   return IMGUIX_NOTIFY_COLOR_ERROR;
        case Type::Info:    return IMGUIX_NOTIFY_COLOR_INFO;
        default:            return IMGUIX_NOTIFY_COLOR_DEFAULT;
        }
    }

    inline char* Notification::content() {
        return m_content;
    }

    inline Phase Notification::phase() const {
        const int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - m_created).count();
        if (elapsed > kFadeInOutTimeMs + m_dismiss_ms + kFadeInOutTimeMs) {
            return Phase::Expired;
        }
        if (elapsed > kFadeInOutTimeMs + m_dismiss_ms) {
            return Phase::FadeOut;
        }
        if (elapsed > kFadeInOutTimeMs) {
            return Phase::Wait;
        }
        return Phase::FadeIn;
    }

    inline float Notification::fadePercent() const {
        const Phase ph = phase();
        const int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - m_created).count();
        if (ph == Phase::FadeIn) {
            return (static_cast<float>(elapsed) / kFadeInOutTimeMs) * kOpacity;
        }
        if (ph == Phase::FadeOut) {
            return (1.0f - (static_cast<float>(elapsed) - kFadeInOutTimeMs - m_dismiss_ms) /
                    kFadeInOutTimeMs) * kOpacity;
        }
        return kOpacity;
    }

    inline void NotificationManager::push(Notification toast) {
        m_notifications.push_back(std::move(toast));
    }

    inline void NotificationManager::remove(std::size_t index) {
        m_notifications.erase(m_notifications.begin() + static_cast<std::ptrdiff_t>(index));
    }

    inline void NotificationManager::clear() {
        m_notifications.clear();
    }

    inline void NotificationManager::render() {
        const ImVec2 main_window_size = ImGui::GetMainViewport()->Size;
        float height = 0.0f;
        for (std::size_t i = 0; i < m_notifications.size(); ++i) {
            Notification& toast = m_notifications[i];
            if (toast.phase() == Phase::Expired) {
                remove(i);
                --i;
                continue;
            }
            if (kRenderLimit > 0 && i > static_cast<std::size_t>(kRenderLimit)) {
                continue;
            }

            const char* title = toast.title();
            const char* content = toast.content();
            const char* default_title = toast.defaultTitle();
            const float opacity = toast.fadePercent();

            ImVec4 text_color = toast.color();
            text_color.w = opacity;

            char window_name[50];
            std::snprintf(window_name, 50, "##TOAST%zu", i);
            ImGui::SetNextWindowBgAlpha(opacity);

#if kRenderOutsideMainWindow
            short main_monitor_id = static_cast<ImGuiViewportP*>(ImGui::GetMainViewport())->PlatformMonitor;
            ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
            ImGuiPlatformMonitor& monitor = platform_io.Monitors[main_monitor_id];
            ImGui::SetNextWindowPos(
                {monitor.WorkPos.x + monitor.WorkSize.x - kPaddingX,
                 monitor.WorkPos.y + monitor.WorkSize.y - kPaddingY - height},
                ImGuiCond_Always,
                {1.0f, 1.0f});
#else
            ImVec2 main_pos = ImGui::GetMainViewport()->Pos;
            ImGui::SetNextWindowPos(
                {main_pos.x + main_window_size.x - kPaddingX,
                 main_pos.y + main_window_size.y - kPaddingY - height},
                ImGuiCond_Always,
                {1.0f, 1.0f});
#endif

            if (!kUseDismissButton && !toast.onButtonPress()) {
                toast.setWindowFlags(kDefaultWindowFlags | ImGuiWindowFlags_NoInputs);
            }

            ImGui::Begin(window_name, nullptr, toast.windowFlags());
            ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
            ImGui::PushTextWrapPos(main_window_size.x / 3.0f);

            bool title_rendered = false;
            const char* icon = nullptr;
            switch (toast.type()) {
            case Type::Success: icon = m_icons.icon_success; break;
            case Type::Warning: icon = m_icons.icon_warning; break;
            case Type::Error:   icon = m_icons.icon_error; break;
            case Type::Info:    icon = m_icons.icon_info; break;
            default: break;
            }
            if (icon && icon[0]) {
                ImGui::TextColored(text_color, "%s", icon);
                title_rendered = true;
            }

            if (title && title[0]) {
                if (title_rendered) ImGui::SameLine();
                ImGui::Text("%s", title);
                title_rendered = true;
            } else if (default_title) {
                if (title_rendered) ImGui::SameLine();
                ImGui::Text("%s", default_title);
                title_rendered = true;
            }

            if (kUseDismissButton) {
                if (title_rendered || (content && content[0])) {
                    ImGui::SameLine();
                }
                float scale = 0.8f;
                if (content && ImGui::CalcTextSize(content).x > ImGui::GetContentRegionAvail().x) {
                    scale = 0.8f;
                }
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetWindowSize().x - ImGui::GetCursorPosX()) * scale);
                if (ImGui::Button(m_icons.icon_close)) {
                    remove(i);
                    ImGui::PopTextWrapPos();
                    ImGui::End();
                    height += kPaddingMessageY;
                    --i;
                    continue;
                }
            }

            if (title_rendered && content && content[0]) {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
            }

            if (content && content[0]) {
                if (title_rendered && kUseSeparator) {
                    ImGui::Separator();
                }
                ImGui::Text("%s", content);
            }

            if (toast.onButtonPress()) {
                if (ImGui::Button(toast.buttonLabel())) {
                    toast.onButtonPress()();
                }
            }

            ImGui::PopTextWrapPos();
            height += ImGui::GetWindowHeight() + kPaddingMessageY;
            ImGui::End();
        }
    }

} // namespace ImGuiX::Notify

#endif // _IMGUIX_CORE_NOTIFY_NOTIFICATION_MANAGER_HPP_INCLUDED
