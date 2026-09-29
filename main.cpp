#include <iostream>
#include <string>

int main()
{

    while (true)
    {
        std::cout << "mysh> ";
        std::cout.flush();

        std::string line;             // Read a line of input from the user
        if (!std::getline(std::cin, line)) {
            std::cout << std::endl; // Handle end-of-file or input failure
            break; // Print a newline if input fails
        }

        if (line == "exit")
        {          // Check if the user wants to exit
            break; // Exit the loop if the user types "exit"
        }

        std::cout << "You typed: " << line << std::endl; // Output the entered line
    }
    return 0;
}