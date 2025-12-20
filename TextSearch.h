#ifndef TEXT_SEARCH_H
#define TEXT_SEARCH_H

#include <vector>
#include <string>

class TextSearch
{
private:
    std::vector<std::string> fileLines;
    std::string filePath;

    bool charsMatch(char textChar, char patternChar, bool caseSensitive);
    std::vector<int> computeLPS(const std::string &pattern, bool caseSensitive);
    bool isValidMatch(const std::string &line, int idx, int patternLen, std::string mode);

public:
    TextSearch() {}

    bool loadFile(const std::string &path);
    void search(std::string keyword, bool caseSensitive = true, std::string matchMode = "Substring");
    void search(std::string keyword, std::string outputPath, bool caseSensitive, std::string matchMode);
    void replace(std::string keyword, std::string target, std::string outPath, bool caseSensitive, std::string matchMode);
};

#endif // TEXT_SEARCH_H
