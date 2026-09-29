#include <iostream>
#include <string>
#include <sstream> // Include the necessary header for string stream
#include <vector>

std::vector<std::string> split(const std::string& line) { 
    std::vector<std::string> words; // Create a vector to hold the split words
    std::istringstream stream(line); // Create a string stream from the input line
    std::string word; 
    while (stream >> word) {
        words.push_back(word);
    }
    return words;
}



int main(){

    while (true)
    {
        std::cout << "mysh> "; // Display the prompt for user input
        std::cout.flush(); // Flush the output buffer to ensure the prompt is displayed immediately

        std::string line;             // Read a line of input from the user
        if (!std::getline(std::cin, line)) {
            std::cout << std::endl; // Handle end-of-file or input failure
            break; // Print a newline if input fails
        }

  

        std::vector<std::string> words = split(line);

                if (words.empty()) {
            continue;
        }

        if (words[0] == "exit") {
            break;
        }

        for (const std::string& w : words) {
            std::cout << "[" << w << "]";
        }
        std::cout << std::endl;

    }

    return 0;
}
