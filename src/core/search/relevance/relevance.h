namespace yas {
class Relevance {
 public:
  virtual float score(float freq,long norm) = 0;
};
}  // namespace yas