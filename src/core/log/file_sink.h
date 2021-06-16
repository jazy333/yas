#pragma once
#include <cstdio>

#include "default_formatter.h"
#include "sink.h"

namespace yas {
class FileSink : public Sink {
 public:
  FileSink(FILE* file)
      : Sink(std::unique_ptr<Formatter>(new DefaultFormatter())), file_(file) {}
  FileSink(std::unique_ptr<Formatter> formatter, FILE* file)
      : Sink(std::move(formatter)), file_(file) {}
  virtual ~FileSink() {}
  virtual void log(LogSeverity severity, const char* format, va_list va) override;
   virtual void log(LogSeverity severity, const char* file_name, int line,
                   const char* format, va_list va) override;
  virtual void flush() override;

 private:
  FILE* file_;
};
}  // namespace yas