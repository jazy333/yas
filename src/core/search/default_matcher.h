#pragma once

namespace yas {
class DefaultMatcher : public Matcher {
 private:
  /* data */
 public:
  DefaultMatcher (/* args */);
  ~DefaultMatcher ();
  int match(MatchSet& set, int min, int max) override;
};
}  // namespace yas