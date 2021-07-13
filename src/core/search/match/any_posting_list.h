#pragma once
#include "posting_list.h"

namespace yas {
class AnyPostingList : public PostingList {
 public:
  AnyPostingList(uint32_t max_docid);
  ~AnyPostingList();
  uint32_t next();
  uint32_t advance(uint32_t target);
  uint32_t docid();
  long cost();
  std::string name();
  
 private:
  uint32_t docid_;
  uint32_t max_docid_;
};
}  // namespace yas