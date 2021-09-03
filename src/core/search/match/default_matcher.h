#pragma once
#include <memory>

#include "matcher.h"

namespace yas {
class DefaultMatcher : public Matcher {
 public:
  DefaultMatcher(std::shared_ptr<PostingList> pl,
                 std::shared_ptr<Scorer> scorer);
  virtual ~DefaultMatcher();
  std::shared_ptr<PostingList> posting_list() override;
  std::shared_ptr<Scorer> scorer() override;

 private:
  std::shared_ptr<PostingList> pl_;
  std::shared_ptr<Scorer> scorer_;
};
}  // namespace yas