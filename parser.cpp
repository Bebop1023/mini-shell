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

Pipeline parsePipeline(const std::string& line) { // Function to parse a line into a Pipeline structure
    Pipeline result;
    std::vector<std::string> words = split(line);

    for (size_t i = 0; i < words.size(); i++) {
        if (words[i] == ">"){
            if (i +2 != words.size()) {
                result.error = "Syntax error: near '>'";
                return result;
            }
            if (words[i+1] == "|" || words[i+1] == ">") {
                result.error = "Syntax error: near '>'";
                return result;
            }
            result.outfile = words[i + 1];
            words.resize(i);
            break;

        }
    }

  
    std::vector<std::string> current;

    for (const std::string& word : words) { // Iterate through each word in the input line
        if (word == "|") {
            if (current.empty()) {
                result.error = "Syntax error: empty command before pipe";
                return result;
            }
            result.commands.push_back(current);
            current.clear();
        } else {
            current.push_back(word);
        }
    }
        if (current.empty() && !result.commands.empty()) {
        result.error = "Syntax error: empty command after pipe";
        return result;
    }


    if (!current.empty()) {
        result.commands.push_back(current);
    }

        if (!result.outfile.empty() && result.commands.empty()) {
        result.error = "Syntax error: near '>'";
        return result;
    }


    return result;
}
