#include "TextSearch.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <chrono>
#include <cstring>
#include <cerrno>
using namespace std;

bool TextSearch::charsMatch(char textChar, char patternChar, bool caseSensitive)
{
    if (caseSensitive)
    {
        return textChar == patternChar;
    }
    return tolower(textChar) == tolower(patternChar);
}

// O(M)
vector<int> TextSearch::computeLPS(const string &pattern, bool caseSensitive)
{
    int m = pattern.length();
    vector<int> lps(m);
    int len = 0; // Length of previous longest prefix suffix
    lps[0] = 0;
    int i = 1;

    while (i < m)
    {
        if (charsMatch(pattern[i], pattern[len], caseSensitive))
        {
            len++;
            lps[i] = len;
            i++;
        }
        else
        {
            if (len != 0)
            {
                len = lps[len - 1];
            }
            else
            {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

bool TextSearch::isValidMatch(const string &line, int idx, int patternLen, string mode)
{
    if (mode == "Substring")
        return true;

    bool isPrefix = (idx == 0) || !isalnum(line[idx - 1]);

    if (mode == "Prefix")
        return isPrefix;

    if (mode == "Whole")
    {
        bool isSuffix = (idx + patternLen == line.length()) || !isalnum(line[idx + patternLen]);
        return isPrefix && isSuffix;
    }

    return true;
}

bool TextSearch::loadFile(const string &path)
{
    this->filePath = path;
    ifstream file(path);

    if (!file.is_open())
    {
        cerr << "Error: Could not open file at " << path << strerror(errno) << endl;
        return false;
    }

    fileLines.clear();
    string line;

    while (getline(file, line))
    {
        fileLines.push_back(line);
    }

    file.close();
    cout << "Successfully loaded " << fileLines.size() << " lines from " << path << endl;
    return true;
}

// Search and Print Highlighted Results
void TextSearch::search(string keyword, bool caseSensitive, string matchMode)
{

    cout << "\n--- Search Results (" << keyword << ") ---" << endl;

    // Start Timer
    auto start = chrono::high_resolution_clock::now();

    vector<int> lps = computeLPS(keyword, caseSensitive);
    int m = keyword.length();
    int totalMatches = 0;

    for (int lineNum = 0; lineNum < fileLines.size(); lineNum++)
    {
        const string &text = fileLines[lineNum];
        int n = text.length();
        int i = 0; // index for text
        int j = 0; // index for pattern

        while (i < n)
        {
            if (charsMatch(text[i], keyword[j], caseSensitive))
            {
                i++;
                j++;
            }

            if (j == m) // found the whole pattern
            {
                int matchIndex = i - j;

                if (isValidMatch(text, matchIndex, m, matchMode))
                {
                    totalMatches++;

                    string highlighted = text;
                    highlighted.insert(matchIndex + m, "]");
                    highlighted.insert(matchIndex, "[");

                    cout << "Line " << (lineNum + 1) << ", Index " << matchIndex << ": " << highlighted << endl;
                }

                j = lps[j - 1];
            }
            else if (i < n && !charsMatch(text[i], keyword[j], caseSensitive))
            {
                if (j != 0)
                    j = lps[j - 1];
                else
                    i++;
            }
        }
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    cout << "--- Stats ---" << endl;
    cout << "Total Occurrences: " << totalMatches << endl;
    cout << "Time Taken (Searching): " << duration.count() << " microseconds" << endl;
}

// Search and Save Results to File
void TextSearch::search(string keyword, string outputPath, bool caseSensitive, string matchMode)
{
    ofstream outFile(outputPath);
    if (!outFile.is_open())
    {
        cerr << "Error: Could not open output file " << outputPath << endl;
        return;
    }

    outFile << "--- Search Results (" << keyword << ") ---" << endl;

    auto start = chrono::high_resolution_clock::now();

    vector<int> lps = computeLPS(keyword, caseSensitive);
    int m = keyword.length();
    int totalMatches = 0;

    for (int lineNum = 0; lineNum < fileLines.size(); lineNum++)
    {
        const string &text = fileLines[lineNum];
        int n = text.length();
        int i = 0; // index for text
        int j = 0; // index for pattern

        while (i < n)
        {
            if (charsMatch(text[i], keyword[j], caseSensitive))
            {
                i++;
                j++;
            }

            if (j == m) // found the whole pattern
            {
                int matchIndex = i - j;

                if (isValidMatch(text, matchIndex, m, matchMode))
                {
                    totalMatches++;

                    string highlighted = text;
                    highlighted.insert(matchIndex + m, "]");
                    highlighted.insert(matchIndex, "[");

                    outFile << "Line " << (lineNum + 1) << ", Index " << matchIndex << ": " << highlighted << endl;
                }

                j = lps[j - 1];
            }
            else if (i < n && !charsMatch(text[i], keyword[j], caseSensitive))
            {
                if (j != 0)
                    j = lps[j - 1];
                else
                    i++;
            }
        }
    }

    // Stop Timer
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    outFile << "--- Stats ---" << endl;
    outFile << "Total Occurrences: " << totalMatches << endl;
    outFile << "Time Taken (Searching): " << duration.count() << " microseconds" << endl;

    outFile.close();
    cout << "--- Search Stats ---" << endl;
    cout << "Total Occurrences: " << totalMatches << " occurrences." << endl;
    cout << "Output saved to: " << outputPath << endl;
    cout << "Time taken (Searching): " << duration.count() << " microseconds" << endl;
}

// Phase 4: Replace Functionality
void TextSearch::replace(string keyword, string target, string outPath, bool caseSensitive, string matchMode)
{
    ofstream outFile(outPath);
    if (!outFile.is_open())
    {
        cerr << "Error: Could not open output file " << outPath << endl;
        return;
    }

    long long totalDuration = 0;
    int replacements = 0;

    vector<int> lps = computeLPS(keyword, caseSensitive);
    int m = keyword.length();

    for (const string &line : fileLines)
    {
        auto start = chrono::high_resolution_clock::now();

        string newLine = "";
        int n = line.length();
        int i = 0;
        int j = 0;
        int lastAddedPos = 0;

        while (i < n)
        {
            if (charsMatch(line[i], keyword[j], caseSensitive))
            {
                i++;
                j++;
            }

            if (j == m)
            {
                int matchIndex = i - j;
                if (isValidMatch(line, matchIndex, m, matchMode))
                {
                    // Found valid match.
                    newLine += line.substr(lastAddedPos, matchIndex - lastAddedPos);
                    newLine += target;
                    lastAddedPos = i;
                    replacements++;

                    j = 0;
                }
                else
                {
                    j = lps[j - 1];
                }
            }
            else if (i < n && !charsMatch(line[i], keyword[j], caseSensitive))
            {
                if (j != 0)
                    j = lps[j - 1];
                else
                    i++;
            }
        }
        newLine += line.substr(lastAddedPos);

        auto stop = chrono::high_resolution_clock::now();
        totalDuration += chrono::duration_cast<chrono::microseconds>(stop - start).count();

        outFile << newLine << "\n";
    }

    outFile.close();
    cout << "--- Replace Stats ---" << endl;
    cout << "Replaced " << replacements << " occurrences." << endl;
    cout << "Output saved to: " << outPath << endl;
    cout << "Time taken (Processing): " << totalDuration << " microseconds" << endl;
}
