#pragma once

namespace yas {
class Scorer {
 public:
  virtual ~Scorer() = default;
  virtual float score() = 0;
};
}  // namespace yas