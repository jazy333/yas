#include "block_term_reader.h"
#include "bm25_relevance.h"

namespace yas {
uint32_t BlockTermReader::next() {}
uint32_t BlockTermReader::advance(uint32_t target) {}
uint32_t BlockTermReader::docid() {}
long BlockTermReader::cost() {}
std::string BlockTermReader::name() { return "BlockTermReader"; }
float BlockTermReader::score() { return 0.0f; }
Scorer* BlockTermReader::scorer() {
  return new TermScorer(this, new BM25Relevance());
}
int BlockTermReader::freq() {}
int BlockTermReader::next_postion() {}
};
}