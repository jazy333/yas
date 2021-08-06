#include "index_reader.h"

#include "dirent.h"
#include "segment_files.h"

namespace yas {
IndexReader::IndexReader(const std::string& dir) : dir_(dir) {}
IndexReader::~IndexReader() {}
std::vector<SubIndexReader*> IndexReader::get_sub_index_reader() {
  return sub_index_readers_;
}

int scan_dir({std::vector<segment_files> & files}) {
  struct dirent** namelist;
  int n;

  n = scandir(dir_.c_str(), &namelist, NULL, alphasort);
  if (n < 0)
    perror("scandir");
  else {
    while (n--) {
      printf("%s\n", namelist[n]->d_name);
      free(namelist[n]);
    }
    free(namelist);
  }
}

int IndexReader::open() {
  std::vector<segment_files>& files;
   int ret = scandir(files);
  for (int i = 0; i < files.size(); ++i) {
    SegmentIndexReader* segment_reader = new SegmentIndexReader(files[i]);
    sub_index_readers_.push_back(segment_reader);
  }
}

void IndexReader::add(SubIndexReader* reader) {
  sub_index_readers_.push_back(reader);
}
}  // namespace yas