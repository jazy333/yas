#include "default_formatter.h"

#include <pthread.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>

namespace yas {
std::string DefaultFormatter::do_format(LogSeverity severity,
                                        const char* file_name, int line,
                                        const char* format, va_list args) {
  char buffer[kMaxLogLength] = {0};
  char* ptr = buffer;
  std::string seve = SeverityStrings[severity];
  const char* header_format = 0;
  if (!file_name)
    header_format = "[%s][%s][%5u]";
  else
    header_format = "[%s][%s][%5u][%s:%d]";
  char header[256] = {0};
  std::time_t now = std::time(nullptr);
  struct tm t;
  localtime_r(&now, &t);
  char ts[64];
  size_t size = std::strftime(ts, 64, "%Y-%m-%d %H:%M", &t);
  pthread_t tid = pthread_self();
  if (!file_name)
    size = snprintf(header, 255, header_format, seve.c_str(), ts, tid);
  else
    size = snprintf(header, 255, header_format, seve.c_str(), ts, tid,
                    file_name, line);
  memcpy(ptr, header, size);
  ptr += size;
  size = vsnprintf(ptr, kMaxLogLength - size - 1, format, args);
  ptr += size;
  *ptr = '\n';

  return buffer;
}
std::string DefaultFormatter::format(LogSeverity severity, const char* format,
                                     va_list args) {
  return do_format(severity, nullptr, -1, format, args);
}

std::string DefaultFormatter::format(LogSeverity severity,
                                     const char* file_name, int line,
                                     const char* format, va_list args) {
  return do_format(severity, file_name, line, format, args);
}

}  // namespace yas