#pragma once
#ifndef _IMGUIX_EVENTS_LOG_EVENT_HPP_INCLUDED
#define _IMGUIX_EVENTS_LOG_EVENT_HPP_INCLUDED

/// \file LogEvent.hpp
/// \brief Tiny header-only logger with file/line/function metadata.

#if defined(__GNUC__)
#   define IMGUIX_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#   define IMGUIX_FUNCTION __FUNCSIG__
#else
#   define IMGUIX_FUNCTION __func__
#endif

/// \brief Emit a LogEvent with source metadata.
/// \param LVL Log level.
/// \param MSG Log message.
#define IMGUIX_LOG_EVENT(LVL, MSG)                                             \
    ImGuiX::Events::LogEvent((LVL), (MSG), __FILE__, __LINE__, IMGUIX_FUNCTION)

namespace ImGuiX::Events {

    /// \enum LogLevel
    /// \brief Logging severity levels.
    enum class LogLevel { Trace, Debug, Info, Warn, Error, Fatal };

    /// \brief Convert level to short string.
    /// \param level Severity level.
    /// \return Null-terminated level string.
    inline const char *levelToCStr(LogLevel level) noexcept {
        switch (level) {
        case LogLevel::Trace:
            return u8"TRACE";
        case LogLevel::Debug:
            return u8"DEBUG";
        case LogLevel::Info:
            return u8"INFO";
        case LogLevel::Warn:
            return u8"WARN";
        case LogLevel::Error:
            return u8"ERROR";
        case LogLevel::Fatal:
            return u8"FATAL";
        }
        return u8"UNKNOWN";
    }

    /// \brief Event containing log message and source metadata.
    class LogEvent : public Pubsub::Event {
    public:
        LogLevel level;       ///< Severity level.
        std::string message;  ///< Log text.
        const char *file;     ///< Source file.
        int line;             ///< Source line.
        const char *function; ///< Function signature.

        /// \brief Construct log event.
        /// \param lvl Severity level.
        /// \param msg Log text.
        /// \param file Source file.
        /// \param line Source line.
        /// \param func Function signature.
        LogEvent(LogLevel lvl, std::string msg, const char *file, int line,
               const char *func)
            : level(lvl), message(std::move(msg)), file(file), line(line),
              function(func) {}

        /// \copydoc Pubsub::Event::type
        std::type_index type() const override { return typeid(LogEvent); }

        /// \copydoc Pubsub::Event::name
        const char *name() const override { return u8"LogEvent"; }

        std::unique_ptr<Event> clone() const override {
            return std::make_unique<LogEvent>(
                level,
                message,
                file,
                line,
                function
            );
        }
    };

} // namespace ImGuiX::Events

#endif // _IMGUIX_EVENTS_LOG_EVENT_HPP_INCLUDED
