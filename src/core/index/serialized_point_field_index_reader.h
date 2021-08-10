#pragma once
#include "bkd_tree.h"
#include "field_index_meta.h"
#include "field_index_reader.h"
#include "sequence_posting_list.h"

namespace yas {

template<class T,class D>
class SerializedPointFieldIndexReader : public PointFieldIndexReader {
 public:
  SerializedPointFieldIndexReader(int field_id, PointFieldMeta* meta, File* kdi,
                                  File* kdd);
  virtual ~SerializedPointFieldIndexReader();

  PostingList* get(u_char* min, u_char* max, int dim);

 private:
  int field_id_;
  PointFieldMeta* meta_;
  File* kdi_;
  File* kdd_;

}  // namespace yas