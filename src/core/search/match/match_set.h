#pragma once

#include <queue>
#include <cstddef>

#include "matched_doc.h"

namespace yas {
class MatchSet {
 public:
  struct MatchedDocComparedWithScore {
    bool operator()(const MatchedDoc& a, const MatchedDoc& b) {
      return a.score_ < b.score_;
    };
  };
  MatchSet() = default;
  virtual ~MatchSet() = default;
  size_t size();
  void add(const MatchedDoc& doc);
  bool next(MatchedDoc& doc);

 private:
  std::priority_queue<MatchedDoc, std::vector<MatchedDoc>,
                      MatchedDocComparedWithScore>
      matched_docs_;
};
}  // namespace yas