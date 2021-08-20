#include "match_set.h"

namespace yas {
size_t MatchSet::size() { return matched_docs_.size(); }
void MatchSet::add(const MatchedDoc& doc) { matched_docs_.push(doc); }

}  // namespace yas