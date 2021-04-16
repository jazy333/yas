#include <string>
#include <vector>
int LevenshteinDistance(const char* a, const char* b);
int LevenshteinDistance(const std::string& a, const std::string& b);
void permute(const std::vector<int>&, std::vector<std::vector<int>>&);
template<class ForwardIt>
ForwardIt max_element(ForwardIt first, ForwardIt last)
{
    if (first == last) return last;

    ForwardIt largest = first;
    ++first;
    for (; first != last; ++first) {
        if (*largest < *first) {
            largest = first;
        }
    }
    return largest;
}