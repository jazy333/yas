#include "sub_index_reader.h"
#include <string>
namespace yas {
class MemoryIndexReader : public SubIndexReader {
 public:
  MemoryIndexReader(
      std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers);
  virtual ~MemoryIndexReader();
  virtual TermReader* posting_list(Term*) override;
  virtual FieldIndexReadr* field_reader(std::string field_name) override;

 private:
  std::unordered_map<std::string, FieldIndexWriter*>* field_index_writers_;
};

}  // namespace yas