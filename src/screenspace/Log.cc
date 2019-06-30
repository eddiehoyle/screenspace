#include "Log.hh"

#include <iostream>

namespace technic {

Log::Log(std::ostream* stream, Level level)
    : m_stream(stream),
      m_level(level),
      m_mutex() {
}

Stream::Stream(Log& log, const std::string& prefix)
    : m_log(log) {
  m_log.m_mutex.lock();
  *( m_log.m_stream ) << "[" << prefix << "]: ";
}

Stream::~Stream() {
  m_log.m_mutex.unlock();
  *( m_log.m_stream ) << std::endl;
}

NullStream::NullStream(Log& log, const std::string& prefix)
    : m_log(log) {
}

Log& debug_log() {
  static Log log(&std::cout, Level::Debug);
  return log;
}

Log& info_log() {
  static Log log(&std::cout, Level::Info);
  return log;
}

Log& warn_log() {
  static Log log(&std::cerr, Level::Warning);
  return log;
}

Log& error_log() {
  static Log log(&std::cerr, Level::Error);
  return log;
};

} // namspace technic