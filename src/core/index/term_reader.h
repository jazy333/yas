#include "term.h"
#include "hash_db.h"

namespace yas{
    class TermReader{
        public:
        TermReader(DB* db,Term* term);
        virtual ~TermReader();
        private:
        DB* hdb_;
        Term* term_;
    };
}