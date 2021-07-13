#include "any_match_query.h"

namespace yas {
Query* AnyMatchQuery::rewrite() {}
PostingList* AnyMatchQuery::posting_list(SubIndexReader* sub_reader) {}
Scorer* AnyMatchQuery::scorer() {}
}  // namespace yas