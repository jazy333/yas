#include "tfidf_relevance.h"
#include "tfidf_scorer.h"

#include <cmath>

namespace yas {

 RelevanceScorer*  TFIDFRelevance::scorer(float boost,IndexStat* index_stat,TermStat* term_stat){
   float idf=idf(term_stat->get_doc_freq(),index_stat->doc_count);
   return new TFIDFScorer(idf);
 }
 
/*
 *log((docCount+1)/(docFreq+1)) + 1
 */
float TFIDFRelevance::idf(long doc_freq, long doc_count) {
  return log((doc_count + 1) / (doc_freq + 1)) + 1.0;
}
}  // namespace yas