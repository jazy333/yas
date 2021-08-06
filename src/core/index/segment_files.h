#pragma once
#include <string>

namespace yas {
struct SegmentFiles {
  std::string invert_index_file;  // invert index
  // bkd tree files
  std::string kdm;
  std::string kdi;
  std::string kdd;
  // filed values index
  std::string fvm;
  std::string fvd;
};

}  // namespace yas