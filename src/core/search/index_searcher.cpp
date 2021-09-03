#include "index_searcher.h"
#include "matched_doc.h"
#include "matcher.h"

namespace yas {
IndexSearcher::IndexSearcher(IndexReader* reader) : reader_(reader) {}

IndexSearcher::~IndexSearcher() {}

void IndexSearcher::search(Query* q, MatchSet& set) {
  std::vector<std::shared_ptr<SubIndexReader>> sub_index_readers =
      reader_->get_sub_index_readers();
  for (auto&& sub_index_reader : sub_index_readers) {
    std::unique_ptr<Matcher> matcher = q->matcher(sub_index_reader.get());
    PostingList* pl = matcher->posting_list();
    Scorer* scorer = matcher->scorer();
    auto field_values_reader=sub_index_reader->field_values_reader();
    auto id_reader=field_values_reader->get_reader("id");
    while (pl->next() != NDOCID) {
      MatchedDoc md;
      md.docid_ = pl->docid();
      md.score_ = scorer->score();
      set.add(md);
      std::string id;
      if(id_reader){
        std::vector<uint8_t> value;
        id_reader->get(md.docid_,value);
        id.assign(value.begin(),value.end());
      }
      std::cout<<"docid:"<<md.docid_<<",score:"<<md.score_<<",id:"<<id<<std::endl;
    }
  }
}

std::shared_ptr<Query> IndexSearcher::rewrite(std::shared_ptr<Query> query) {
  std::shared_ptr<Query> original = query;
  do {
    original = query;
    query.reset(query->rewrite());
  } while (query.get() != original.get());
  return query;
}

}  // namespace yas