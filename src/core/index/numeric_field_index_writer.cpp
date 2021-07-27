#include "numeric_field_index_writer.h"

namespace yas {
void NumericFieldIndexWriter::flush() {}
void NumericFieldIndexWriter::add(uint32_t docid, Field*) {}
FieldIndexReader* NumericFieldIndexWriter::get_reader() {}
}  // namespace yas