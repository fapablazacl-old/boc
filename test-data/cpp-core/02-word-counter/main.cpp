
#include <iostream>

#include "WordCounter.hpp"
#include "WordList.hpp"

int main() {
    auto words = get_word_list();
    auto counts = count_words(words);

    std::cout << "The count is done!" << std::endl;

    return 0;
}
