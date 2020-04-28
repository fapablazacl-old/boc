
#include "WordCounter.hpp"

std::map<std::string, int> count_words(const std::vector<std::string> &words) {
    std::map<std::string, int> counts;

    for (const std::string &word: words) {
        if (auto it=counts.find(word); it!=counts.end()) {
            it->second++;
        } else {
            counts[word] = 1;
        }
    }

    return counts;
}
