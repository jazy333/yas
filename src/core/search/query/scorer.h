#pragma once
#include "term_stat.h"
#include "index_stat.h"

namespace yas {
class Scorer {
  virtual float score() = 0;
};
}  // namespace yas