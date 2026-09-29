#include <iostream>

int main() {
    std::cout << "mysh> ";
    std::cout.flush();

    std::string line; // Read a line of input from the user
    std::getline(std::cin, line);  // Get the input line from the user

    std::cout << "You typed: " << line << std::endl; // Output the entered line
    return 0;
    
}