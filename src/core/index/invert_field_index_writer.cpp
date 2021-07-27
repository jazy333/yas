#include "invert_field_index_writer.h"
#include "simd_binary_compression.h"


namespace yas {
InvertFieldIndexWriter::InvertFieldIndexWriter(/* args */);
InvertFieldIndexWriter::~InvertFieldIndexWriter();
void InvertFieldIndexWriter::flush() {

}
void InvertFieldIndexWriter::add(uin32_t docid, Field* field) {
  TextField* tf = dynamic_cast<TextField*>(field);
  std::vector<int> postion_list;
  TermIterator* ti = tokenizer_->get_term_iterator(tf->get_value());
  while (ti->next()) {
    Term term = ti->term();
    term.set_field(tf->get_field());
    int postion = ti->postion();
    std::string key = term->get_term() + term->get_field();
    if (position_lists_.count(key) == 0) {
      std::vector<uint32_t> posting_list;
      posting_list.push_back(docid);
      std::vector<std::vector<int>> postion_lists;
      std::vector<int> position_list;
      position_list.push_back(postion);
      postion_lists.push_back(postion_list);
      position_lists_[key] = position_list;

    } else {
      if (docid == posting_lists_[key].rbegin()) {
        position_lists_[key].rbegin()->push_back(postion);
      } else {
        posting_lists_[key].push_back(docid);
        std::vector<int> position_list;
        position_list.push_back(postion);
        postion_lists.push_back(postion_list);
        position_lists_[key] = position_list;
      }
    }

    position_list.push_back(position);
  }
}
FieldIndexReader* InvertFieldIndexWriter::get_reader() {}
}  // namespace yas
