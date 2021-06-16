#include "file_sink.h"

#include <cstdarg>
#include <cstdio>

namespace yas {
void FileSink::log(LogSeverity severity, const char* format, va_list va) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!should_log(severity)) return;
  std::string msg = formatter_->format(severity, format, va);
  fwrite(msg.data(), msg.size(), sizeof(char), file_);
}

void FileSink::log(LogSeverity severity, const char* file_name, int line,
                   const char* format, va_list va) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!should_log(severity)) return;
  std::string msg = formatter_->format(severity, file_name, line, format, va);
  fwrite(msg.data(), msg.size(), sizeof(char), file_);
}

void FileSink::flush() { ::fflush(file_); }

}  // namespace yas