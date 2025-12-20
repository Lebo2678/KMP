#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring> // for strerror
#include <cctype>  // for tolower, isalnum
#include <chrono>  // For timing
using namespace std;

class TextSearch
{
private:
    vector<string> fileLines;
    string filePath;

    bool charsMatch(char textChar, char patternChar, bool caseSensitive)
    {
        if (caseSensitive)
        {
            return textChar == patternChar;
        }
        return tolower(textChar) == tolower(patternChar);
    }

    // O(M)
    vector<int> computeLPS(const string &pattern, bool caseSensitive)
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

    bool isValidMatch(const string &line, int idx, int patternLen, string mode)
    {
        if (mode == "Substring")
            return true;

        // Check Prefix Condition (Must be at start of line OR after a separator)
        bool isPrefix = (idx == 0) || !isalnum(line[idx - 1]);

        if (mode == "Prefix")
            return isPrefix;

        // Check Whole Word Condition (Must be Prefix AND end at separator)
        if (mode == "Whole")
        {
            bool isSuffix = (idx + patternLen == line.length()) || !isalnum(line[idx + patternLen]);
            return isPrefix && isSuffix;
        }

        return true;
    }

public:
    TextSearch() {}

    bool loadFile(const string &path)
    {
        this->filePath = path;
        ifstream file(path);

        if (!file.is_open())
        {
            // Simple error handling
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
    void search(string keyword, bool caseSensitive = true, string matchMode = "Substring")
    {
        if (keyword.empty())
            return;

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

                if (j == m)
                {
                    // Match found at index (i - j)
                    int matchIndex = i - j;

                    // Verify Match Mode
                    if (isValidMatch(text, matchIndex, m, matchMode))
                    {
                        totalMatches++;

                        // Phase 4: Output Highlighting
                        // We construct a temporary string with brackets around the keyword
                        string highlighted = text;
                        highlighted.insert(matchIndex + m, "]");
                        highlighted.insert(matchIndex, "[");

                        cout << "Line " << (lineNum + 1) << ", Index " << matchIndex << ": " << highlighted << endl;
                    }

                    // Prepare for next match in same line
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

        cout << "--- Stats ---" << endl;
        cout << "Total Occurrences: " << totalMatches << endl;
        cout << "Time Taken (Searching): " << duration.count() << " microseconds" << endl;
    }

    // Phase 4: Replace Functionality
    void replace(string keyword, string target, string outPath, bool caseSensitive, string matchMode)
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
            // Start Timer (Measure logic only, ignore I/O where possible)
            auto start = chrono::high_resolution_clock::now();

            string newLine = "";
            int n = line.length();
            int i = 0;
            int j = 0;
            int lastAddedPos = 0; // Tracks what we've appended to newLine so far

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
                        // 1. Append text from lastPos up to matchIndex
                        newLine += line.substr(lastAddedPos, matchIndex - lastAddedPos);
                        // 2. Append Target
                        newLine += target;
                        // 3. Update lastPos
                        lastAddedPos = i;
                        replacements++;

                        // Reset J to 0 to avoid overlapping replacements (e.g. replacing 'ana' in 'banana')
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
            // Append remaining text
            newLine += line.substr(lastAddedPos);

            auto stop = chrono::high_resolution_clock::now();
            totalDuration += chrono::duration_cast<chrono::microseconds>(stop - start).count();

            // Write to file (Time excluded)
            outFile << newLine << "\n";
        }

        outFile.close();
        cout << "--- Replace Stats ---" << endl;
        cout << "Replaced " << replacements << " occurrences." << endl;
        cout << "Output saved to: " << outPath << endl;
        cout << "Time taken (Processing): " << totalDuration << " microseconds" << endl;
    }
};

string getFilePathFromArgs(int argc, char *argv[])
{
    string prefix = "--file=";
    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg.find(prefix) == 0)
        {
            return arg.substr(prefix.length());
        }
    }
    return "";
}

// example: tokenize('Query "hello world" c=s m=Whole r="hi there" o=output.txt')
// returns: ['Query', 'hello world', 'c=s', 'm=Whole', 'r=hi there', 'o=output.txt']
vector<string> tokenize(const string &str)
{
    vector<string> tokens;
    string token;
    bool inQuotes = false;
    for (size_t i = 0; i < str.size(); ++i)
    {
        char c = str[i];
        if (c == '\"')
        {
            if (inQuotes)
            {
                // Closing quote: push token even if empty (handles "")
                tokens.push_back(token);
                token.clear();
                inQuotes = false;
            }
            else
            {
                // Opening quote: enter quoted mode
                inQuotes = true;
            }
        }
        else if (isspace(static_cast<unsigned char>(c)) && !inQuotes)
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        else
        {
            token += c;
        }
    }
    if (!token.empty())
        tokens.push_back(token);
    return tokens;
}

int main(int argc, char *argv[])
{
    // 1. Load File
    string path = getFilePathFromArgs(argc, argv);
    if (path.empty())
    {
        cout << "Usage: ./textsearch --file=<path>" << endl;
        return 1;
    }

    TextSearch engine;
    if (!engine.loadFile(path))
        return 1;

    // 2. Interactive Loop
    cout << "\n--- Interactive Mode ---" << endl;
    cout << "Usage: Query <keyword> [c=i|s] [m=Whole|Prefix|Substring] [r=replace] [o=path]" << endl;
    cout << "Type 'exit' to quit." << endl;

    string inputLine;
    while (true)
    {
        cout << "\n> "; // Prompt
        if (!getline(cin, inputLine))
            break; // Handle EOF
        if (inputLine == "exit" || inputLine == "quit")
            break;
        if (inputLine.empty())
            continue;

        // 3. Parse Command
        vector<string> tokens = tokenize(inputLine);
        if (tokens.empty())
            continue;

        string keyword;
        int argStartIndex = 0;

        if (tokens[0] == "Query" && tokens.size() > 1)
        {
            keyword = tokens[1]; /// First arg is "Query", second is keyword
            argStartIndex = 2;
        }
        else
        {
            keyword = tokens[0];
            argStartIndex = 1;
        }

        if (keyword == ""){
            cout << "You can't Search with empty keyword";
            continue;
        }

        // 4. Parse Options
        bool caseSensitive = true;  // Default: Sensitive (s)
        string matchMode = "Whole"; // Default: Whole Word
        string replaceTarget = "";  // Default: No replace
        string outputPath = "";

        for (int i = argStartIndex; i < tokens.size(); i++)
        {
            string token = tokens[i];
            if (token.substr(0, 2) == "c=")
            {
                caseSensitive = (token.substr(2) == "s");
            }
            else if (token.substr(0, 2) == "m=")
            {
                matchMode = token.substr(2);
            }
            else if (token.substr(0, 2) == "r=")
            {
                replaceTarget = token.substr(2);
            }
            else if (token.substr(0, 2) == "o=")
            {
                outputPath = token.substr(2);
            }
        }

        // 5. Execute Search (Prints stats and highlighted text)
        engine.search(keyword, caseSensitive, matchMode);

        // 6. Execute Replace (If requested)
        if (!replaceTarget.empty())
        {
            if (outputPath.empty())
            {
                cout << "Error: Output path (o=path) is required for replacement." << endl;
            }
            else
            {
                engine.replace(keyword, replaceTarget, outputPath, caseSensitive, matchMode);
            }
        }
    }

    cout << "Goodbye." << endl;
    return 0;
}