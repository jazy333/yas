#include "index_reader.h"

#include <dirent.h>
#include <unistd.h>

#include <cstdlib>
#include <set>

#include "log.h"
#include "segment_files.h"

namespace yas {
IndexReader::IndexReader(IndexOption option) : option_(option) {}

IndexReader::IndexReader() option_(IndexOption()) {}

IndexReader::~IndexReader() { close(); }

std::vector<std::shared_ptr<SubIndexReader>>
IndexReader::get_sub_index_readers() {
  return sub_index_readers_;
}

int IndexReader::scan_dir(std::vector<SegmentFiles>& files) {
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
      string file_name = namelist[n]->d_name;
      free(namelist[n]);
      if (file_name.find(option_.segment_prefix) == 0) {
        size_t pos = option_.segment_prefix.size();
        uint64_t seq = std::stoul(file_name, &pos);
        if (seqs.count(seq) == 1)
          continue;
        else
          seqs.insert(seq);

        SegmentFiles sf;
        std::string full_path_prefix =
            option_.dir + "/" + option_.segment_prefix + std::to_string(seq);
        std::string index_file = full_path + ".hdb";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.invert_index_file = index_file;
        }

        index_file = full_path + ".fvm";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.fvm = index_file;
        }

        index_file = full_path + ".fvd";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.fvd = index_file;
        }

        index_file = full_path + ".kvm";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.kvm = index_file;
        }

        index_file = full_path + ".kvi";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.kvi = index_file;
        }

        index_file = full_path + ".kvd";
        if (access(index_file.c_str(), R_OK) == 0) {
          sf.kvd = index_file;
        }

        files.push_back(sf);
      }
    }
    free(namelist);
  }
}

int IndexReader::open() {
  std::vector<SegmentFiles> files;
  int ret = scandir(files);
  for (size_t i = 0; i < files.size(); ++i) {
    std::shared_ptr<SegmentIndexReader> segment_reader =
        new SegmentIndexReader(files[i],field_infos_);
    segment_reader->open();
    sub_index_readers_.push_back(segment_reader);
  }
}

void IndexReader::add(std::shared_ptr<SubIndexReader> reader) {
  sub_index_readers_.push_back(reader);
}

int IndexReader::close() {}
}  // namespace yas