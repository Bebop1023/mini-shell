#pragma once

#include <string>
#include <vector>

std::vector<std::string> split(const std::string& line);

// Define a struct to represent a pipeline of commands
struct Pipeline {
std::vector<std::vector<std::string>> commands; // Each command is represented as a vector of strings (arguments)
std::string error;
};

Pipeline parsePipeline(const std::string& line); // Function to parse a line into a Pipeline structure
