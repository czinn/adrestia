#pragma once

#include <iostream>
#include <string>
#include <cstdarg>

/* Adrestia's logger. Each thread has their own instance, which lets each
 * thread define its own [prefix] (printed before every log message).
 *
 * TRACE (grey): Feel free to print anything reasonable. We currently print
 * some function entry/exits, raw data received from users, and SQL queries.
 * DEBUG (white): Print things that could conceivably be used to debug stuff
 * when things go wrong.
 * INFO (blue): Print things that we would be interested in seeing. Examples:
 * New users, users coming online, new matches being played/finished, users
 * changing their name
 * WARN (yellow): Print strange conditions that may have resulted from bad
 * input data.
 * ERROR (red): Print failures that should absolutely not happen and that
 * demand further attention.
 */

class Logger {
  public:
    enum Level {
      TRACE = 0,
      DEBUG = 1,
      INFO = 2,
      WARN = 3,
      ERROR = 4,
    };

    std::string prefix;
    Level min_level = INFO;
    bool color = true;

    void trace(const char *format, ...) const;
    void debug(const char *format, ...) const;
    void info(const char *format, ...) const;
    void warn(const char *format, ...) const;
    void error(const char *format, ...) const;

    std::ostream &trace_() const;
    std::ostream &debug_() const;
    std::ostream &info_() const;
    std::ostream &warn_() const;
    std::ostream &error_() const;

  private:
    void log_base(const char *format, va_list args) const;
    std::ostream &log_base_stream(std::ostream &os) const;
};

extern thread_local Logger logger;
