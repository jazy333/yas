#pragma once
#include "bkd_tree.h"
#include "field_index_meta.h"
#include "sequence_posting_list.h"

namespace yas {
template <class T, class D>
class PointFieldIndexReader {
 public:
  PointFieldIndexReader(int field_id, PointFieldMeta* meta, File* kdi,
                        File* kdd)
      : field_id_(field_id), meta_(meta), kdi_(kdi), kdd_(kdd) {}

  ~PointFieldIndexReader() {}

  PostingList* get(Point<T, D> min, Point<T, D> max) {
    std::vector<uint32_t> docids;
    bkd_.intersect(meta_, min, max, docids);
    sort(docids.begin(), docids.end());
    return new SequencePostingList(std::move(docids));
  }

 private:
  BkdTree<T, D> bkd_;
  int field_id_;
  PointFieldMeta* meta_;
  File* kdi_;
  File* kdd_;
};
}  // namespace yas