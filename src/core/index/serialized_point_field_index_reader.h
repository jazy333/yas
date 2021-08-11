#pragma once
#include "bkd_tree.h"
#include "field_index_meta.h"
#include "field_index_reader.h"
#include "sequence_posting_list.h"

namespace yas {

template <class T, class D>
class SerializedPointFieldIndexReader : public PointFieldIndexReader {
 public:
  SerializedPointFieldIndexReader(int field_id, PointFieldMeta* meta, std::shared_ptr<File> kdi,
                                  std::shared_ptr<File> kdd)
      : field_id_(field_id), meta_(meta), kdi_(kdi), kdd_(kdd) {}
  SerializedPointFieldIndexReader()
      : field_id_(-1), meta_(nullptr), kdi_(nullptr), kdd_(nullptr) {}
  virtual ~SerializedPointFieldIndexReader() = default;

  PostingList* get(u_char* min, u_char* max) override {
    std::vector<uint32_t> docids;
    BkdTree<T, D> bkd(kdi_, kdd_);
    bkd.intersect(meta_, min, max, docids);
    sort(docids.begin(), docids.end());
    return new SequencePostingList(std::move(docids));
  }
  void init(int field_id, PointFieldMeta meta, std::shared_ptr<File> kdi,
            std::shared_ptr<File> kdd) override {
    field_id_ = field_id;
    meta_ = meta;
    kdi_ = kdi;
    kdd_ = kdd;
  }

 private:
  int field_id_;
  PointFieldMeta meta_;
  std::shared_ptr<File> kdi_;
  std::shared_ptr<File> kdd_;
};

}  // namespace yas