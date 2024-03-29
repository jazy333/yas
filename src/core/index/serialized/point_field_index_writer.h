#pragma once
#include <algorithm>

#include "and_posting_list.h"
#include "bkd_tree.h"
#include "field_index_reader.h"
#include "field_index_writer.h"
#include "field_info.h"
#include "file.h"
#include "memory_points.h"
#include "none_posting_list.h"
#include "point_field.h"
#include "posting_list.h"
#include "sequence_posting_list.h"
#include "skip_list.h"

namespace yas {
template <class T, int D>
class PointField;

template <class T, int D>
class PointFieldIndexWriter : public FieldIndexWriter,
                              public PointFieldIndexReader {
 public:
  PointFieldIndexWriter() = default;
  virtual ~PointFieldIndexWriter() = default;

  void flush(FieldInfo fi, uint32_t max_doc,
             const IndexOption& option) override {
    BkdTree<T, D> bkd;
    std::string file_kdm = option.get_point_fields_meta_file();
    File* kdm = new MMapFile;
    kdm->open(file_kdm, true);

    std::string file_kdi = option.get_point_fields_index_file();
    File* kdi = new MMapFile;
    kdi->open(file_kdi, true);

    std::string file_kdd = option.get_point_fields_data_file();
    File* kdd = new MMapFile;
    kdd->open(file_kdd, true);

    bkd.pack(fi.get_field_id(), &points_, kdm, kdi, kdd);
    kdm->close();
    delete kdm;
    kdi->close();
    delete kdi;
    kdd->close();
    delete kdd;
  }

  int add(uint32_t docid, std::shared_ptr<Field> field) override {
    PointField<T, D>* pf = dynamic_cast<PointField<T, D>*>(field.get());
    auto point = pf->get_value();
    for (int i = 0; i < D; ++i) {
      skip_lists_[i].insert(point.get(i), docid);
    }
    point.set_docid(docid);
    points_.write(point);
    return 0;
  }

  std::shared_ptr<PostingList> get(u_char* min, u_char* max) override {
    std::vector<std::shared_ptr<PostingList>> pls;
    Point<T, D> min_point = Point<T, D>(min);
    Point<T, D> max_point = Point<T, D>(max);

    for (int i = 0; i < D; ++i) {
      auto low = skip_lists_[i].lower_bound(min_point.get(i));
      auto high = skip_lists_[i].upper_bound(max_point.get(i));
      std::vector<uint32_t> docids;
      while (low != high) {
        docids.push_back(low->value);
        low = low->next();
      }
      sort(docids.begin(), docids.end());
      pls.push_back(std::shared_ptr<PostingList>(new SequencePostingList(std::move(docids))));
    }
    if (pls.size() == 0) {
      return std::shared_ptr<PostingList>(new NonePostingList());
    } else if (pls.size() == 1) {
      return pls[0];
    } else
      return std::shared_ptr<PostingList>(new AndPostingList(pls));
  }

  void init(int field_id, PointFieldMeta meta, std::shared_ptr<File> kdi,
            std::shared_ptr<File> kdd) {}

 private:
  MemoryPoints<T, D> points_;
  SkipList<T, uint32_t> skip_lists_[D];
};
}  // namespace yas
