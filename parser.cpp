#include "parser.h"

#include <sstream>

std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> words;
    std::istringstream stream(line);
    std::string word;
    while (stream >> word) {
        words.push_back(word);
    }
    return words;
}
