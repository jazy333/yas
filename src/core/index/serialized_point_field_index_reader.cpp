#include "serialized_point_field_index_reader.h"

namespace yas {
SerializedPointFieldIndexReader::SerializedPointFieldIndexReader(
    int field_id, PointFieldMeta* meta, File* kdi, File* kdd)
    : field_id_(field_id), meta_(meta), kdi_(kdi), kdd_(kdd) {}

SerializedPointFieldIndexReader::~SerializedPointFieldIndexReader() {}

PostingList* SerializedPointFieldIndexReader::get(u_char* min, u_char* max) {
  std::vector<uint32_t> docids;
  BkdTree<T, D> bkd(kdi_, kdd_);
  bkd.intersect(meta_, min, max, docids);
  sort(docids.begin(), docids.end());
  return new SequencePostingList(std::move(docids));
}

}  // namespace yas