#pragma once
#ifndef _IMGUIX_CORE_NOTIFY_NOTIFICATION_MANAGER_HPP_INCLUDED
#define _IMGUIX_CORE_NOTIFY_NOTIFICATION_MANAGER_HPP_INCLUDED

/// \file NotificationManager.hpp
/// \brief Toast notification system for ImGuiX windows.
/// Based on imgui-notify by patrickcjk (https://github.com/patrickcjk/imgui-notify)
/// \author Original implementation by TyomaVader
/// \author Modified by NewYaroslav + refactor by Nick

#include <fmt/format.h>

#include "Notification.hpp"

namespace ImGuiX::Notify {

    /// \brief Manager that queues and renders notifications.
    class NotificationManager {
    public:
        NotificationManager() = default;

        /// \brief Access mutable config.
        Config&       config()       { return m_cfg; }
        /// \brief Access const config.
        const Config& config() const { return m_cfg; }

        /// \brief Fonts configuration.
        FontConfig&       fonts()       { return m_fonts; }
        const FontConfig& fonts() const { return m_fonts; }
        
        IconConfig&       icons()       { return m_icons; }
        const IconConfig& icons() const { return m_icons; }

        void push(Notification toast) { m_notifications.push_back(std::move(toast)); }
        void clear() { m_notifications.clear(); }

        /// \brief Render all active notifications.
        void render();

        /// \brief Post notification using fmt with compile-time checked format (string literal).
        template<class... Args>
        void postFmt(Notify::Type type, int dismiss_ms,
                     fmt::format_string<Args...> fmt_s, Args&&... args) {
            Notify::Notification n(type, dismiss_ms);
            const auto text = fmt::format(fmt_s, std::forward<Args>(args)...);
            n.setContent("%s", text.c_str());
            push(std::move(n));
        }

        /// \brief Post notification using fmt for a runtime format string.
        template<class... Args>
        void postFmtRuntime(Notify::Type type, int dismiss_ms,
                            fmt::string_view fmt_rt, Args&&... args) {
            Notify::Notification n(type, dismiss_ms);
            const auto text = fmt::vformat(fmt::runtime(fmt_rt),
                                           fmt::make_format_args(std::forward<Args>(args)...));
            n.setContent("%s", text.c_str());
            push(std::move(n));
        }

        /// \brief Post notification with title/content via fmt (both literals).
        template<class... ArgsT, class... ArgsC>
        void postWithTitleFmt(Notify::Type type, int dismiss_ms,
                              fmt::format_string<ArgsT...> title_fmt,   ArgsT&&... title_args,
                              fmt::format_string<ArgsC...> content_fmt, ArgsC&&... content_args) {
            Notify::Notification n(type, dismiss_ms);
            const auto t = fmt::format(title_fmt,   std::forward<ArgsT>(title_args)...);
            const auto c = fmt::format(content_fmt, std::forward<ArgsC>(content_args)...);
            n.setTitle("%s", t.c_str());
            n.setContent("%s", c.c_str());
            push(std::move(n));
        }

        /// \brief Post notification with a button via fmt (all literals).
        template<class... ArgsT, class... ArgsC>
        void postWithButtonFmt(Notify::Type type, int dismiss_ms,
                               const char* button_label, std::function<void()> on_click,
                               fmt::format_string<ArgsT...> title_fmt,   ArgsT&&... title_args,
                               fmt::format_string<ArgsC...> content_fmt, ArgsC&&... content_args) {
            Notify::Notification n(type, dismiss_ms);
            const auto t = fmt::format(title_fmt,   std::forward<ArgsT>(title_args)...);
            const auto c = fmt::format(content_fmt, std::forward<ArgsC>(content_args)...);
            n.setTitle("%s", t.c_str());
            n.setContent("%s", c.c_str());
            n.setOnButtonPress(std::move(on_click));
            n.setButtonLabel("%s", button_label);
            push(std::move(n));
        }

        /// \brief Post notification using classic printf-style format (always available).
        inline void postf(Notify::Type type, int dismiss_ms, const char* fmt_s, ...) {
            if (!fmt_s) return;
            va_list args; va_start(args, fmt_s);
            std::string text = ImGuiX::Utils::vformat_va(fmt_s, args);
            va_end(args);
            Notify::Notification n(type, dismiss_ms);
            n.setContent("%s", text.c_str());
            push(std::move(n));
        }

    private:
        void remove(std::size_t index) {
            m_notifications.erase(m_notifications.begin() + static_cast<std::ptrdiff_t>(index));
        }
		
		/// \brief Посчитать эффективный dismiss с учётом политики и wrap.
		int computeEffectiveDismissMs(
			const Notification& n,
			const Config& cfg,
			float wrap_width,
			int wrapped_lines /*>=1*/) const;

        std::vector<Notification> m_notifications;
        Config     m_cfg{};
        IconConfig m_icons{};
        FontConfig m_fonts{};
    };

} // namespace ImGuiX::Notify

#ifdef IMGUIX_HEADER_ONLY
#   include "NotificationManager.ipp"
#endif

#endif // _IMGUIX_CORE_NOTIFY_NOTIFICATION_MANAGER_HPP_INCLUDED
