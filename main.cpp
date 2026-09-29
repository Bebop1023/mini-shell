#include <iostream>

int main() {

    while (true) {
    std::cout << "mysh> ";
    std::cout.flush();

    std::string line; // Read a line of input from the user
    std::getline(std::cin, line);  // Get the input line from the user

    if (line == "exit") { // Check if the user wants to exit
        break; // Exit the loop if the user types "exit"
    }

    std::cout << "You typed: " << line << std::endl; // Output the entered line
    }
    return 0;
    
}