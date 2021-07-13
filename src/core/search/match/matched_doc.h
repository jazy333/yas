#pragma once
#include <cstdint>

namespace yas {
struct MatchedDoc {
  uint32_t docid_;
  float score_;
};
}  // namespace yas
