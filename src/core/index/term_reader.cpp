#include "term_reader.h"

namespace yas {

TermReader::TermReader(DB* db, Term* term) : db_(db), term_(term) {}
TermReader::~TermReader() {}
}  // namespace yas