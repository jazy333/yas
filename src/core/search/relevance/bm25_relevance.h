#pragma once
#include "relevance.h"

namespace yas {
class BM25Relevance : public Relevance {
 public:
  BM25Relevance(float k1, float b);
  BM25Relevance();
  ~BM25Relevance();
  float score(float term_freq, long norm) override;

 protected:
  float idf(long doc_freq, long doc_count);

 private:
  float k1_;
  float b_;

}  // namespace yas