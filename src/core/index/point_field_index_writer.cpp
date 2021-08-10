#include "point_field_index_writer.h"
#include "point_field.h"
#include "memory_point_field_index_reader.h"
#include "mmap_file.h"

namespace yas {
PointFieldIndexWriter::PointFieldIndexWriter() {}
PointFieldIndexWriter::~PointFieldIndexWriter() {}
void PointFieldIndexWriter::flush(FieldInfo fi, uint32_t max_doc,Index,const IndexWriterOption& option) {
   BkdTree<T, D> bkd;
   std::string file_kdm = option.dir + "/" + option.segment_prefix +
                        std::to_string(option.current_segment_no)+".kdm";
  File* kdm=new MMapFile;
  kdm->open(file_kdm);
  std::string file_kdi = option.dir + "/" + option.segment_prefix +
                        std::to_string(option.current_segment_no)+".kdi";
  File* kdi=new MMapFile;
   kdm->open(file_kdi);
  std::string file_kdd = option.dir + "/" + option.segment_prefix +
                        std::to_string(option.current_segment_no)+".kdd";
  File* kdd=new MMapFile;
  kdd->open(file_kdd)
  bkd.pack(fi.get_field_id(), &points_, kdm, kdi, kdd);
  kdm->close();delete kdm;
  kdi->close();delete kdi;
  kdd->close();delete kdd;
}

void PointFieldIndexWriter::add(uint32_t docid, Field* field) {
  PointField<T,D>* pf=dynamic_cast<Point<T,D>*>(field);
  auto point = pf->get_value();
  for (int i = 0; i < point.dim(); ++i) {
    skip_list[i].insert(point.get(i), docid);
  }
  point.set_docid(docid);
  points_.write(point);
}

PostingList* PointFieldIndexWriter::get(u_char* min, u_char* max) {
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
}  // namespace yas