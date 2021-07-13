#include "hash_db.h"
#include "posting_list.h"
#include "term.h"

namespace yas {
class TermReader : public PostingList {
 public:
  virtual int freq() = 0;
  virtual int next_postion() = 0;
};
}  // namespace yas