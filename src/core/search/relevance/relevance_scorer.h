#pragma once
#include "term_stat.h"
#include "index_stat.h"

namespace yas {
class RelevanceScorer {
  virtual float score(long freq,long norm) = 0;
};
}  // namespace yas