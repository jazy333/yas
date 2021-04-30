#pragma once

namespace yas {
    class DocIdList {
        public：
            virtual int next() = 0;
            virtual int advance(int target) = 0;
            virtual int docid() {
                return _docid;
            }
        private:
            int _docid;
    };
}