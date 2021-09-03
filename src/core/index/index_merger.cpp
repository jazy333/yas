#include "index_merger.h"

#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

#include "binary_field_index_writer.h"
#include "block_term_reader.h"
#include "db.h"
#include "field_index_reader.h"
#include "invert_fields_index_writer.h"
#include "mmap_file.h"
#include "numeric_field.h"
#include "numeric_field_index_writer.h"
#include "point_field.h"
#include "point_field_index_writer.h"
#include "text_field.h"

namespace yas {
IndexMerger::IndexMerger(IndexOption& index_option)
    : index_option_(index_option) {}

IndexMerger::~IndexMerger() {}

int IndexMerger::merge_numeric(
    std::vector<std::shared_ptr<FieldValueIndexReader>>& readers,
    std::vector<DocMap>& doc_maps, FieldInfo& field_info, uint32_t max_doc) {
  NumericFieldIndexWriter nfiw;
  for (auto&& doc_map : doc_maps) {
    auto field_value_index_reader = readers[doc_map.seg_no];
    uint64_t value = 0;
    field_value_index_reader->get(doc_map.ori_docid, value);
    nfiw.add(doc_map.new_docid, std::make_shared<NumericField>(
                                    field_info.get_field_name(), value));
  }

  nfiw.flush(field_info, max_doc, index_option_);
  return 0;
}

int IndexMerger::merge_binary(
    std::vector<std::shared_ptr<FieldValueIndexReader>>& readers,
    std::vector<DocMap>& doc_maps, FieldInfo& field_info, uint32_t max_doc) {
  BinaryFieldIndexWriter bfiw;
  for (auto&& doc_map : doc_maps) {
    auto field_value_index_reader = readers[doc_map.seg_no];
    std::vector<uint8_t> value;
    field_value_index_reader->get(doc_map.ori_docid, value);
    std::string svalue(value.begin(), value.end());
    bfiw.add(doc_map.new_docid,
             std::make_shared<TextField>(field_info.get_field_name(), svalue));
  }

  bfiw.flush(field_info, max_doc, index_option_);
  return 0;
}

int IndexMerger::merge_invert(
    std::vector<std::shared_ptr<InvertFieldsIndexReader>>& readers,
    std::vector<std::unordered_map<uint32_t, uint32_t>>& doc_maps,
    FieldInfo& field_info, uint32_t max_doc) {
  auto out_db = std::shared_ptr<HashDB>(new HashDB);
  std::string db_path = index_option_.get_invert_file();
  out_db->open(db_path);
  for (int i = 0; i < readers.size(); ++i) {
    DB* db = readers[i]->get_db();
    auto iter = db->make_iterator();
    iter->first();
    do {
      std::vector<std::shared_ptr<BlockTermReader>> term_readers(readers.size(),
                                                                 nullptr);
      std::string key, value;
      iter->get(key, value);
      if (out_db->test(key) == 0) {
        continue;
      }
      term_readers[i] =
          std::shared_ptr<BlockTermReader>(new BlockTermReader(value));
      for (int j = i + 1; j < readers.size(); ++j) {
        DB* db = readers[j]->get_db();
        if (db->test(key) == 0) {
          std::string value;
          db->get(key, value);
          term_readers[j] =
              std::shared_ptr<BlockTermReader>(new BlockTermReader(value));
        }
      }

      std::priority_queue<DocidWithPositions, std::vector<DocidWithPositions>,
                          DocidWithPositionsCompare>
          pq;

      for (int k = 0; k < term_readers.size(); ++k) {
        auto reader = term_readers[k];
        if (!reader) continue;
        while (reader->next() != NDOCID) {
          uint32_t docid = reader->docid();
          if (doc_maps[k].count(docid) == 0) continue;
          std::vector<uint32_t> positions = reader->positions();
          uint32_t new_docid = doc_maps[k][docid];
          DocidWithPositions doc;
          doc.docid = new_docid;
          doc.positions = positions;
          pq.emplace(std::move(doc));
        }
      }

      std::vector<uint32_t> docids;
      std::vector<std::vector<uint32_t>> positions;

      while (pq.size() > 0) {
        auto top = pq.top();
        docids.push_back(top.docid);
        positions.push_back(top.positions);
        pq.pop();
      }

      InvertFieldsIndexWriter::flush(out_db, key, docids, positions);

    } while (iter->next() != -1);
  }
  return 0;
}

template <class T, int D>
int IndexMerger::merge_point(
    FieldInfo info, std::vector<PointFieldMeta*> metas, std::vector<File*> kdis,
    std::vector<File*> kdds,
    std::vector<std::unordered_map<uint32_t, uint32_t>>& doc_maps,
    uint32_t max_doc) {
  int count = metas.size();
  int i = 0;
  PointFieldIndexWriter<T, D> writer;
  while (i < count) {
    PointFieldMeta* meta = metas[i];
    File* kdd = kdds[i];
    File* kdi = kdis[i];
    BkdTree<T, D> bkd(*meta, kdi, kdd);
    std::vector<Point<T, D>> values;
    long offset = meta->data_fp_;

    while (bkd.read_block(meta->field_id_, values, offset)) {
      for (auto&& point : values) {
        auto field =
            std::make_shared<PointField<T, D>>(info.get_field_name(), point);
        uint32_t new_docid = doc_maps[point.get_docid()];
        writer.add(new_docid, field);
      }
    }
    ++i;
  }
  writer.flush(info, max_doc, index_option_);
  return 0;
}

uint32_t IndexMerger::build_doc_maps(
    std::vector<std::shared_ptr<FieldValueIndexReader>>& id_readers,
    const std::vector<uint32_t>& max_docids,
    std::vector<std::unordered_map<uint32_t, uint32_t>>& old_to_news,
    std::vector<DocMap>& new_to_old) {
  std::unordered_set<std::string> id_set;
  int new_docid = 1;
  for (int i = 0; i < id_readers.size(); ++i) {
    auto reader = id_readers[i];
    if (!reader) continue;
    std::unordered_map<uint32_t, uint32_t> old_to_new;
    for (uint32_t j = 1; j <= max_docids[i]; ++j) {
      std::vector<uint8_t> value;
      reader->get(j, value);
      std::string svalue(value.begin(), value.end());
      if (id_set.count(svalue) == 0) {
        id_set.insert(svalue);
        DocMap doc_map;
        doc_map.seg_no = i;
        doc_map.ori_docid = j;
        old_to_new[j] = new_docid;
        doc_map.new_docid = new_docid;
        new_to_old.push_back(doc_map);
        new_docid++;
      }
    }
    old_to_news.push_back(old_to_new);
  }
  return new_docid - 1;
}

int IndexMerger::write_segment_info(uint32_t max_docid) {
  auto segment_file_handle = std::unique_ptr<File>(new MMapFile);
  std::string file_si = index_option_.get_segment_info_file();
  segment_file_handle->open(file_si, true, true);
  segment_file_handle->append(&max_docid, sizeof(max_docid));
  time_t now = time(nullptr);
  segment_file_handle->append(&now, sizeof(now));
  segment_file_handle->close();
  return 0;
}

int IndexMerger::merge() {
  IndexReader reader(index_option_);
  reader.open();
  std::string id_field = "id";
  std::vector<std::unordered_map<uint32_t, uint32_t>> old_to_new;
  std::vector<DocMap> new_to_old;
  std::vector<std::shared_ptr<FieldValueIndexReader>> id_readers;
  std::vector<std::shared_ptr<InvertFieldsIndexReader>> readers;
  std::vector<uint32_t> max_docids;
  for (auto&& sub_reader : reader.get_sub_index_readers()) {
    auto segment_info = sub_reader->get_segment_info();
    max_docids.push_back(segment_info.max_docid);
    auto field_values_reader = sub_reader->field_values_reader();
    auto id_reader = field_values_reader->get_reader(id_field);
    id_readers.push_back(id_reader);
    auto invert_fields_index_reader = sub_reader->invert_index_reader();
    readers.push_back(invert_fields_index_reader);
  }

  uint32_t max_docid =
      build_doc_maps(id_readers, max_docids, old_to_new, new_to_old);

  {  // merge invert
    FieldInfo dummy;
    merge_invert(readers, old_to_new, dummy, max_docid);
  }

  auto field_infos = reader.get_field_infos();
  for (auto&& kv : field_infos) {
    auto field_name = kv.first;
    auto field_info = kv.second;
    int index_type = field_info.get_index_type();
    switch (index_type) {
      case 2:
      case 3: {  // numeric
        auto sub_index_readers = reader.get_sub_index_readers();
        std::vector<std::shared_ptr<FieldValueIndexReader>> field_value_readers;
        for (auto&& sub_index_reader : sub_index_readers) {
          auto field_values_index_reader =
              sub_index_reader->field_values_reader();
          std::shared_ptr<FieldValueIndexReader> field_value_reader = nullptr;
          if ((field_value_reader = field_values_index_reader->get_reader(
                   field_name)) != nullptr) {
            field_value_readers.push_back(field_value_reader);
          }
        }
        if (index_type == 2)
          merge_numeric(field_value_readers, new_to_old, field_info, max_docid);
        else
          merge_binary(field_value_readers, new_to_old, field_info, max_docid);
        break;
      }

      case 4: {  // point
        break;
      }
      default:
        break;
    }
  }
  write_segment_info(max_docid);
  reader.close();
  return 0;
}

}  // namespace yas