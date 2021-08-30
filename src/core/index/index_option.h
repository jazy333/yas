#pragma once
#include <string>
#include <unordered_map>

#include "field_info.h"
#include "index_stat.h"
#include "mmap_file.h"

namespace yas {
struct IndexOption {
  IndexOption()
      : dir("./"),
        segment_prefix("segment_"),
        stat_file("index.stat"),
        field_info_file("index.fi"),
        current_segment_no(0),
        mode(0) {}

  IndexOption(std::string dir, std::string segment_prefix,
              std::string stat_file, std::string field_info,
              uint64_t current_segment_no, int mode)
      : dir(dir),
        segment_prefix(segment_prefix),
        stat_file(stat_file),
        field_info_file(field_info),
        current_segment_no(current_segment_no),
        mode(mode) {}

  int read_stat(IndexStat& index_stat) {
    auto stat_file_handle = std::unique_ptr<File>(new MMapFile());
    stat_file_handle->open(dir + "/" + stat_file, false);
    stat_file_handle->read(&index_stat.doc_count, sizeof(index_stat.doc_count));
    stat_file_handle->read(&index_stat.max_doc, sizeof(index_stat.max_doc));
    stat_file_handle->read(&index_stat.total_term_freq,
                    sizeof(index_stat.total_term_freq));
    stat_file_handle->close();
    return 0;
  }

  int write_stat(IndexStat& index_stat) {
    auto stat_file_handle = std::unique_ptr<File>(new MMapFile());
    stat_file_handle->open(dir + "/" + stat_file, true, true);
    stat_file_handle->append(&index_stat.doc_count, sizeof(index_stat.doc_count));
    stat_file_handle->append(&index_stat.max_doc, sizeof(index_stat.max_doc));
    stat_file_handle->append(&index_stat.total_term_freq,
                      sizeof(index_stat.total_term_freq));
    stat_file_handle->close();
    return 0;
  }

  int read_field_info(std::unordered_map<std::string, FieldInfo>& field_infos) {
    auto field_infos_handle = std::unique_ptr<File>(new MMapFile());
    field_infos_handle->open(dir + "/" + field_info_file, false);
    int max_field_id = 0;
    while (true) {
      FieldInfo fi;
      int field_name_len;
      int ret =
          field_infos_handle->read(&field_name_len, sizeof(field_name_len));
      if (field_name_len == 0 || ret < 0) break;
      std::string field_name;
      field_name.resize(field_name_len, 0);
      field_infos_handle->read(const_cast<char*>(field_name.data()), field_name_len);
      int field_id = -1;
      field_infos_handle->read(&field_id, sizeof(field_id));
      fi.set_field_id(field_id);
      fi.set_field_name(field_name);
      field_infos[field_name] = fi;
      if (field_id > max_field_id) max_field_id = field_id;
    }
    field_infos_handle->close();
    return max_field_id;
  }

  int write_field_info(
      std::unordered_map<std::string, FieldInfo>& field_infos) {
    auto field_infos_handle = std::unique_ptr<File>(new MMapFile());
    field_infos_handle->open(dir + "/" + field_info_file, true, true);
    for (auto&& kv : field_infos) {
      std::string field_name = kv.first;
      FieldInfo fi = kv.second;
      int field_name_len = field_name.size();
      field_infos_handle->append(&field_name_len, sizeof(field_name_len));
      field_infos_handle->append(field_name.data(), field_name_len);
      int field_id = fi.get_field_id();
      field_infos_handle->append(&field_id, sizeof(int));
    }

    // write an end
    field_infos_handle->append(0, sizeof(int));
    field_infos_handle->close();
    return 0;
  }

  std::string dir;
  std::string segment_prefix;
  std::string stat_file;
  std::string field_info_file;
  uint64_t current_segment_no;
  int mode;
};
}  // namespace yas