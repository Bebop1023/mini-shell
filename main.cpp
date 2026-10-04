#include <iostream>
#include <string>
#include "parser.h"
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <sys/wait.h>
#include <cstdlib>









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

  

        Pipeline p = parsePipeline(line);

        if (!p.error.empty()) {
            std::cerr << "mysh: " << p.error << std::endl;
            continue;
        }

        if (p.commands.empty()) {
            continue;
        }

        if (p.commands.size() == 2) {
            std::cout << "pipe: " << p.commands[0][0] << " | " << p.commands[1][0] << std::endl;
            continue;
        }

        std::vector<std::string> words = p.commands[0];


        if (words[0] == "exit") {
            break;
        }

        if (words[0] == "cd") {
            const char* dir; 
            if (words.size() < 2) {
                dir = getenv("HOME"); // Use getenv to get the value of the HOME environment variable
            } else {
                dir = words[1].c_str(); // Use c_str() to get a C-style string from the std::string
            }
            if (dir == nullptr || chdir(dir) != 0) { // Use chdir to change the current working directory
                perror("cd");
            }
            continue;
        }



               std::vector<char*> args;
        for (std::string& w : words) {
            args.push_back(w.data());   // Use data() to get a pointer to the underlying character array of the std::string
        }
        args.push_back(nullptr);

        pid_t pid = fork(); // Create a new process using fork

        if (pid < 0){
            perror("fork");
            continue;
        }

        if (pid == 0) {
            execvp(args[0], args.data()); // Execute the command in the child process
             perror(args[0]); 
            _exit(127);
            
        }
                waitpid(pid, nullptr, 0);




    }

    return 0;
}
