#pragma once
#include <algorithm>

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
  PointFieldIndexWriter() = default;
  virtual ~PointFieldIndexWriter() = default;
  void flush(FieldInfo fi, uint32_t max_doc, Index,
             const IndexWriterOption& option) override {
    BkdTree<T, D> bkd;
    std::string file_kdm = option.dir + "/" + option.segment_prefix +
                           std::to_string(option.current_segment_no) + ".kdm";
    File* kdm = new MMapFile;
    kdm->open(file_kdm, true);

    std::string file_kdi = option.dir + "/" + option.segment_prefix +
                           std::to_string(option.current_segment_no) + ".kdi";
    File* kdi = new MMapFile;
    kdi->open(file_kdi, true);

    std::string file_kdd = option.dir + "/" + option.segment_prefix +
                           std::to_string(option.current_segment_no) + ".kdd";
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

  void add(uint32_t docid, std::shared_ptr<Field> field) override {
    PointField<T, D>* pf = dynamic_cast<Point<T, D>*>(field->get());
    auto point = pf->get_value();
    for (int i = 0; i < point.dim(); ++i) {
      skip_list[i].insert(point.get(i), docid);
    }
    point.set_docid(docid);
    points_.write(point);
  }

  PostingList* get(u_char* min, u_char* max) override {
    std::vector<PostingList*> pls;
    Point<T, D> min_point = Point<T, D>(min);
    Point<T, D> max_point = Point<T, D>(max);

    for (int i = 0; i < D; ++i) {
      auto low = skip_lists_[i].lower_bound(min_point.get(i));
      auto high = skip_lists_[i].upper_bound(max_point.get(i));
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
  MemoryPoints<T, D> points_;
  SkipList<T, uint32_t> skip_list[D];
};
}  // namespace yas
