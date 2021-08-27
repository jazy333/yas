#include "match_set.h"

namespace yas {
size_t MatchSet::size() { return matched_docs_.size(); }

void MatchSet::add(const MatchedDoc& doc) { matched_docs_.push(doc); }

bool MatchSet::next(MatchedDoc& doc) {
  if (matched_docs_.size() == 0) return false;
  doc = matched_docs_.top();
  matched_docs_.pop();
  return true;
}

}  // namespace yas