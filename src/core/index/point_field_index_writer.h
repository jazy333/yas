#pragma once
#include "bkd_tree.h"
#include "file.h"
#include "memory_points.h"
#include "posting_list.h"
#include "skip_list.h"

namespace yas {
template <class T, class D>
class PointFieldIndexWriter {
 public:
  PointFieldIndexWriter(File* kdm, File* kdi, File* kdd);
  ~PointFieldIndexWriter();
  void flush(FieldInfo fi, uint32_t max_doc);
  void add(uint32_t docid, Field* field);
  FieldIndexReader* get_reader();

 private:
  MemoryPoints<T, D> points_;
  SkipList<T,uint32_t> skip_list[D];
  BkdTree<T, D> bkd_;
  File* kdm_;
  File* kdi_;
  File* kdd_;
};
}  // namespace yas
