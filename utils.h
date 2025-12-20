#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// Parse command line arguments to extract file path
std::string getFilePathFromArgs(int argc, char *argv[]);

// Tokenize a string
std::vector<std::string> tokenize(const std::string &str);

#endif // UTILS_H
