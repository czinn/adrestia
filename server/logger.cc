#include "logger.h"

#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstdarg>

using namespace std;

const std::string RESET = "\033[0m";
const std::string RED = "\033[1;31m";
const std::string YELLOW = "\033[1;33m";
const std::string BLACK = "\033[1;30m";
const std::string WHITE = "\033[1;37m";
const std::string CYAN = "\033[1;36m";

std::ofstream null_ostream;

void Logger::log_base(const char *format, va_list args) const {
  printf("[%s] ", prefix.c_str());
  vprintf(format, args);
  printf("\n");
}

void Logger::trace(const char *format, ...) const {
  if (min_level > TRACE) return;
  if (color) printf("%s", BLACK.c_str());
  va_list args;
  va_start(args, format);
  log_base(format, args);
  va_end(args);
}

void Logger::debug(const char *format, ...) const {
  if (min_level > DEBUG) return;
  if (color) printf("%s", RESET.c_str());
  va_list args;
  va_start(args, format);
  log_base(format, args);
  va_end(args);
}

void Logger::info(const char *format, ...) const {
  if (min_level > INFO) return;
  if (color) printf("%s", CYAN.c_str());
  va_list args;
  va_start(args, format);
  log_base(format, args);
  va_end(args);
}

void Logger::warn(const char *format, ...) const {
  if (min_level > WARN) return;
  if (color) printf("%s", YELLOW.c_str());
  va_list args;
  va_start(args, format);
  log_base(format, args);
  va_end(args);
}

void Logger::error(const char *format, ...) const {
  if (min_level > ERROR) return;
  if (color) printf("%s", RED.c_str());
  va_list args;
  va_start(args, format);
  log_base(format, args);
  va_end(args);
}

std::ostream &Logger::log_base_stream(std::ostream &os) const {
  return os << "[" << prefix << "] ";
}

std::ostream &Logger::trace_() const {
  if (min_level > TRACE) return null_ostream;
  return log_base_stream(cout << (color ? BLACK : ""));
}

std::ostream &Logger::debug_() const {
  if (min_level > DEBUG) return null_ostream;
  return log_base_stream(cout << (color ? RESET : ""));
}

std::ostream &Logger::info_() const {
  if (min_level > INFO) return null_ostream;
  return log_base_stream(cout << (color ? CYAN : ""));
}

std::ostream &Logger::warn_() const {
  if (min_level > WARN) return null_ostream;
  return log_base_stream(cout << (color ? YELLOW : ""));
}

std::ostream &Logger::error_() const {
  if (min_level > ERROR) return null_ostream;
  return log_base_stream(cout << (color ? RED : ""));
}
