#include "utils.h"
#include <cctype>
using namespace std;

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
    for (long long i = 0; i < str.size(); ++i)
    {
        char c = str[i];
        if (c == '\"')
        {
            if (inQuotes)
            {
                // Closing quote
                tokens.push_back(token);
                token.clear();
                inQuotes = false;
            }
            else
            {
                // Opening quote
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
