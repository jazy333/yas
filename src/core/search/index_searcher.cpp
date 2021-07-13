#include "index_searcher.h"
#include "matched_doc.h"
#include "matcher.h"

namespace yas {
IndexSearcher::IndexSearcher(IndexReader* reader) {}

IndexSearcher::~IndexSearcher() {}

void IndexSearcher::search(Query* q, MatchSet& set) {
  std::vector<SubIndexReader*> sub_index_readers =
      reader_->get_sub_index_reader();
  for (auto&& sub_index_reader : sub_index_readers) {
    std::unique_ptr<Matcher> matcher = q->matcher(sub_index_reader);
    PostingList* pl = matcher->posting_list();
    Scorer scorer = matcher->scorer();
    while (pl->next() ！ = PostingList::NDOCID) {
      MatchedDoc md;
      md.docid_ = pl->docid();
      md.score_ = scorer->score();
      set.add(md);
    }
  }
}

std::shared_ptr<Query> IndexSearcher::rewrite(std::shared_ptr<Query> query) {
  std::shared_ptr<Query> original = query;
  do {
    original = query;
    query = query->rewrite();
  } while (query.get() != original.get());
  return query;
}
}  // namespace yas