#pragma once
#include "index_option.h"
#include "index_reader.h"
#include "index_writer.h"

namespace yas {
class IndexMerger {
 public:
  struct DocMap {
    int seg_no;
    uint32_t ori_docid;
    uint32_t new_docid;
  };

  struct DocidWithPositions {
    uint32_t docid;
    std::vector<uint32_t> positions;
  };

  struct DocidWithPositionsCompare {
    bool operator()(const DocidWithPositions& a, const DocidWithPositions& b) {
      return a.docid > b.docid;
    }
  };

  IndexMerger(IndexOption& index_option,IndexOption& merge_option);
  virtual ~IndexMerger();
  int merge();

 private:
  int merge_numeric(
      std::vector<std::shared_ptr<FieldValueIndexReader>>& readers,
      std::vector<DocMap>& doc_maps, FieldInfo& field_info, uint32_t max_doc);
  int merge_binary(std::vector<std::shared_ptr<FieldValueIndexReader>>& readers,
                   std::vector<DocMap>& doc_maps, FieldInfo& field_info,
                   uint32_t max_doc);
  int merge_invert(
      std::vector<std::shared_ptr<InvertFieldsIndexReader>>& readers,
      std::vector<std::unordered_map<uint32_t, uint32_t>>& doc_maps,
      FieldInfo& field_info, uint32_t max_doc,uint32_t& term_freq);
  template <class T, int D>
  int merge_point(
      FieldInfo info, std::vector<PointFieldMeta*> metas,
      std::vector<File*> kdis, std::vector<File*> kdds,
      std::vector<std::unordered_map<uint32_t, uint32_t>>& doc_maps,uint32_t max_doc);
  uint32_t build_doc_maps(
      std::vector<std::shared_ptr<FieldValueIndexReader>>& id_readers,
      const std::vector<uint32_t>& max_docids,
      std::vector<std::unordered_map<uint32_t, uint32_t>>& old_to_news,
      std::vector<DocMap>& new_to_old);
  int write_segment_info(uint32_t max_docid);

 private:
  IndexOption index_option_;
  IndexOption merge_option_;
};
}  // namespace yas