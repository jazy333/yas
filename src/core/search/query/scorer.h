#pragma once

namespace yas {
class Scorer {
 public:
  virtual float score() = 0;
};
}  // namespace yas