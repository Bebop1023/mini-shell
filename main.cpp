#include <iostream>
#include <string>
#include "parser.h"
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <sys/wait.h>
#include <cstdlib>
#include <fcntl.h>
#include <csignal>



// Function to run a command represented as a vector of strings (arguments) and redirect output to a specified file if provided
void runCommand(std::vector<std::string>& words, const std::string& outfile) { 
    signal(SIGINT, SIG_DFL);

    if (!outfile.empty()) {
        int fd = open(outfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open the output file for writing, creating it if it doesn't exist, and truncating it if it does
        if (fd < 0) {
            perror(outfile.c_str());
            _exit(1);
        }
        dup2(fd, 1); // Redirect standard output to the specified output file
        close(fd);
    }
    
    std::vector<char*> args;
    for (std::string& w : words) {
        args.push_back(w.data());
    }
    args.push_back(nullptr);

    execvp(args[0], args.data());
    perror(args[0]);
    _exit(127);
}
// Function to run a pipeline of two commands represented as vectors of strings (arguments)
void runPipeline(std::vector<std::string>& left, std::vector<std::string>& right, const std::string& outfile) { 
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe");
        return;
    }
    // Create a new process for the left command
    pid_t pid1 = fork();
        if (pid1 < 0) { // Check for fork error
        perror("fork");
        close(fds[0]);
        close(fds[1]);
        return;
    }
    // In the child process for the left command, redirect standard output to write to the pipe and execute the command
    if (pid1 == 0) {
        dup2(fds[1], 1);
        close(fds[0]);
        close(fds[1]);
        runCommand(left, "");
    }
    // Create a new process for the right command
    pid_t pid2 = fork();
        if (pid2 < 0) { // Check for fork error
        perror("fork");
        close(fds[0]);
        close(fds[1]);
        waitpid(pid1, nullptr, 0);
        return;
    }
    // In the child process for the right command, redirect standard input to read from the pipe and execute the command
    if (pid2 == 0) {
        dup2(fds[0], 0);
        close(fds[0]);
        close(fds[1]);
        runCommand(right, outfile);
    }
    // Close the pipe file descriptors in the parent process and wait for both child processes to finish
    close(fds[0]);
    close(fds[1]);
    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);
}












int main(){

    signal(SIGINT, SIG_IGN); // Ignore the SIGINT signal (Ctrl+C) in the shell process


    while (true)
    {
        std::cout << "mysh> "; // Display the prompt for user input
        std::cout.flush(); // Flush the output buffer to ensure the prompt is displayed immediately

        std::string line;             // Read a line of input from the user
        if (!std::getline(std::cin, line)) {
            std::cout << std::endl; // Handle end-of-file or input failure
            break; // Print a newline if input fails
        }

  

        Pipeline p = parsePipeline(line); // Parse the input line into a Pipeline structure

        if (!p.error.empty()) {
            std::cerr << "mysh: " << p.error << std::endl;
            continue;
        }

        if (p.commands.empty()) { // Handle the case where no commands were parsed from the input line
            continue;
        }

        if (p.commands.size() > 2) {
            std::cerr << "mysh: only one pipe is supported" << std::endl;
            continue;
        }

        if (p.commands.size() == 2) {
            runPipeline(p.commands[0], p.commands[1], p.outfile); // Execute the pipeline of two commands
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



        pid_t pid = fork(); // Create a new process using fork

        if (pid < 0){
            perror("fork");
            continue;
        }

        if (pid == 0) {
                    runCommand(words, p.outfile); // Execute the command in the child process

        }
                waitpid(pid, nullptr, 0);




    }

    return 0;
}
