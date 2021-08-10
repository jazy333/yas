#pragma once
#include <algorithm>
#include <utility>

#include "and_posting_list.h"
#include "field_index_reader.h"
#include "posting_list.h"
#include "sequence_posting_list.h"
#include "skip_list.h"

namespace yas {
template <class T, class D>
class MemoryPointFieldIndexReader : public PointFieldIndexReader {
 public:
  MemoryPointFieldIndexReader(SkipList<T, uint32_t>* skip_lists)
      : skip_lists_(skip_lists) {}
  virtual ~MemoryPointFieldIndexReader() {}
  PostingList* get(u_char* min, u_char* max) {
    std::vector<PostingList*> pls;
    for (int i = 0; i < D; ++i) {
      auto low = skip_lists_[i].lower_bound(min.get(i));
      auto high = skip_lists_[i].upper_bound(max.get(i));
      std::vector<uint32_t> docids;
      while (low != high) {
        docids.push_back(low->value);
      }
      sort(docids.begin(), docids.end());
      pls.push_back(new SequencePostingList(std::move(docids)));
    }
    if (pls.size() == 0) {
      return new NonePostingList();
    } else if (pls.size() == 1) {
      return pls[0];
    } else
      return new AndPostingList(std::move(pls));
  }

 private:
  SkipList<T, uint32_t>* skip_lists_;
};
}  // namespace yas