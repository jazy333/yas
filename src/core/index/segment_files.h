#pragma once
#include <string>

namespace yas {
struct SegmentFiles {
  // invert index
  std::string invert_index_file;  
  // bkd tree files
  std::string kdm;
  std::string kdi;
  std::string kdd;
  // filed values index
  std::string fvm;
  std::string fvd;
  //segment info
  std::string si;
};

}  // namespace yas