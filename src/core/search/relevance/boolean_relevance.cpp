#include "boolean_relevance.h"
#include "boolean_scorer.h"

namespace yas {
RelevanceScorer* BooleanRelevance::scorer(float boost, IndexStat* index_stat,
                                          TermStat* term_stat) {
  return new BooleanScorer(boost);
}
}  // namespace yas