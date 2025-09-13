#pragma once
#include <Arduino.h>


struct Logger {

    using WriteFunction = void (*)(const char *, size_t);

    WriteFunction write_func{nullptr};

    static Logger &instance() {
        static Logger instance;
        return instance;
    }

    void log(const char *level, const char *function, const char *format, ...) const {
        if (write_func == nullptr) { return; }

        char buffer[128];
        size_t pos = 0;

        // Форматируем префикс
        int prefix_len = snprintf(buffer, sizeof(buffer),
                                  "[%lu|%s|%s] ",
                                  millis(), level, function);

        if (prefix_len > 0) {
            pos = min(static_cast<size_t>(prefix_len), sizeof(buffer) - 1);
        }

        // Форматируем основное сообщение
        if (pos < sizeof(buffer)) {
            va_list args;
            va_start(args, format);
            int msg_len = vsnprintf(buffer + pos,
                                    sizeof(buffer) - pos,
                                    format,
                                    args);
            va_end(args);

            if (msg_len > 0) {
                pos += min(static_cast<size_t>(msg_len), sizeof(buffer) - pos - 1);
            }
        }

        // Добавляем перевод строки
        if (pos < sizeof(buffer) - 1) {
            buffer[pos] = '\n';
            pos++;
        } else {
            // Если не хватило места - заменяем последний символ
            buffer[sizeof(buffer) - 2] = '\n';
            pos = sizeof(buffer) - 1;
        }

        write_func(buffer, pos);
    }

private:

    Logger() = default;
};

/// Debug
#define Logger_level_debug 0
/// Info
#define Logger_level_info 1
/// Warn
#define Logger_level_warn 2
/// Error
#define Logger_level_error 3
/// Fatal
#define Logger_level_fatal 4
/// None (Disable)
#define Logger_level_none 5


#if not defined(Logger_level)
#define Logger_level Logger_level_debug
#endif

#if Logger_level_debug >= Logger_level
#define Logger_debug(...)   Logger::instance().log("Debug",   __PRETTY_FUNCTION__, __VA_ARGS__)
#else
#define Logger_debug(...)
#endif

#if Logger_level_info >= Logger_level
#define Logger_info(...)   Logger::instance().log("Info",   __PRETTY_FUNCTION__, __VA_ARGS__)
#else
#define Logger_info(...)
#endif

#if Logger_level_warn >= Logger_level
#define Logger_warn(...)   Logger::instance().log("Warn",   __PRETTY_FUNCTION__, __VA_ARGS__)
#else
#define Logger_warn(...)
#endif

#if Logger_level_error >= Logger_level
#define Logger_error(...)   Logger::instance().log("Error",   __PRETTY_FUNCTION__, __VA_ARGS__)
#else
#define Logger_error(...)
#endif

#if Logger_level_fatal >= Logger_level
#define Logger_fatal(...)   Logger::instance().log("Fatal",   __PRETTY_FUNCTION__, __VA_ARGS__)
#else
#define Logger_fatal(...)
#endif

