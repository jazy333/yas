#pragma once
#include <cstdint>
namespace yas {
struct SegmentInfo {
  uint32_t max_docid;
  time_t last_update_time;
};

}  // namespace yas