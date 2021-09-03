#pragma once
#include "posting_list.h"

namespace yas {
class TermReader : public PostingList {
 public:
  virtual ~TermReader() = default;
  virtual int freq() = 0;
  virtual int next_postion() = 0;
  virtual int doc_freq() = 0;
  virtual void set_scorer(Scorer* scorer) = 0;
};
}  // namespace yas