#include "index_searcher.h"

#include "common/common.h"
#include "matched_doc.h"
#include "matcher.h"

namespace yas {
IndexSearcher::IndexSearcher(std::shared_ptr<IndexReader> reader)
    : reader_(reader) {}

IndexSearcher::IndexSearcher() : reader_(nullptr) {}

IndexSearcher::~IndexSearcher() {}

void IndexSearcher::search(Query* q, MatchSet& set) {
  if (!reader_) return;
  std::vector<std::shared_ptr<SubIndexReader>> sub_index_readers =
      reader_->get_sub_index_readers();
  for (auto&& sub_index_reader : sub_index_readers) {
    auto matcher = q->matcher(sub_index_reader.get());
    auto pl = matcher->posting_list();
    auto scorer = matcher->scorer();
    auto field_values_reader = sub_index_reader->field_values_reader();
    auto id_reader = field_values_reader->get_reader("id");
    while (pl->next() != NDOCID) {
      MatchedDoc md;
      md.docid_ = pl->docid();
      md.score_ = scorer->score();
      md.field_value_reader = field_values_reader;
      auto hits = pl->hit();

      std::string id;
      if (id_reader) {
        std::vector<uint8_t> value;
        id_reader->get(md.docid_, value);
        id.assign(value.begin(), value.end());
      }
      std::cout << "docid:" << md.docid_ << ",score:" << md.score_
                << ",id:" << id << std::endl;
      for (auto&& kv : hits) {
        md.match_term_counts[kv.first] = kv.second;
        std::string doc_len_field = "__" + kv.first + "_dl";
        auto doc_len_reader = field_values_reader->get_reader(doc_len_field);
        if (doc_len_reader) {
          uint64_t norm = 0;
          doc_len_reader->get(md.docid_, norm);
          norm = uchar2uint(norm);
          md.doc_lens[kv.first] = norm;
        }
      }
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

void IndexSearcher::set_reader(std::shared_ptr<IndexReader> reader) {
  reader_ = reader;
}

std::shared_ptr<IndexReader> IndexSearcher::get_reader() { return reader_; }

}  // namespace yas