#include "field.h"

namespace yas{
    class TextField:public Field{
        public:
        TextField(std::string name,std::string value);
        virtual ~TextField()=default;
        FieldIndexWriter* get_field_index_writer();
        private:
        std::string value_;
    };
}