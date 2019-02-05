#pragma once

#include <iostream>
#include <string>
#include <cstdarg>

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
    Level min_level = TRACE;
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
