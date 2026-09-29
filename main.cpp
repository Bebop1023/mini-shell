#include <iostream>
#include <string>
#include <sstream> // Include the necessary header for string stream
#include <vector>

std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> words;
    std::istringstream stream(line);
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

        if (line == "exit")
        {          // Check if the user wants to exit
            break; // Exit the loop if the user types "exit"
        }

        std::vector<std::string> words = split(line);
        for (const std::string& w : words) {
            std::cout << "[" << w << "]";
        }
        std::cout << std::endl;

    }

    return 0;
}
