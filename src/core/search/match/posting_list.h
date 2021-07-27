#pragma once
#include "scorer.h"

#include <cstdint>
#include <limits>
#include <string>


namespace yas {
class PostingList {
 public:
  virtual uint32_t next() = 0;
  virtual uint32_t advance(uint32_t target) = 0;
  virtual uint32_t docid() = 0;
  virtual long cost() = 0;
  virtual std::string name() { return "PostingList"; }
  
  static const uint32_t NDOCID=std::numeric_limits<uint32_t>::max();

 protected:
  bool posting_list_compare_with_docid(PostingList* l, PostingList* r) {
    return l->docid() < r->docid();
  }

  bool posting_list_compare_with_cost(PostingList* l, PostingList* r) {
    return l->cost() < r->cost();
  }
};
}  // namespace yas