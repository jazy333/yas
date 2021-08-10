#pragma once
#include <string>
namespace yas {
struct IndexOption {
  IndexOption()
      : dir("./"), segment_prefix("segment_"), current_segment_no(0), mode(0) {}
  IndexOption(std::string dir, std::string segment_prefix,
                    uint64_t current_segment_no, int mode)
      : dir(dir),
        segment_prefix(segment_prefix),
        current_segment_no(current_segment_no),
        mode(mode) {}
  std::string dir;
  std::string segment_prefix;
  uint64_t current_segment_no;
  int mode;
};
}  // namespace yas