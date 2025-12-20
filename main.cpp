#include <iostream>
#include "TextSearch.h"
#include "utils.h"
using namespace std;

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
                caseSensitive = (token.substr(2) == "i" ? false : true);
            }
            else if (token.substr(0, 2) == "m=")
            {
                string mode = token.substr(2);
                if (mode == "Whole" || mode == "Prefix" || mode == "Substring")
                {
                    matchMode = mode;
                }
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

        // 5. Execute Search
        if (!outputPath.empty())
        {
            engine.search(keyword, outputPath, caseSensitive, matchMode);
        }
        else
        {
            engine.search(keyword, caseSensitive, matchMode);
        }

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