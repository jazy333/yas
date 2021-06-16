#include "file_sink.h"

#include <cstdio>
#include <cstdarg>

namespace yas {
  void FileSink::log(LogSeverity severity, const char* format, ...) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (!should_log(severity)) return;
  va_list va;
  va_start( va,format);
  std::string msg = formatter_->format(severity,format, va);
  fwrite(msg.data(), msg.size(), sizeof(char), file_);
  va_end(va);
}

void FileSink::flush() { ::fflush(file_); }

}  // namespace yas