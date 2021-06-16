#include "default_formatter.h"

#include <pthread.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>

namespace yas {
std::string DefaultFormatter::format(LogSeverity severity, const char* format,
                                     ...) {
  va_list args;
  va_start(args, format);
  char buffer[kMaxLogLength] = {0};
  char* ptr = buffer;
  std::string seve = SeverityStrings[severity];
  char* header_format = "[%s][%s][%lu][%s][%d]";
  char header[128] = {0};
  std::time_t now = std::time(nullptr);
  struct tm t;
  localtime_r(&now, &t);
  char ts[64];
  size_t size = std::strftime(ts, 64, "%Y-%m-%d %H:%M", &t);
  pthread_t tid = pthread_self();
  size = snprintf(header, 127, header_format, seve.c_str(), ts, tid, __FILE__,
                  __LINE__);
  ptr += size;
  size = vsnprintf(ptr, kMaxLogLength - size - 1, format, args);
  ptr += size;
  *ptr = '\n';
  va_end(args);
  return buffer;
}
}  // namespace