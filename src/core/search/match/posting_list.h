#pragma once
#include <cstdint>
#include <limits>
#include <memory>
#include <string>

#include "scorer.h"

namespace yas {
class PostingList : public Scorer {
 public:
  virtual ~PostingList() = default;
  virtual uint32_t next() = 0;
  virtual uint32_t advance(uint32_t target) = 0;
  virtual uint32_t docid() = 0;
  virtual long cost() = 0;
  virtual std::string name() { return "PostingList"; }

 protected:
  struct posting_list_compare_with_docid {
    bool operator()(std::shared_ptr<PostingList> l,
                    std::shared_ptr<PostingList> r) {
      return l->docid() > r->docid();
    }
  };
  struct posting_list_compare_with_cost {
    bool operator()(std::shared_ptr<PostingList> l,
                    std::shared_ptr<PostingList> r) {
      return l->cost() > r->cost();
    }
  };
};
// invalid docid
static const uint32_t NDOCID = std::numeric_limits<uint32_t>::max();
}  // namespace yas
