#pragma once

#include "default_formatter.h"
#include "sink.h"

#include <cstdio>

namespace yas {
class FileSink : public Sink {
 public:
  FileSink(FILE* file)
      : Sink(std::unique_ptr<Formatter>(new DefaultFormatter())), file_(file) {}
  FileSink(std::unique_ptr<Formatter> formatter, FILE* file)
      : Sink(std::move(formatter)), file_(file) {}
  FileSink(const FileSink&) = delete;
  FileSink(FileSink&&) = delete;
  FileSink& operator=(const FileSink&) = delete;
  FileSink& operator=(FileSink&&) = delete;
  virtual ~FileSink() {}
  virtual void log(LogSeverity severity, const char* format,
                   va_list va) override;
  virtual void log(LogSeverity severity, const char* file_name, int line,
                   const char* format, va_list va) override;
  virtual void flush() override;

 private:
  FILE* file_;
};
}  // namespace yas