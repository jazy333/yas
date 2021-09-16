#include "index_reader.h"

#include <dirent.h>
#include <unistd.h>

#include <cstdlib>
#include <queue>
#include <set>

#include "log.h"
#include "mmap_file.h"
#include "segment_files.h"
#include "segment_index_reader.h"

namespace yas {
IndexReader::IndexReader(IndexOption option) : option_(option) {
  index_stat_.doc_count = 0;
  index_stat_.max_doc = 0;
  index_stat_.total_term_freq = 0;
}

IndexReader::IndexReader() : option_(IndexOption()) {}

IndexReader::~IndexReader() { close(); }

std::vector<std::shared_ptr<SubIndexReader>>
IndexReader::get_sub_index_readers() {
  return sub_index_readers_;
}

int IndexReader::get_segment_files(std::vector<SegmentFiles>& files) {
  struct dirent** namelist;
  int n;

  n = scandir(option_.dir.c_str(), &namelist, nullptr, alphasort);
  if (n < 0) {
    LOG_ERROR("scan_dir eror");
    return -1;
  } else {
    std::set<uint64_t> seqs;
    while (n--) {
      LOG_INFO("index dir :%s", namelist[n]->d_name);
      std::string file_name = namelist[n]->d_name;
      free(namelist[n]);
      if (file_name.find(option_.segment_prefix) == 0) {
        size_t pos = option_.segment_prefix.size();
        uint64_t seq = std::stoul(file_name.substr(pos), &pos);
        if (seqs.count(seq) == 1)
          continue;
        else
          seqs.insert(seq);

        SegmentFiles sf;
        std::string full_path_prefix =
            option_.dir + "/" + option_.segment_prefix + std::to_string(seq);
        std::string index_file = full_path_prefix + ".hdb";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.invert_index_file = index_file;
        }

        index_file = full_path_prefix + ".fvm";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.fvm = index_file;
        }

        index_file = full_path_prefix + ".fvd";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.fvd = index_file;
        }

        index_file = full_path_prefix + ".kdm";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.kdm = index_file;
        }

        index_file = full_path_prefix + ".kdi";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.kdi = index_file;
        }

        index_file = full_path_prefix + ".kdd";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.kdd = index_file;
        }

        index_file = full_path_prefix + ".si";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.si = index_file;
        }

        files.push_back(sf);
      }
    }
    free(namelist);
  }
  return 0;
}

int IndexReader::open() {
  option_.read_stat(index_stat_);
  option_.read_field_info(field_infos_);
  std::vector<SegmentFiles> files;
  std::priority_queue<std::shared_ptr<SubIndexReader>,
                      std::vector<std::shared_ptr<SubIndexReader>>,
                      SubIndexReaderCompareWithUpdatetime>
      pq;
  int ret = get_segment_files(files);
  for (size_t i = 0; i < files.size(); ++i) {
    std::shared_ptr<SubIndexReader> segment_reader =
        std::shared_ptr<SegmentIndexReader>(
            new SegmentIndexReader(files[i], field_infos_));
    int ret = segment_reader->open();
    if (ret < 0) continue;
    pq.push(segment_reader);
    // sub_index_readers_.push_back(segment_reader);
  }

  int max_segment_used = option_.mode <= 0 ? pq.size() : option_.mode;
  while (!pq.empty() && sub_index_readers_.size() < max_segment_used) {
    auto reader = pq.top();
    sub_index_readers_.push_back(reader);
    pq.pop();
  }

  while (!pq.empty()) {
    auto reader = pq.top();
    reader->close();
    if (option_.auto_evict) reader->unlink();
    pq.pop();
  }
  return 0;
}

void IndexReader::add(std::shared_ptr<SubIndexReader> reader) {
  sub_index_readers_.push_back(reader);
}

int IndexReader::close() {
  for (auto&& reader : sub_index_readers_) {
    if (reader) reader->close();
  }
  sub_index_readers_.clear();
  return 0;
}

int IndexReader::reopen() {
  close();
  open();
  return 0;
}

IndexStat IndexReader::get_index_stat() { return index_stat_; }

std::unordered_map<std::string, FieldInfo> IndexReader::get_field_infos() {
  return field_infos_;
}

void IndexReader::set_option(const IndexOption& option) { option_ = option; }

IndexOption& IndexReader::get_option() { return option_; }

}  // namespace yas