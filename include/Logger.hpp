/*
 * Logger.hpp
 * Minimal, generic logging utility. No protocol-specific code here.
 */

#pragma once

#include <chrono>
#include <ctime>
#include <exception>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

namespace logger {

  enum class Level { Debug = 0, Info, Warn, Error, Off };

  class Logger {
  public:
    static Logger &instance() {
      static Logger inst;
      return inst;
    }

    void setLevel(Level l) {
      std::lock_guard<std::mutex> g(mu_);
      level_ = l;
    }

    Level level() const { return level_; }

    void setOutput(std::ostream &os) {
      std::lock_guard<std::mutex> g(mu_);
      out_ = &os;
    }

    void log(Level l, const std::string &msg) {
      if (l < level_ || level_ == Level::Off)
        return;
      auto now = std::chrono::system_clock::now();
      std::time_t t = std::chrono::system_clock::to_time_t(now);

      std::stringstream ss;
      ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
      ss << " [" << levelToString(l) << "] " << msg << '\n';

      std::lock_guard<std::mutex> g(mu_);
      if (out_)
        (*out_) << ss.str();
    }

    void debug(const std::string &m) { log(Level::Debug, m); }
    void info(const std::string &m) { log(Level::Info, m); }
    void warn(const std::string &m) { log(Level::Warn, m); }
    void error(const std::string &m) { log(Level::Error, m); }

    // Log a caught std::exception with optional context message.
    void logException(const std::exception &ex,
                      const std::string &context = "") {
      std::string msg;
      if (!context.empty())
        msg = context + ": ";
      msg += "exception: ";
      msg += ex.what();
      log(Level::Error, msg);
    }

    // Log from an exception_ptr (rethrows to extract std::exception::what()).
    void logExceptionPtr(std::exception_ptr ep,
                         const std::string &context = "") {
      if (!ep) {
        log(Level::Error, context.empty() ? "exception: <null>"
                                          : context + ": exception: <null>");
        return;
      }
      try {
        std::rethrow_exception(ep);
      } catch (const std::exception &e) {
        logException(e, context);
      } catch (...) {
        log(Level::Error, context.empty() ? "exception: <non-std>"
                                          : context + ": exception: <non-std>");
      }
    }

  private:
    Logger() : level_(Level::Info), out_(&std::cerr) {}
    ~Logger() = default;

    static const char *levelToString(Level l) {
      switch (l) {
      case Level::Debug:
        return "DEBUG";
      case Level::Info:
        return "INFO";
      case Level::Warn:
        return "WARN";
      case Level::Error:
        return "ERROR";
      default:
        return "UNKNOWN";
      }
    }

    mutable std::mutex mu_;
    Level level_;
    std::ostream *out_;
  };

  inline Logger &get() { return Logger::instance(); }

} // namespace logger
