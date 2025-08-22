#pragma once
#ifndef _IMGUIX_EVENTS_LOG_EVENT_HPP_INCLUDED
#define _IMGUIX_EVENTS_LOG_EVENT_HPP_INCLUDED

/// \file LogeEvent.hpp
/// \brief Tiny header-only logger with file/line/function metadata.

#if defined(__GNUC__)
    #define IMGUIX_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define IMGUIX_FUNCTION __FUNCSIG__
#else
    #define IMGUIX_FUNCTION __func__
#endif

/// \brief Helper macro to emit a LogEvent.
#define IMGUIX_LOG_EVENT(LVL, MSG) \
    ImGuiX::Events::LogEvent((LVL), (MSG), __FILE__, __LINE__, IMGUIX_FUNCTION)

namespace ImGuiX::Events {

    /// \enum LogLevel
    /// \brief Logging severity levels.
    enum class LogLevel {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };

    /// \brief Convert level to short string.
    inline const char* level_to_cstr(LogLevel lv) noexcept {
        switch (lv) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info:  return "INFO";
            case LogLevel::Warn:  return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Fatal: return "FATAL";
        }
        return "UNKNOWN";
    }

    /// \brief Event containing log message and source metadata.
    class LogEvent : public Pubsub::Event {
    public:
        LogLevel level;       ///< Severity level
        std::string message;  ///< Log text
        const char* file;     ///< Source file
        int line;             ///< Source line
        const char* function; ///< Function signature

        LogEvent(
                LogLevel lvl,
                std::string msg,
                const char* file,
                int line,
                const char* func
            )
            : level(lvl), message(std::move(msg)),
              file(file), line(line), function(func) {}

        std::type_index type() const override {
            return typeid(LogEvent);
        }

        const char* name() const override {
            return "LogEvent";
        }
    };

} // namespace ImGuiX::Events

#endif // _IMGUIX_EVENTS_LOG_EVENT_HPP_INCLUDED
