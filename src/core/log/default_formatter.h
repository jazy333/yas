#pragma once
#include "formatter.h"
#include "severity.h"

namespace yas {
class DefaultFormatter : public Formatter {
 public:
  DefaultFormatter() = default;
  virtual ~DefaultFormatter() = default;
  virtual std::string format(LogSeverity severity, const char* format,
                             ...) override;
};
}  // namespace yas