#pragma once
#include "bkd_tree.h"
#include "field_index_reader.h"
#include "file.h"
#include "memory_points.h"
#include "posting_list.h"
#include "skip_list.h"

namespace yas {
template <class T, class D>
class PointFieldIndexWriter : public FieldIndexWriter,
                              public PointFieldIndexReader {
 public:
  PointFieldIndexWriter();
  virtual ~PointFieldIndexWriter();
  void flush(FieldInfo fi, uint32_t max_doc, Index,
             const IndexWriterOption& option) override;
  void add(uint32_t docid, std::shared_ptr<Field> field) override;
  PostingList* get(u_char* min, u_char* max) override;

 private:
  MemoryPoints<T, D> points_;
  SkipList<T, uint32_t> skip_list[D];
};
}  // namespace yas
