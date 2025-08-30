#pragma once
#ifndef _IMGUIX_WIDGETS_NOTIFY_NOTIFICATIONS_HPP_INCLUDED
#define _IMGUIX_WIDGETS_NOTIFY_NOTIFICATIONS_HPP_INCLUDED

/// \file notifications.hpp
/// \brief Helpers to post toast notifications via NotificationManager.
/// \details Rendering is handled by the manager (e.g., from Controller loop). This widget does not render.

#include <string>
#include <cstdarg>
#include <utility>
#include <functional>

#include <imguix/core/controller/Controller.hpp>
#include <imguix/core/notify/NotificationManager.hpp>
#include <imguix/utils/vformat.hpp> // ImGuiX::Utils::vformat / vformat_va
#include <fmt/format.h>

namespace ImGuiX::Widgets {

    /// \brief Insert a ready-made notification (copied).
    /// \param ctrl Controller with a NotificationManager.
    /// \param toast Notification to push.
    inline void InsertNotification(Controller* ctrl, const Notify::Notification& toast) {
        if (!ctrl) return;
        ctrl->notifications().push(Notify::Notification(toast));
    }
    
    /// \brief Insert a ready-made notification (moved).
    /// \param ctrl Controller with a NotificationManager.
    /// \param toast Notification to push (rvalue).
    inline void InsertNotification(Controller* ctrl, Notify::Notification&& toast) {
        if (!ctrl) return;
        ctrl->notifications().push(std::move(toast));
    }

    /// \brief Clear all queued notifications.
    /// \param ctrl Controller with a NotificationManager.
    inline void ClearNotifications(Controller* ctrl) {
        if (!ctrl) return;
        ctrl->notifications().clear();
    }

    // -------------------- printf-style helpers (always available) --------------------

    /// \brief Post a notification with formatted content (printf-style).
    /// \param ctrl Controller with a NotificationManager.
    /// \param type Notification type.
    /// \param dismiss_ms Auto-dismiss delay in milliseconds.
    /// \param fmt printf-like format string for content.
    inline void Notifyf(
            Controller* ctrl,
            Notify::Type type,
            int dismiss_ms,
            const char* fmt, ...
        ) {
        if (!ctrl || !fmt) return;
        va_list args; va_start(args, fmt);
        const std::string content = ImGuiX::Utils::vformat_va(fmt, args);
        va_end(args);

        Notify::Notification n(type, dismiss_ms);
        n.setContent("%s", content.c_str());
        InsertNotification(ctrl, std::move(n));
    }

    /// \brief Post a notification with title and formatted content (printf-style).
    /// \param ctrl Controller with a NotificationManager.
    /// \param type Notification type.
    /// \param dismiss_ms Auto-dismiss delay in milliseconds.
    /// \param title Title text (C-string, nullable).
    /// \param fmt printf-like format string for content.
    inline void Notifyf(
            Controller* ctrl,
            Notify::Type type,
            int dismiss_ms,
            const char* title,
            const char* fmt, ...
        ) {
        if (!ctrl || !fmt) return;
        va_list args; va_start(args, fmt);
        const std::string content = ImGuiX::Utils::vformat_va(fmt, args);
        va_end(args);

        Notify::Notification n(type, dismiss_ms);
        if (title && *title) n.setTitle("%s", title);
        n.setContent("%s", content.c_str());
        InsertNotification(ctrl, std::move(n));
    }

    /// \brief Post a notification with a button, title and formatted content (printf-style).
    /// \param ctrl Controller with a NotificationManager.
    /// \param type Notification type.
    /// \param dismiss_ms Auto-dismiss delay in milliseconds.
    /// \param button_label Button label (C-string).
    /// \param on_click Callback executed on button click.
    /// \param title Title text (nullable).
    /// \param fmt printf-like format string for content.
    inline void NotifyWithButtonf(
            Controller* ctrl,
            Notify::Type type,
            int dismiss_ms,
            const char* button_label,
            std::function<void()> on_click,
            const char* title,
            const char* fmt, ...
        ) {
        if (!ctrl || !button_label || !*button_label || !fmt) return;

        va_list args; va_start(args, fmt);
        const std::string content = ImGuiX::Utils::vformat_va(fmt, args);
        va_end(args);

        Notify::Notification n(type, dismiss_ms);
        if (title && *title) n.setTitle("%s", title);
        n.setContent("%s", content.c_str());
        n.setOnButtonPress(std::move(on_click));
        n.setButtonLabel("%s", button_label);
        InsertNotification(ctrl, std::move(n));
    }

    /// \brief Convenience helper: Success (plain C-string).
    inline void NotifySuccess(Controller* ctrl, const char* content, int dismiss_ms = 0) {
        if (!ctrl || !content) return;
        Notify::Notification n(Notify::Type::Success, dismiss_ms);
        n.setContent("%s", content);
        InsertNotification(ctrl, std::move(n));
    }

    /// \brief Convenience helper: Warning (plain C-string).
    inline void NotifyWarning(Controller* ctrl, const char* content, int dismiss_ms = 0) {
        if (!ctrl || !content) return;
        Notify::Notification n(Notify::Type::Warning, dismiss_ms);
        n.setContent("%s", content);
        InsertNotification(ctrl, std::move(n));
    }

    /// \brief Convenience helper: Error (plain C-string).
    inline void NotifyError(Controller* ctrl, const char* content, int dismiss_ms = 0) {
        if (!ctrl || !content) return;
        Notify::Notification n(Notify::Type::Error, dismiss_ms);
        n.setContent("%s", content);
        InsertNotification(ctrl, std::move(n));
    }

    /// \brief Convenience helper: Info (plain C-string).
    inline void NotifyInfo(Controller* ctrl, const char* content, int dismiss_ms = 0) {
        if (!ctrl || !content) return;
        Notify::Notification n(Notify::Type::Info, dismiss_ms);
        n.setContent("%s", content);
        InsertNotification(ctrl, std::move(n));
    }

    // -------------------- fmt-style helpers (optional) --------------------

    /// \brief Post a notification using fmt with compile-time checked format.
    /// \tparam Args Format argument types.
    /// \param ctrl Controller with a NotificationManager.
    /// \param type Notification type.
    /// \param dismiss_ms Auto-dismiss delay in milliseconds.
    /// \param fmt_s Format string.
    /// \param args Format arguments.
    template<class... Args>
    inline void NotifyFmt(
            Controller* ctrl,
            Notify::Type type,
            int dismiss_ms,
            fmt::format_string<Args...> fmt_s, Args&&... args
        ) {
        if (!ctrl) return;
        const auto content = fmt::format(fmt_s, std::forward<Args>(args)...);
        Notify::Notification n(type, dismiss_ms);
        n.setContent("%s", content.c_str());
        InsertNotification(ctrl, std::move(n));
    }

    /// \brief Post a notification using fmt for a runtime format string.
    /// \tparam Args Format argument types.
    /// \param ctrl Controller with a NotificationManager.
    /// \param type Notification type.
    /// \param dismiss_ms Auto-dismiss delay in milliseconds.
    /// \param fmt_rt Runtime format string.
    /// \param args Format arguments.
    template<class... Args>
    inline void NotifyFmtRuntime(
            Controller* ctrl,
            Notify::Type type,
            int dismiss_ms,
            fmt::string_view fmt_rt, Args&&... args
        ) {
        if (!ctrl) return;
        const auto content = fmt::vformat(fmt::runtime(fmt_rt),
                                          fmt::make_format_args(std::forward<Args>(args)...));
        Notify::Notification n(type, dismiss_ms);
        n.setContent("%s", content.c_str());
        InsertNotification(ctrl, std::move(n));
    }

    /// \brief Post a notification with title and content via fmt.
    /// \tparam ArgsT Title argument types.
    /// \tparam ArgsC Content argument types.
    /// \param ctrl Controller with a NotificationManager.
    /// \param type Notification type.
    /// \param dismiss_ms Auto-dismiss delay in milliseconds.
    /// \param title_fmt Title format string.
    /// \param title_args Title format arguments.
    /// \param content_fmt Content format string.
    /// \param content_args Content format arguments.
    template<class... ArgsT, class... ArgsC>
    inline void NotifyWithTitleFmt(
            Controller* ctrl,
            Notify::Type type,
            int dismiss_ms,
            fmt::format_string<ArgsT...> title_fmt,   ArgsT&&... title_args,
            fmt::format_string<ArgsC...> content_fmt, ArgsC&&... content_args
        ) {
        if (!ctrl) return;
        const auto title   = fmt::format(title_fmt,   std::forward<ArgsT>(title_args)...);
        const auto content = fmt::format(content_fmt, std::forward<ArgsC>(content_args)...);
        Notify::Notification n(type, dismiss_ms);
        n.setTitle("%s", title.c_str());
        n.setContent("%s", content.c_str());
        InsertNotification(ctrl, std::move(n));
    }

    /// \brief Post a notification with a button via fmt.
    /// \tparam ArgsT Title argument types.
    /// \tparam ArgsC Content argument types.
    /// \param ctrl Controller with a NotificationManager.
    /// \param type Notification type.
    /// \param dismiss_ms Auto-dismiss delay in milliseconds.
    /// \param button_label Button label.
    /// \param on_click Callback executed on button click.
    /// \param title_fmt Title format string.
    /// \param title_args Title format arguments.
    /// \param content_fmt Content format string.
    /// \param content_args Content format arguments.
    template<class... ArgsT, class... ArgsC>
    inline void NotifyWithButtonFmt(
            Controller* ctrl,
            Notify::Type type,
            int dismiss_ms,
            const char* button_label,
            std::function<void()> on_click,
            fmt::format_string<ArgsT...> title_fmt,   ArgsT&&... title_args,
            fmt::format_string<ArgsC...> content_fmt, ArgsC&&... content_args
        ) {
        if (!ctrl || !button_label || !*button_label) return;
        const auto title   = fmt::format(title_fmt,   std::forward<ArgsT>(title_args)...);
        const auto content = fmt::format(content_fmt, std::forward<ArgsC>(content_args)...);
        Notify::Notification n(type, dismiss_ms);
        n.setTitle("%s", title.c_str());
        n.setContent("%s", content.c_str());
        n.setOnButtonPress(std::move(on_click));
        n.setButtonLabel("%s", button_label);
        InsertNotification(ctrl, std::move(n));
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_NOTIFY_NOTIFICATIONS_HPP_INCLUDED

