#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <cstring>
#include <windows.h>
#include <shlobj.h>
#include <fstream>
#include <algorithm>
#include <regex>
using namespace std;
void checkParenthesesGlobal(const vector<string>& lines, vector<string>& errors) {
    vector<pair<char, int>> stack;
    for (size_t lineNumber = 0; lineNumber < lines.size(); ++lineNumber) {
        const string& line = lines[lineNumber];

        for (size_t i = 0; i < line.size(); ++i) {
            char ch = line[i];
            if (ch == '(' || ch == '[' || ch == '{') {
                stack.emplace_back(ch, lineNumber + 1);
            } else if (ch == ')' || ch == ']' || ch == '}') {
                if (stack.empty()) {
                    errors.push_back("Error: Unmatched closing bracket '" + string(1, ch) +
                                     "' at line " + to_string(lineNumber + 1) +
                                     ", position " + to_string(i + 1));
                } else {
                    char top = stack.back().first;
                    int openingLine = stack.back().second;

                    if ((ch == ')' && top == '(') ||
                        (ch == ']' && top == '[') ||
                        (ch == '}' && top == '{')) {
                        stack.pop_back();
                    } else {
                        errors.push_back("Error: Mismatched brackets '" + string(1, top) + "' (line " +
                                         std::to_string(openingLine) + ") and '" + string(1, ch) +
                                         "' at line " + to_string(lineNumber + 1) +
                                         ", position " + to_string(i + 1));
                        stack.pop_back();
                    }
                }
            }
        }
    }
    while (!stack.empty()) {
        char top = stack.back().first;
        int openingLine = stack.back().second;
        errors.push_back("Error: Unmatched opening bracket '" + string(1, top) +
                         "' at line " + to_string(openingLine));
        stack.pop_back();
    }
}
vector<string> generate_permutations(string word) {
    vector<string> swapped_words;
    int len = word.size();
    for (int i = 0; i < len; ++i) {
        for (int j = i + 1; j < len; ++j) {
            string temp = word;
            swap(temp[i], temp[j]);
            swapped_words.push_back(temp);
        }
    }
    return swapped_words;
}
void find_misspelled_words(const string& text, const vector<string>& functions, vector<string>& errors, int linenum) {
    for(const string& correct_word : functions) {
        vector<string> permutations = generate_permutations(correct_word);
        for (const string& perm : permutations) {
            if (perm != correct_word) {
                regex pattern("\\b" + perm + "\\b", regex_constants::icase);
                if (regex_search(text, pattern)) {
                    errors.push_back("this problem was found: " + perm + " maybe you meant:  " + correct_word + " (line " +
                                     to_string(linenum) + ")");
                }
            }
        }
    }
}
void checkDataTypeFunctionErrors(const string& input, const vector<string>& datatypes, const vector<string>& functions, vector<string>& errors, int linenum) {
    string dataTypePattern = "\\b(";
    for (size_t i = 0; i < datatypes.size(); ++i) {
        dataTypePattern += datatypes[i];
        if (i != datatypes.size() - 1) dataTypePattern += "|";
    }
    dataTypePattern += ")\\b";
    string functionPattern = "\\b(";
    for (size_t i = 0; i < functions.size(); ++i) {
        functionPattern += functions[i];
        if (i != functions.size() - 1) functionPattern += "|";
    }
    functionPattern += ")\\b";
    regex pattern(dataTypePattern + "\\s+" + functionPattern);
    smatch match;
    auto words_begin = std::sregex_iterator(input.begin(), input.end(), pattern);
    auto words_end = std::sregex_iterator();
    for (auto it = words_begin; it != words_end; ++it) {
        errors.push_back("Error: Incorrect usage of data type '" + it->str(1) +
                         "' before function '" + it->str(2) + "'" + "(line " + to_string(linenum) + ")");
    }
}
void checkUnclosedComments(const vector<string>& lines, vector<string>& errors) {
    bool inComment = false;
    for (size_t lineNumber = 0; lineNumber < lines.size(); ++lineNumber) {
        const string& line = lines[lineNumber];
        size_t pos = 0;
        while (pos < line.size()) {
            if (!inComment) {
                size_t start = line.find("/*", pos);
                if (start != string::npos) {
                    inComment = true;
                    pos = start + 2;
                } else {
                    break;
                }
            }
            if (inComment) {
                size_t end = line.find("*/", pos);
                if (end != string::npos) {
                    inComment = false;
                    pos = end + 2;
                } else {
                    break;
                }
            }
        }
    }
    if (inComment) {
        errors.push_back("Error: Unclosed comment block detected.");
    }
}
void checkInvalidOperators(const vector<string>& lines, vector<string>& errors) {
    regex invalidOperators(R"((\+\+\+|---|\*\*|\/\/|==\+|!=\-|<<>|>>\*|&&\||\|\|&|\+==|==\+|\+\+=))");

    for (size_t lineNumber = 0; lineNumber < lines.size(); ++lineNumber) {
        if (regex_search(lines[lineNumber], invalidOperators)) {
            errors.push_back("Error: Invalid operator detected on line " + to_string(lineNumber + 1));
        }
    }
}
void extractIdentifiers(const vector<string>& lines, const vector<string>& datatypes, vector<string>& variables, map<string, int>& manualfunctions) {
    if (datatypes.empty()) return;
    ostringstream datatypeStream;
    for (size_t i = 0; i < datatypes.size(); ++i) {
        if (i > 0) datatypeStream << "|";
        datatypeStream << datatypes[i];
    }
    string datatypeRegex = "\\b(" + datatypeStream.str() + ")\\s+(\\w+)";
    regex functionPattern(datatypeRegex + "\\s*\\(([^)]*)\\)");
    regex variablePattern(datatypeRegex + "\\s*([=;,]|$)");
    bool reachedMain = false;
    for (const string& line : lines) {
        if (regex_search(line, regex("\\bint\\s+main\\s*\\("))) {
            reachedMain = true;
        }
        if (reachedMain) continue;
        smatch match;
        string temp = line;
        while (regex_search(temp, match, functionPattern)) {
            string functionName = match[2].str();
            string params = match[3].str();
            int paramCount = params.empty() ? 0 : count(params.begin(), params.end(), ',') + 1;
            if (params.find_first_not_of(" ") == string::npos) {
                paramCount = 0;
            }
            manualfunctions[functionName] = paramCount;
            temp = match.suffix().str();
        }
        temp = line;
        while (regex_search(temp, match, variablePattern)) {
            string variableName = match[2].str();
            if (!variableName.empty()) {
                variables.push_back(variableName);
            }
            temp = match.suffix().str();
        }
    }
}
void validateFunctionCalls(const string& line, const map<string, int>& manualfunctions, vector<string>& errors) {
    for (const auto& func : manualfunctions) {
        regex functionCallPattern("\\b" + func.first + "\\s*\\(([^)]*)\\)");
        smatch match;
        string temp = line;
        while (regex_search(temp, match, functionCallPattern)) {
            string params = match[1].str();
            int paramCount = params.empty() ? 0 : count(params.begin(), params.end(), ',') + 1;
            if (params.find_first_not_of(" ") == string::npos) {
                paramCount = 0;
            }
            if (paramCount != func.second) {
                errors.push_back("Error: Function '" + func.first + "' expects " + to_string(func.second) +
                                 " arguments, but " + to_string(paramCount) + " provided.");
            }
            temp = match.suffix().str();
        }
    }
}
bool needsSemicolon(const string& line) {
    string trimmed = regex_replace(line, regex("^\\s+|\\s+$"), "");

    if (regex_match(trimmed, regex("^\\s*(if|else if|else)\\b.*$"))) {
        return false;
    }

    if (regex_match(trimmed, regex(".*\\{\\s*$")) || regex_match(trimmed, regex("^\\s*}\\s*$"))) {
        return false;
    }
    if (regex_match(trimmed, regex("^\\s*(while|for|switch)\\s*\\(.*\\)\\s*$"))) {
        return false;
    }
    if (regex_match(trimmed, regex("^\\s*(int|float|double|long long|char|bool|void|array|pointer|string|struct|union|class|enum|vector|list|stack|queue|map|set)\\s+\\w+\\s*\\(.*\\)\\s*(const)?\\s*\\{?$"))) {
        return false;
    }
    if (regex_match(trimmed, regex("^\\s*(cin|cout|cerr|clog|return|abs|acos|asin|atan|for|while|switch|case|atan2|ceil|cos|cosh|exp|fabs|floor|fmod|frexp|ldexp|log|log10|modf|pow|sin|sinh|sqrt|tan|tanh|round|trunc|printf|scanf|gets|puts|sprintf|sscanf|tolower|toupper|isalnum|isalpha|iscntrl|isdigit|isgraph|islower|isprint|ispunct|isspace|isupper|isxdigit|memchr|memcmp|memcpy|memmove|memset|strcat|strchr|strcmp|strcpy|strcspn|strlen|strncat|strncmp|strncpy|strpbrk|strrchr|strspn|strstr|strtok|atoi|atof|atol|strtod|strtol|strtoul|sort|max|min|reverse|find|count|accumulate|lower_bound|upper_bound|binary_search|next_permutation|prev_permutation|to_string|stoi|stol|stoll|stof|stod|stold|ifstream|ofstream|fstream|)\\b.*[^;{}\\s]$"))) {
        return true;
    }
    return false;
}
void checkSemicolon(const string& line, vector<string>& errors, int lineNumber) {
    if (needsSemicolon(line)) {
        errors.push_back("Error on line " + to_string(lineNumber) + ": Missing semicolon.");
    }
}
void checkStringDefinitions(const vector<string>& lines, vector<string>& errors) {
    regex correctString(R"(^\s*string\s+\w+\s*=\s*\".*\"\s*;?\s*$)");
    regex incorrectString(R"(^\s*string\s+\w+\s*=\s*[^"].*[^"]\s*;?\s*$)");
    for (size_t i = 0; i < lines.size(); ++i) {
        if (!regex_match(lines[i], correctString) && regex_match(lines[i], incorrectString)) {
            errors.push_back("Error on line " + to_string(i + 1) + ": String values must be enclosed in double quotes.");
        }
    }
}
/*void checkUndefinedVariables(const vector<string>& lines, const vector<string>& variables, const map<string, int>& manualfunctions, const vector<string>& datatypes, const vector<string>& keywords, vector<string>& errors)
{
    regex identifierPattern("\\b[a-zA-Z_][a-zA-Z0-9_]*\\b");
    for (size_t lineNumber = 0; lineNumber < lines.size(); ++lineNumber) {
        smatch match;
        string line = lines[lineNumber];
        while (regex_search(line, match, identifierPattern)) {
            string identifier = match.str();
            if (find(datatypes.begin(), datatypes.end(), identifier) != datatypes.end()) {
                line = match.suffix();
                continue;
            }
            if (manualfunctions.find(identifier) != manualfunctions.end()) {
                line = match.suffix();
                continue;
            }
            if (find(keywords.begin(), keywords.end(), identifier) != keywords.end()) {
                line = match.suffix();
                continue;
            }
            if (find(variables.begin(), variables.end(), identifier) == variables.end()) {
                errors.push_back("Error: Undefined variable '" + identifier + "' at line " + to_string(lineNumber + 1));
            }
            line = match.suffix();
        }
    }
}*/
void suggestSwitchCaseOptimization(const vector<string>& lines, vector<string>& recommendations) {
    regex ifPattern(R"(if\s*\(\s*(\w+)\s*==\s*(\d+)\s*\))");
    regex elseIfPattern(R"(else\s+if\s*\(\s*(\w+)\s*==\s*(\d+)\s*\))");
    vector<pair<int, string>> conditions;
    vector<string> caseBodies;
    string variable = "";
    bool collecting = false;
    string currentBody = "";
    for (size_t i = 0; i < lines.size(); i++) {
        smatch match;
        if (regex_search(lines[i], match, ifPattern)) {
            if (!conditions.empty()) {
                caseBodies.push_back(currentBody);
                currentBody.clear();
            }
            variable = match[1].str();
            conditions.push_back({i, match[2].str()});
            collecting = true;
        }
        else if (regex_search(lines[i], match, elseIfPattern)) {
            if (!conditions.empty()) {
                caseBodies.push_back(currentBody);
                currentBody.clear();
            }
            if (variable == match[1].str()) {
                conditions.push_back({i, match[2].str()});
            }
        }
        else if (collecting) {
            if (lines[i].find('}') != string::npos) {
                collecting = false;
            } else {
                currentBody += "    " + lines[i] + "\n";
            }
        }
        else {
            if (!conditions.empty()) {
                caseBodies.push_back(currentBody);
                currentBody.clear();
                if (conditions.size() >= 3) {
                    string suggestion = "Suggestion: Convert lines " +
                                        to_string(conditions.front().first + 1) + " to " +
                                        to_string(conditions.back().first + 1) +
                                        " into a switch-case:\nswitch(" + variable + ") {\n";
                    for (size_t j = 0; j < conditions.size(); j++) {
                        suggestion += "    case " + conditions[j].second + ":\n";
                        suggestion += caseBodies[j] + "        break;\n";
                    }
                    suggestion += "}";
                    recommendations.push_back(suggestion);
                }
                conditions.clear();
                caseBodies.clear();
            }
        }
    }
    if (!conditions.empty()) {
        caseBodies.push_back(currentBody);
        currentBody.clear();
        if (conditions.size() >= 3) {
            string suggestion = "Suggestion: Convert lines " +
                                to_string(conditions.front().first + 1) + " to " +
                                to_string(conditions.back().first + 1) +
                                " into a switch-case:\nswitch(" + variable + ") {\n";
            for (size_t j = 0; j < conditions.size(); j++) {
                suggestion += "    case " + conditions[j].second + ":\n";
                suggestion += caseBodies[j] + "        break;\n";
            }
            suggestion += "}";
            recommendations.push_back(suggestion);
        }
    }
}
void suggestForLoopOptimization(const vector<string>& lines, vector<string>& recommendations) {
    regex coutPattern(R"(cout\s*<<\s*\"?(\d+)\"?\s*<<\s*endl\s*;)");
    vector<pair<int, int>> sequence;
    for (size_t i = 0; i < lines.size(); i++) {
        smatch match;
        if (regex_search(lines[i], match, coutPattern)) {
            int num = stoi(match[1].str());
            sequence.push_back({i, num});
        } else {
            if (sequence.size() >= 3) {
                int start = sequence.front().second;
                int end = sequence.back().second;
                bool isSequential = true;
                for (size_t j = 1; j < sequence.size(); j++) {
                    if (sequence[j].second != sequence[j - 1].second + 1) {
                        isSequential = false;
                        break;
                    }
                }
                if (isSequential) {
                    recommendations.push_back("Suggestion: Convert lines " + to_string(sequence.front().first + 1) + " to " + to_string(sequence.back().first + 1) + " into a for loop: \n for(int i = " + to_string(start) + "; i <= " + to_string(end) + "; i++) {\n    cout << i << endl;\n}");
                }
            }
            sequence.clear();
        }
    }
    if (sequence.size() >= 3) {
        int start = sequence.front().second;
        int end = sequence.back().second;
        recommendations.push_back("Suggestion: Convert lines " + to_string(sequence.front().first + 1) + " to " + to_string(sequence.back().first + 1) + " into a for loop: \n for(int i = " + to_string(start) + "; i <= " + to_string(end) + "; i++) {\n    cout << i << endl;\n}");
    }
}
void calldebug(const vector<string>& lines, vector<string>& errors, vector<string>& recommendations, vector<string>& variables, map<string, int>& manualfunctions, const vector<string>& functions, const vector<string>& datatypes, const vector<string>& operators)
{
    extractIdentifiers(lines, datatypes, variables, manualfunctions);
    checkParenthesesGlobal(lines, errors);
    checkUnclosedComments(lines, errors);
    checkInvalidOperators(lines, errors);
    extractIdentifiers(lines, datatypes, variables, manualfunctions);
    checkStringDefinitions(lines, errors);
    suggestSwitchCaseOptimization(lines, recommendations);
    suggestForLoopOptimization(lines, recommendations);
    for(size_t i = 0; i < lines.size(); i ++) {
        find_misspelled_words(lines[i], functions, errors, i+1);
        checkDataTypeFunctionErrors(lines[i], datatypes, functions, errors, i+1);
        validateFunctionCalls(lines[i], manualfunctions, errors);
        checkSemicolon(lines[i], errors, i+1);
    }
}
struct Selection {
    int startLine;
    int startPos;
    int endLine;
    int endPos;
    bool active;
};
Selection selection = {-1, -1, -1, -1, false};


bool useiostream = false;
bool usecmath = false;
bool usevector = false;
bool usestring = false;
bool usemap = false;
bool useset = false;
bool usealgorithm = false;



const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
void ensureLastLineVisible(int currentLine, int &scrollOffset, int SCREEN_HEIGHT, int LINE_HEIGHT, int totalLines);
bool lightmode = false;
std::vector<std::string> keywords = {"if", "while", "class", "else", "else if", "switch", "case", "default", "for", "do", "break", "continue", "return"};
std::vector<std::string> dataTypes = {"int", "float", "double", "string", "char"};
std::vector<std::string> brackets = {"{}", "()", "[]"};
vector<string> recommendations;
vector<string> variables;
map<string, int> manualfunctions;
vector<string> functions = {"cin", "cout", "cerr", "clog","return", "abs", "acos", "asin", "atan", "for", "while", "switch", "case", "atan2", "ceil", "cos", "cosh", "exp", "fabs", "floor", "fmod", "frexp", "ldexp", "log", "log10", "modf", "pow", "sin", "sinh", "sqrt", "tan", "tanh", "round", "trunc", "printf", "scanf", "gets", "puts", "sprintf", "sscanf", "tolower", "toupper", "isalnum", "isalpha", "iscntrl", "isdigit", "isgraph", "islower", "isprint", "ispunct", "isspace", "isupper", "isxdigit", "memchr", "memcmp", "memcpy", "memmove", "memset", "strcat", "strchr", "strcmp", "strcpy", "strcspn", "strlen", "strncat", "strncmp", "strncpy", "strpbrk", "strrchr", "strspn", "strstr", "strtok", "atoi", "atof", "atol", "strtod", "strtol", "strtoul", "sort", "max", "min", "reverse", "find", "count", "accumulate", "lower_bound", "upper_bound", "binary_search", "next_permutation", "prev_permutation", "to_string", "stoi", "stol", "stoll", "stof", "stod", "stold", "ifstream", "ofstream", "fstream"};
vector<string> datatypes = {"int", "float", "double", "long long", "char", "bool", "void", "array", "pointer", "string", "struct", "union", "class", "enum", "vector", "list", "stack", "queue", "map", "set"};
vector<string> operators = {"+", "-", "*", "/", "%", "++", "--", "==", "!=", ">", "<", ">=", "<=", "&&", "||", "!", "&", "|", "^", "~", "<<", ">>", "=", "+=", "-=", "*=", "/=", "%=", "?", ":", "&", "*", "sizeof", "typeid", "static_cast", "dynamic_cast", "const_cast", "reinterpret_cast", ".", "->", "::*"};

SDL_Color defaultColor;
SDL_Color keywordColor ;
SDL_Color dataTypeColor ;
SDL_Color numberColor ;
SDL_Color operatorColor ;
SDL_Color bracketColor ;
SDL_Color variableColor ;
SDL_Color functionColor ;
SDL_Color stringColor ;
SDL_Color commentColor ;
SDL_Color includeColor ;


void renderTextWithSyntaxHighlighting(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y) {
    if (text.empty()) return;
    if(lightmode) {
        defaultColor = {0, 0, 0, 255};
        keywordColor = {0, 51, 102, 255};
        dataTypeColor = {0, 128, 128, 255};
        numberColor = {128, 0, 128, 255};
        operatorColor = {128, 0, 0, 255};
        bracketColor = {184, 134, 11, 255};
        variableColor = {139, 0, 0, 255};
        functionColor = {255, 140, 0, 255};
        stringColor = {0, 100, 0, 255};
        commentColor = {128, 128, 128, 255};
        includeColor = {0,139,139,255};
    }
    else {
        defaultColor = {255, 255, 255, 255};
        keywordColor = {198, 120, 221, 255};
        dataTypeColor = {224, 108, 117, 255};
        numberColor = {209, 154, 102, 255};
        operatorColor = {213, 94, 0, 255};
        bracketColor = {171, 178, 191, 255};
        variableColor = {229, 192, 123, 255};
        functionColor = {97, 175, 254, 255};
        stringColor = {152, 195, 121, 255};
        commentColor = {92, 99, 112, 255};
        includeColor = {86,182,194,255};

    }
    size_t startPos = 0;
    int xPos = x;

    while (startPos < text.length()) {
        bool found = false;


        size_t commentPos = text.find("//", startPos);
        if (commentPos != std::string::npos && commentPos == startPos) {
            std::string commentText = text.substr(commentPos);
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, commentText.c_str(), commentColor);
            if (!textSurface) {
                std::cerr << "Error rendering comment: " << TTF_GetError() << std::endl;
                return;
            }
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth = textSurface->w;
            SDL_Rect renderQuad = {xPos, y, textWidth, TTF_FontHeight(font)};
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
            SDL_DestroyTexture(textTexture);
            return;
        }


        if (text.substr(startPos, 8) == "#include") {
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, "#include", includeColor);
            if (!textSurface) {
                std::cerr << "Error rendering #include: " << TTF_GetError() << std::endl;
                return;
            }
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth = textSurface->w;
            SDL_Rect renderQuad = {xPos, y, textWidth, TTF_FontHeight(font)};
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
            SDL_DestroyTexture(textTexture);

            xPos += textWidth;
            startPos += 8;
            continue;
        }


        if (text[startPos] == '"' || text[startPos] == '\'') {
            char quoteChar = text[startPos];
            size_t endPos = text.find(quoteChar, startPos + 1);
            if (endPos == std::string::npos) endPos = text.length();
            std::string quotedText = text.substr(startPos, endPos - startPos + 1);

            SDL_Surface* textSurface = TTF_RenderText_Blended(font, quotedText.c_str(), stringColor);
            if (!textSurface) {
                std::cerr << "Error rendering string/character: " << TTF_GetError() << std::endl;
                return;
            }
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth = textSurface->w;
            SDL_Rect renderQuad = {xPos, y, textWidth, TTF_FontHeight(font)};
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
            SDL_DestroyTexture(textTexture);

            xPos += textWidth;
            startPos = endPos + 1;
            continue;
        }
        if (text[startPos] == '(' || text[startPos] == ')' || text[startPos] == '{' || text[startPos] == '}' || text[startPos] == '[' || text[startPos] == ']') {
            char bracket = text[startPos];
            std::string bracketText(1, bracket);

            SDL_Surface* textSurface = TTF_RenderText_Blended(font, bracketText.c_str(), bracketColor);
            if (!textSurface) {
                std::cerr << "Error rendering bracket: " << TTF_GetError() << std::endl;
                return;
            }
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth = textSurface->w;
            SDL_Rect renderQuad = {xPos, y, textWidth, TTF_FontHeight(font)};
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
            SDL_DestroyTexture(textTexture);

            xPos += textWidth;
            startPos++;
            continue;
        }




        for (const auto& keyword : keywords) {
            size_t pos = text.find(keyword, startPos);
            if (pos == startPos && (pos + keyword.length() == text.length() || std::isspace(text[pos + keyword.length()]))) {
                SDL_Surface* textSurface = TTF_RenderText_Blended(font, keyword.c_str(), keywordColor);
                if (!textSurface) {
                    std::cerr << "Error rendering keyword: " << TTF_GetError() << std::endl;
                    return;
                }
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int textWidth = textSurface->w;
                SDL_Rect renderQuad = {xPos, y, textWidth, TTF_FontHeight(font)};
                SDL_FreeSurface(textSurface);
                SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
                SDL_DestroyTexture(textTexture);

                xPos += textWidth;
                startPos += keyword.length();
                found = true;
                break;
            }
        }

        if (found) continue;


        for (const auto& dataType : dataTypes) {
            size_t pos = text.find(dataType, startPos);
            if (pos == startPos && (pos + dataType.length() == text.length() || std::isspace(text[pos + dataType.length()]))) {
                SDL_Surface* textSurface = TTF_RenderText_Blended(font, dataType.c_str(), dataTypeColor);
                if (!textSurface) {
                    std::cerr << "Error rendering data type: " << TTF_GetError() << std::endl;
                    return;
                }
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int textWidth = textSurface->w;
                SDL_Rect renderQuad = {xPos, y, textWidth, TTF_FontHeight(font)};
                SDL_FreeSurface(textSurface);
                SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
                SDL_DestroyTexture(textTexture);

                xPos += textWidth;
                startPos += dataType.length();
                found = true;
                break;
            }
        }

        if (found) continue;







        if (std::isdigit(text[startPos])) {
            size_t endPos = text.find_first_not_of("0123456789.", startPos);
            if (endPos == std::string::npos) endPos = text.length();
            std::string number = text.substr(startPos, endPos - startPos);

            SDL_Surface* textSurface = TTF_RenderText_Blended(font, number.c_str(), numberColor);
            if (!textSurface) {
                std::cerr << "Error rendering number: " << TTF_GetError() << std::endl;
                return;
            }
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth = textSurface->w;
            SDL_Rect renderQuad = {xPos, y, textWidth, TTF_FontHeight(font)};
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
            SDL_DestroyTexture(textTexture);

            xPos += textWidth;
            startPos = endPos;
            continue;
        }


        for (const auto& op : operators) {
            size_t pos = text.find(op, startPos);
            if (pos == startPos) {
                SDL_Surface* textSurface = TTF_RenderText_Blended(font, op.c_str(), operatorColor);
                if (!textSurface) {
                    std::cerr << "Error rendering operator: " << TTF_GetError() << std::endl;
                    return;
                }
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int textWidth = textSurface->w;
                SDL_Rect renderQuad = {xPos, y, textWidth, TTF_FontHeight(font)};
                SDL_FreeSurface(textSurface);
                SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
                SDL_DestroyTexture(textTexture);

                xPos += textWidth;
                startPos += op.length();
                found = true;
                break;
            }
        }

        if (found) continue;

        if (std::isspace(text[startPos])) {
            size_t endPos = text.find_first_not_of(" ", startPos);
            if (endPos == std::string::npos) endPos = text.length();
            std::string spaces = text.substr(startPos, endPos - startPos);

            SDL_Surface* textSurface = TTF_RenderText_Blended(font, spaces.c_str(), defaultColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth = textSurface->w;
            SDL_Rect renderQuad = {xPos, y, textWidth, TTF_FontHeight(font)};
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
            SDL_DestroyTexture(textTexture);

            xPos += textWidth;
            startPos = endPos;
            continue;
        }

        size_t endPos = text.find_first_of(" ", startPos);
        if (endPos == std::string::npos) endPos = text.length();
        std::string normalText = text.substr(startPos, endPos - startPos);

        SDL_Surface* textSurface = TTF_RenderText_Blended(font, normalText.c_str(), defaultColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        int textWidth = textSurface->w;
        SDL_Rect renderQuad = {xPos, y, textWidth, TTF_FontHeight(font)};
        SDL_FreeSurface(textSurface);
        SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
        SDL_DestroyTexture(textTexture);

        xPos += textWidth;
        startPos = endPos;
    }
}



void SaveFile(const std::string& name) {
    std::cout << "File saved as: " << name << std::endl;
}

std::vector<std::vector<std::string>> undoStack;
std::vector<std::vector<std::string>> redoStack;
const int MAX_UNDO_REDO = 5;

bool showEditBox = false;
SDL_Rect undoBox = {420, 30, 60, 30};
SDL_Rect redoBox = {420, 70, 60, 30};



void SaveToUndoStack(std::vector<std::string>& lines) {
    if (undoStack.size() >= MAX_UNDO_REDO) {
        undoStack.erase(undoStack.begin());
    }
    undoStack.push_back(lines);
    redoStack.clear();
}
void Undo(std::vector<std::string>& lines, int& currentLine, int& cursorPos) {
    if (!undoStack.empty()) {
        redoStack.push_back(lines);
        lines = undoStack.back();
        undoStack.pop_back();


        if (lines.empty()) {
            lines.push_back("");
        }
        currentLine = std::min(currentLine, (int)lines.size() - 1);
        cursorPos = std::min(cursorPos, (int)lines[currentLine].size());
    }
}

void Redo(std::vector<std::string>& lines, int& currentLine, int& cursorPos) {
    if (!redoStack.empty()) {
        undoStack.push_back(lines);
        lines = redoStack.back();
        redoStack.pop_back();


        if (lines.empty()) {
            lines.push_back("");
        }
        currentLine = std::min(currentLine, (int)lines.size() - 1);
        cursorPos = std::min(cursorPos, (int)lines[currentLine].size());
    }
}




bool showFileMenu = false;
SDL_Rect pNewBox = {360, 35, 100, 30};
SDL_Rect pSaveBox = {360, 70, 100, 30};
SDL_Rect exitBox = {360, 105, 100, 30};








bool isFirstSave = true;
std::string currentFilename = "";



string SaveFileToDesktop(std::vector<std::string>& lines, SDL_Renderer* renderer, TTF_Font* font) {
    char desktopPath[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath) != S_OK) {
        std::cerr << "خطا در دریافت مسیر دسکتاپ!" << std::endl;
        return "";
    }

    std::string filename;
    if (isFirstSave) {

        SDL_Window* textWindow = SDL_CreateWindow("Enter Filename", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 150, SDL_WINDOW_SHOWN);
        SDL_Renderer* textRenderer = SDL_CreateRenderer(textWindow, -1, SDL_RENDERER_ACCELERATED);

        std::string inputText = "";
        bool enterPressed = false;
        SDL_Event event;

        SDL_StartTextInput();
        while (!enterPressed) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    SDL_DestroyRenderer(textRenderer);
                    SDL_DestroyWindow(textWindow);
                    return "";
                } else if (event.type == SDL_TEXTINPUT) {
                    inputText += event.text.text;
                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_BACKSPACE && !inputText.empty()) {
                        inputText.pop_back();
                    } else if (event.key.keysym.sym == SDLK_RETURN && !inputText.empty()) {
                        filename = inputText;
                        enterPressed = true;
                    }
                }
            }

            SDL_SetRenderDrawColor(textRenderer, 0, 0, 0, 255);
            SDL_RenderClear(textRenderer);

            SDL_Surface* textSurface = TTF_RenderText_Solid(font, inputText.c_str(), {255, 255, 255});
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(textRenderer, textSurface);
                SDL_Rect textRect = {20, 50, textSurface->w, textSurface->h};
                SDL_FreeSurface(textSurface);
                SDL_RenderCopy(textRenderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }

            SDL_RenderPresent(textRenderer);
        }

        SDL_StopTextInput();
        SDL_DestroyRenderer(textRenderer);
        SDL_DestroyWindow(textWindow);


        SDL_StartTextInput();

        currentFilename = filename;
        isFirstSave = false;
    } else {
        filename = currentFilename;
    }

    std::string fullPath = std::string(desktopPath) + "\\" + filename + ".cpp";

    std::ofstream outFile(fullPath, std::ios::trunc);
    if (outFile.is_open()) {
        for (const auto& line : lines) {
            outFile << line << "\n";
        }
        outFile.close();
        std::cout << "✅ فایل ذخیره شد: " << fullPath << std::endl;
    } else {
        std::cerr << "⛔ خطا در ذخیره فایل!" << std::endl;
    }
    return fullPath;
}


std::vector<std::string> GetSavedFiles() {
    std::vector<std::string> files;
    char desktopPath[MAX_PATH];

    if (SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath) != S_OK) {
        std::cerr << "خطا در دریافت مسیر دسکتاپ!" << std::endl;
        return files;
    }

    std::string searchPath = std::string(desktopPath) + "\\*.cpp";
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return files;
    }

    do {
        files.push_back(findFileData.cFileName);
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
    return files;
}





bool selecting = false;
int selectionStart = -1;
int selectionEnd = -1;









int promptForLineNumber(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Window* inputWindow = SDL_CreateWindow("Go to Line", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 150, SDL_WINDOW_SHOWN);
    SDL_Renderer* inputRenderer = SDL_CreateRenderer(inputWindow, -1, SDL_RENDERER_ACCELERATED);

    std::string inputText = "";
    bool done = false;
    SDL_Event event;

    SDL_StartTextInput();
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = true;
            } else if (event.type == SDL_TEXTINPUT) {
                inputText += event.text.text;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_BACKSPACE && !inputText.empty()) {
                    inputText.pop_back();
                } else if (event.key.keysym.sym == SDLK_RETURN && !inputText.empty()) {
                    done = true;
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    SDL_DestroyRenderer(inputRenderer);
                    SDL_DestroyWindow(inputWindow);
                    return -1;
                }
            }
        }

        SDL_SetRenderDrawColor(inputRenderer, 0, 0, 0, 255);
        SDL_RenderClear(inputRenderer);

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, inputText.c_str(), {255, 255, 255});
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(inputRenderer, textSurface);
            SDL_Rect textRect = {20, 50, textSurface->w, textSurface->h};
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(inputRenderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }

        SDL_RenderPresent(inputRenderer);
    }

    SDL_StopTextInput();
    SDL_DestroyRenderer(inputRenderer);
    SDL_DestroyWindow(inputWindow);

    return std::stoi(inputText) - 1;
}

vector<string> split_lines(vector<string> input){
    vector<string> result;
    for(string s : input){
        string t = "";
        for(char c : s){
            if(c == '\n'){
                result.push_back(t);
                t = "";
            } else{
                t.push_back(c);
            }
        }
        result.push_back(t);
    }
    return result;
}

bool compile(const std::string &filename)
{
    std::string compileCommand = "g++ " + filename + " -o output";
    int result = std::system(compileCommand.c_str());
    return (result == 0);
}


void run(const std::string &filename)
{
    std::string runCommand = ".\\" + filename + ".exe ";
    std::system(runCommand.c_str());
}

void run_in_another_window(const std::string &filename)
{
#ifdef _WIN32
    std::string runCommand = "start cmd /c \"" + filename + " & pause\"";
#elif __linux__
    std::string runCommand = "x-terminal-emulator -e \"" + filename + "; read -n 1 -s -r -p 'Press any key to continue...'\"";
#elif __APPLE__
    std::string runCommand = "osascript -e 'tell application \"Terminal\" to do script \"" + filename + "\"'";
#else
    std::cerr << "Unsupported operating system." << std::endl;
    return;
#endif

    std::system(runCommand.c_str());
}


void debug(const std::string &filename)
{
    std::cout << "Debugging " << filename << " (simulated):\n";
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::string line;
        int lineNumber = 1;
        while (std::getline(file, line))
        {
            std::cout << lineNumber << ": " << line << std::endl;
            lineNumber++;
        }
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file for debugging." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    vector<string> allwords;
    for(string s : keywords){
        allwords.push_back(s);
    }
    for(string s : functions){
        allwords.push_back(s);
    }
    for(string s : datatypes){
        allwords.push_back(s);
    }

    vector<string> errors;
    vector<string> recommendations;
    vector<string> variables;
    map<string, int> manualfunctions;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Cshir",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    SDL_Surface* icon = SDL_LoadBMP("C:/Users/IT/Downloads/logo.bmp");
    if (icon) {
        SDL_SetWindowIcon(window, icon);
        SDL_FreeSurface(icon);
    } else {
        std::cerr << "خطا در بارگذاری آیکون: " << SDL_GetError() << std::endl;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    TTF_Font* font = TTF_OpenFont(R"(C:\Windows\Fonts\consola.ttf)", 24);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    std::vector<std::string> lines = {""};
    int currentLine = 0;
    int cursorPos = 1;
    int scrollOffset = 0;
    const int LINE_HEIGHT = TTF_FontHeight(font);

    Uint32 lastCursorToggle = SDL_GetTicks();
    bool cursorVisible = true;
    const Uint32 CURSOR_BLINK_INTERVAL = 500;

    SDL_Color textColor = {0, 150, 0};
    SDL_Rect savebutton = {740,0,60,30};
    SDL_Rect textRect = {savebutton.x + 5 , savebutton.y  , 50 , 25};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Save", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    bool saveuse = false;
    std::string filename;
    SDL_Event saveusing;

    SDL_Rect RUNbutton = {680,0,60,30};
    SDL_Rect textRectRUN = {RUNbutton.x + 5 , RUNbutton.y  , 50 , 25};
    SDL_Surface* textSurfaceRUN = TTF_RenderText_Solid(font, "Run", textColor);
    SDL_Texture* textTextureRUN = SDL_CreateTextureFromSurface(renderer, textSurfaceRUN);
    SDL_FreeSurface(textSurfaceRUN);

    SDL_Rect DEBUGCOMPILEbutton = {540,0,140,30};
    SDL_Rect textRectDEBUGCOMPILE = {DEBUGCOMPILEbutton.x + 5 , DEBUGCOMPILEbutton.y  , 130 , 25};
    SDL_Surface* textSurfaceDEBUGCOMPILE = TTF_RenderText_Solid(font, "Debug&Compile", textColor);
    SDL_Texture* textTextureDEBUGCOMPILE = SDL_CreateTextureFromSurface(renderer, textSurfaceDEBUGCOMPILE);
    SDL_FreeSurface(textSurfaceDEBUGCOMPILE);

    SDL_Rect VIEWbutton = {480,0,60,30};
    SDL_Rect textRectVIEW = {VIEWbutton.x + 5 , VIEWbutton.y  , 50 , 25};
    SDL_Surface* textSurfaceVIEW = TTF_RenderText_Solid(font, "View", textColor);
    SDL_Texture* textTextureVIEW = SDL_CreateTextureFromSurface(renderer, textSurfaceVIEW);
    SDL_FreeSurface(textSurfaceVIEW);

    SDL_Rect EDITbutton = {420,0,60,30};
    SDL_Rect textRectEDIT = {EDITbutton.x + 5 , EDITbutton.y  , 50 , 25};
    SDL_Surface* textSurfaceEDIT = TTF_RenderText_Solid(font, "Edit", textColor);
    SDL_Texture* textTextureEDIT = SDL_CreateTextureFromSurface(renderer, textSurfaceEDIT);
    SDL_FreeSurface(textSurfaceEDIT);

    SDL_Rect FILEbutton = {360,0,60,30};
    SDL_Rect textRectFILE = {FILEbutton.x + 5 , FILEbutton.y  , 50 , 25};
    SDL_Surface* textSurfaceFILE = TTF_RenderText_Solid(font, "File", textColor);
    SDL_Texture* textTextureFILE = SDL_CreateTextureFromSurface(renderer, textSurfaceFILE);
    SDL_FreeSurface(textSurfaceFILE);





    SDL_Event e;
    bool quit = false;

    std::vector<std::string> savedFiles = GetSavedFiles();
    SDL_Rect filePanel = {SCREEN_WIDTH - 200, 30, 200, SCREEN_HEIGHT};


    bool showModeBox = false;
    SDL_Rect modeBox = {480, 30, 60, 30};










    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > lastCursorToggle + CURSOR_BLINK_INTERVAL) {
            cursorVisible = !cursorVisible;
            lastCursorToggle = currentTime;
        }

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEWHEEL) {

                if (e.wheel.y > 0) {
                    scrollOffset = std::max(0, scrollOffset - LINE_HEIGHT);
                } else if (e.wheel.y < 0) {
                    scrollOffset += LINE_HEIGHT;
                }
            } else if (e.type == SDL_KEYDOWN) {

                bool shiftPressed = (e.key.keysym.mod & KMOD_SHIFT);

                if (shiftPressed && e.key.keysym.sym == SDLK_RIGHT) {
                    if (!selection.active) {
                        selection.startLine = currentLine;
                        selection.startPos = cursorPos;
                        selection.active = true;
                    }

                    if (cursorPos < lines[currentLine].size()) {
                        cursorPos++;
                    } else if (currentLine < lines.size() - 1) {
                        currentLine++;
                        cursorPos = 0;
                    }

                    selection.endLine = currentLine;
                    selection.endPos = cursorPos;
                }

                if (shiftPressed && e.key.keysym.sym == SDLK_LEFT) {
                    if (!selection.active) {
                        selection.startLine = currentLine;
                        selection.startPos = cursorPos;
                        selection.active = true;
                    }

                    if (cursorPos > 0) {
                        cursorPos--;
                    } else if (currentLine > 0) {
                        currentLine--;
                        cursorPos = lines[currentLine].size();
                    }

                    selection.endLine = currentLine;
                    selection.endPos = cursorPos;
                }
                if (e.key.keysym.sym == SDLK_DOWN) {
                    if (shiftPressed) {
                        if (!selection.active) {
                            selection.startLine = currentLine;
                            selection.startPos = cursorPos;
                            selection.active = true;
                        }

                        if (currentLine < lines.size() - 1) {
                            currentLine++;
                            cursorPos = std::min(cursorPos, (int)lines[currentLine].size());
                        }

                        selection.endLine = currentLine;
                        selection.endPos = cursorPos;
                    } else {
                        if (currentLine < lines.size() - 1) {
                            currentLine++;
                            cursorPos = std::min(cursorPos, (int)lines[currentLine].size());
                        }

                        if (selection.active) {
                            selection.active = false;
                            selection.startLine = selection.endLine = -1;
                            selection.startPos = selection.endPos = -1;
                        }
                    }
                }

                if (e.key.keysym.sym == SDLK_UP) {
                    if (shiftPressed) {
                        if (!selection.active) {
                            selection.startLine = currentLine;
                            selection.startPos = cursorPos;
                            selection.active = true;
                        }

                        if (currentLine > 0) {
                            currentLine--;
                            cursorPos = std::min(cursorPos, (int)lines[currentLine].size());
                        }

                        selection.endLine = currentLine;
                        selection.endPos = cursorPos;
                    } else {
                        if (currentLine > 0) {
                            currentLine--;
                            cursorPos = std::min(cursorPos, (int)lines[currentLine].size());
                        }

                        if (selection.active) {
                            selection.active = false;
                            selection.startLine = selection.endLine = -1;
                            selection.startPos = selection.endPos = -1;
                        }
                    }
                }


                else if (!(shiftPressed || (e.key.keysym.mod & KMOD_CTRL)) && selection.active) {
                    selection.active = false;
                    selection.startLine = selection.endLine = -1;
                    selection.startPos = selection.endPos = -1;
                }


                if (selection.active) {
                    int startLine = std::min(selection.startLine, selection.endLine);
                    int endLine = std::max(selection.startLine, selection.endLine);
                    int startPos = (selection.startLine < selection.endLine) ? selection.startPos : selection.endPos;
                    int endPos = (selection.startLine < selection.endLine) ? selection.endPos : selection.startPos;

                    std::string selectedText = "";
                    for (int i = startLine; i <= endLine; ++i) {
                        if (i == startLine && i == endLine) {
                            selectedText += lines[i].substr(startPos, endPos - startPos);
                        } else if (i == startLine) {
                            selectedText += lines[i].substr(startPos) + "\n";
                        } else if (i == endLine) {
                            selectedText += lines[i].substr(0, endPos);
                        } else {
                            selectedText += lines[i] + "\n";
                        }
                    }

                    std::cout << "Selected Text: " << selectedText << std::endl;
                }















                bool ctrlPressed = (e.key.keysym.mod & KMOD_CTRL);


                if ((e.key.keysym.mod & KMOD_CTRL) && e.key.keysym.sym == SDLK_c) {
                    if (selection.active) {
                        std::string copiedText = "";

                        if (selection.startLine == selection.endLine) {
                            int start = std::min(selection.startPos, selection.endPos);
                            int end = std::max(selection.startPos, selection.endPos);
                            copiedText = lines[selection.startLine].substr(start, end - start);
                        } else {
                            for (int i = selection.startLine; i <= selection.endLine; ++i) {
                                if (i == selection.startLine) {
                                    copiedText += lines[i].substr(selection.startPos) + "\n";
                                } else if (i == selection.endLine) {
                                    copiedText += lines[i].substr(0, selection.endPos);
                                } else {
                                    copiedText += lines[i] + "\n";
                                }
                            }
                        }

                        SDL_SetClipboardText(copiedText.c_str());
                        std::cout << "Copied: " << copiedText << std::endl;
                        selection.active = false;
                        selection.startLine = selection.endLine = -1;
                        selection.startPos = selection.endPos = -1;

                    }
                }



                if ((e.key.keysym.mod & KMOD_CTRL) && e.key.keysym.sym == SDLK_x) {
                    if (selection.active) {
                        std::string cutText = "";

                        if (selection.startLine == selection.endLine) {
                            int start = std::min(selection.startPos, selection.endPos);
                            int end = std::max(selection.startPos, selection.endPos);
                            cutText = lines[selection.startLine].substr(start, end - start);
                            lines[selection.startLine].erase(start, end - start);
                        } else {
                            for (int i = selection.startLine; i <= selection.endLine; ++i) {
                                if (i == selection.startLine) {
                                    cutText += lines[i].substr(selection.startPos) + "\n";
                                    lines[i] = lines[i].substr(0, selection.startPos);
                                } else if (i == selection.endLine) {
                                    cutText += lines[i].substr(0, selection.endPos);
                                    lines[i].erase(0, selection.endPos);
                                } else {
                                    cutText += lines[i] + "\n";
                                    lines.erase(lines.begin() + i);
                                    i--;
                                    selection.endLine--;
                                }
                            }
                        }

                        SDL_SetClipboardText(cutText.c_str());
                        cursorPos = selection.startPos;
                        currentLine = selection.startLine;

                        selection.active = false;
                        std::cout << "Cut: " << cutText << std::endl;
                        selection.active = false;
                        selection.startLine = selection.endLine = -1;
                        selection.startPos = selection.endPos = -1;

                    }
                }



                if ((e.key.keysym.mod & KMOD_CTRL) && e.key.keysym.sym == SDLK_v) {
                    char* clipboardText = SDL_GetClipboardText();
                    if (clipboardText && clipboardText[0] != '\0') {
                        std::cout << "Pasting text: " << clipboardText << std::endl;

                        std::string pasteText(clipboardText);
                        size_t pos = 0;

                        while ((pos = pasteText.find('\n')) != std::string::npos) {
                            std::string linePart = pasteText.substr(0, pos);

                            if (currentLine >= lines.size()) {
                                lines.push_back("");
                            }
                            lines[currentLine].insert(cursorPos, linePart);
                            pasteText.erase(0, pos + 1);

                            currentLine++;
                            if (currentLine >= lines.size()) {
                                lines.push_back("");
                            }
                            cursorPos = 0;
                        }

                        if (currentLine >= lines.size()) {
                            lines.push_back("");
                        }
                        lines[currentLine].insert(cursorPos, pasteText);
                        cursorPos += pasteText.size();
                    }
                    SDL_free(clipboardText);
                }


                if ((e.key.keysym.mod & KMOD_CTRL) && e.key.keysym.sym == SDLK_a) {
                    selection.startLine = 0;
                    selection.startPos = 0;
                    selection.endLine = lines.size() - 1;
                    selection.endPos = lines.back().size();
                    selection.active = true;
                }






                if ((e.key.keysym.mod & KMOD_CTRL) && e.key.keysym.sym == SDLK_g) {
                    int lineNumber = promptForLineNumber(renderer, font);
                    if (lineNumber >= 0 && lineNumber < lines.size()) {
                        currentLine = lineNumber;
                        cursorPos = 0;
                        ensureLastLineVisible(currentLine, scrollOffset, SCREEN_HEIGHT, LINE_HEIGHT, lines.size());
                        std::cout << "Moved to line: " << (lineNumber + 1) << std::endl;
                    } else {
                        std::cout << "Invalid line number!" << std::endl;
                    }
                }




                if (ctrlPressed && e.key.keysym.sym == SDLK_z) {
                    Undo(lines, currentLine, cursorPos);
                }

                else if (ctrlPressed && e.key.keysym.sym == SDLK_s) {
                    SaveFileToDesktop(lines, renderer, font);
                    savedFiles = GetSavedFiles();
                }


                if(e.key.keysym.sym == SDLK_LALT){
                    SaveToUndoStack(lines);
                    string to_insert = "";
                    int global_max = -1;
                    for(string word : allwords){
                        int max_match = -1;
                        for(int i = 1; i <= word.size() && i <= cursorPos; i++){
                            bool matches = true;
                            for(int j = 0; j < i; j++){
                                if(lines[currentLine][cursorPos - i + j] != word[j]){
                                    matches = false;
                                }
                            }
                            if(matches){
                                max_match = i;
                            }
                        }
                        if(max_match > 1 && global_max < max_match){
                            global_max = max_match;
                            to_insert = word.substr(max_match);
                        }
                    }
                    if(to_insert != ""){
                        lines[currentLine].insert(cursorPos, to_insert);
                        cursorPos += to_insert.size();
                    }
                }








                if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    SaveToUndoStack(lines);
                    if (cursorPos > 1 && cursorPos <= lines[currentLine].size()) {
                        lines[currentLine].erase(cursorPos - 1, 1);
                        cursorPos--;
                    } else if (currentLine > 0) {
                        cursorPos = lines[currentLine - 1].size();
                        lines[currentLine - 1] += lines[currentLine].substr(1, lines[currentLine].size());
                        lines.erase(lines.begin() + currentLine);
                        currentLine--;
                    }

                    if (lines.empty()) {
                        lines.push_back("");
                        currentLine = 0;
                        cursorPos = 0;
                    }
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    if (cursorPos <= lines[currentLine].size()) {
                        SaveToUndoStack(lines);
                        std::string remainder = lines[currentLine].substr(cursorPos);
                        lines[currentLine] = lines[currentLine].substr(0, cursorPos);
                        lines.insert(lines.begin() + currentLine + 1, remainder);
                        currentLine++;
                        cursorPos = 0;
                        ensureLastLineVisible(currentLine, scrollOffset, SCREEN_HEIGHT, LINE_HEIGHT, lines.size());
                    }
                }
                else if (e.key.keysym.sym == SDLK_TAB) {
                    lines[currentLine].insert(cursorPos, "    ");
                    cursorPos += 4;
                }else if (!shiftPressed && e.key.keysym.sym == SDLK_LEFT) {
                    if (cursorPos > 0) {
                        cursorPos -= 1;
                    } else if (currentLine > 0) {
                        currentLine -= 1;
                        cursorPos = lines[currentLine].size();
                    }
                }
                else if (!shiftPressed && e.key.keysym.sym == SDLK_RIGHT) {
                    if (cursorPos < lines[currentLine].size()) {
                        cursorPos++;
                    } else if (currentLine < lines.size() - 1) {
                        currentLine++;
                        cursorPos = 0;
                    }
                }
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;




                if(x >= 540 && x <= 680 && y >=0 && y <= 30){

                    calldebug(lines, errors, recommendations, variables, manualfunctions, functions, datatypes, operators);
                    SDL_Window* errorWindow = SDL_CreateWindow("Errors", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 800, SDL_WINDOW_SHOWN);
                    SDL_Renderer* errorRenderer = SDL_CreateRenderer(errorWindow, -1, SDL_RENDERER_ACCELERATED);


                    SDL_Color errorColor = {255, 255, 255, 255};
                    int errorY = 10;
                    for (const std::string& error : errors) {
                        SDL_Surface* textSurface = TTF_RenderText_Blended(font, error.c_str(), errorColor);
                        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(errorRenderer, textSurface);
                        SDL_Rect errorRect = {10, errorY, textSurface->w, textSurface->h};
                        SDL_FreeSurface(textSurface);
                        SDL_RenderCopy(errorRenderer, textTexture, nullptr, &errorRect);
                        SDL_DestroyTexture(textTexture);
                        errorY += 20;
                    }
                    vector<string> new_recommendations = split_lines(recommendations);
                    for (const std::string& recomm : new_recommendations) {
                        SDL_Surface* textSurface = TTF_RenderText_Blended(font, recomm.c_str(), errorColor);
                        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(errorRenderer, textSurface);
                        SDL_Rect errorRect = {10, errorY, textSurface->w, textSurface->h};
                        SDL_FreeSurface(textSurface);
                        SDL_RenderCopy(errorRenderer, textTexture, nullptr, &errorRect);
                        SDL_DestroyTexture(textTexture);
                        errorY += 20;
                    }
                    SDL_RenderPresent(errorRenderer);


                    bool runningerror = true;
                    SDL_Event errorevent;
                    while (runningerror) {
                        while (SDL_PollEvent(&errorevent)) {
                            if (errorevent.type == SDL_QUIT ||
                                (errorevent.type == SDL_KEYDOWN && errorevent.key.keysym.sym == SDLK_ESCAPE)) {
                                runningerror = false;
                            }
                        }
                    }
                    SDL_DestroyRenderer(errorRenderer);
                    SDL_DestroyWindow(errorWindow);


                }

                if(x >= 680 && x <= 740 && y >= 0 && y <= 30){
                    string address = SaveFileToDesktop(lines, renderer, font);
                    compile(address);
                    run_in_another_window("output");
                }










                if (x >= FILEbutton.x && x <= FILEbutton.x + FILEbutton.w &&
                    y >= FILEbutton.y && y <= FILEbutton.y + FILEbutton.h) {
                    showFileMenu = !showFileMenu;
                }
                if (showFileMenu && x >= pNewBox.x && x <= pNewBox.x + pNewBox.w &&
                    y >= pNewBox.y && y <= pNewBox.y + pNewBox.h) {
                    lines.clear();
                    lines.push_back("");
                    currentLine = 0;
                    cursorPos = 0;
                    scrollOffset = 0;
                    undoStack.clear();
                    redoStack.clear();


                    isFirstSave = true;
                    currentFilename = "";

                    showFileMenu = false;
                }


                if (showFileMenu && x >= pSaveBox.x && x <= pSaveBox.x + pSaveBox.w &&
                    y >= pSaveBox.y && y <= pSaveBox.y + pSaveBox.h) {
                    SaveFileToDesktop(lines, renderer, font);
                    savedFiles = GetSavedFiles();
                    showFileMenu = false;
                }


                if (showFileMenu && x >= exitBox.x && x <= exitBox.x + exitBox.w &&
                    y >= exitBox.y && y <= exitBox.y + exitBox.h) {
                    quit = true;
                }











                if (x >= EDITbutton.x && x <= EDITbutton.x + EDITbutton.w &&
                    y >= EDITbutton.y && y <= EDITbutton.y + EDITbutton.h) {
                    showEditBox = !showEditBox;
                }


                if (showEditBox && x >= undoBox.x && x <= undoBox.x + undoBox.w &&
                    y >= undoBox.y && y <= undoBox.y + undoBox.h) {
                    Undo(lines, currentLine, cursorPos);
                    showEditBox = false;
                }


                if (showEditBox && x >= redoBox.x && x <= redoBox.x + redoBox.w &&
                    y >= redoBox.y && y <= redoBox.y + redoBox.h) {
                    Redo(lines, currentLine, cursorPos);
                    showEditBox = false;
                }








                if (x >= VIEWbutton.x && x <= VIEWbutton.x + VIEWbutton.w &&
                    y >= VIEWbutton.y && y <= VIEWbutton.y + VIEWbutton.h) {
                    showModeBox = !showModeBox;
                }
                if (showModeBox && x >= modeBox.x && x <= modeBox.x + modeBox.w &&
                    y >= modeBox.y && y <= modeBox.y + modeBox.h) {
                    lightmode = !lightmode;
                    showModeBox = false;
                }










                if(x >= 740 && x <= 800 && y >= 0 && y <= 30 ) {
                    SaveFileToDesktop(lines, renderer, font);
                    savedFiles = GetSavedFiles();
                }

            }


            else if (e.type == SDL_TEXTINPUT) {
                if (e.text.text) {
                    SaveToUndoStack(lines);
                    string ins = e.text.text;
                    if(ins == "("){
                        ins = "()";
                    }
                    if(ins == "["){
                        ins = "[]";
                    }
                    if(ins == "{"){
                        ins = "{}";
                    }
                    if(ins == "\""){
                        ins = "\"\"";
                    }
                    lines[currentLine].insert(cursorPos, ins);
                    cursorPos += strlen(e.text.text);
                    ensureLastLineVisible(currentLine, scrollOffset, SCREEN_HEIGHT, LINE_HEIGHT, lines.size());
                }
            }
        }


        if(lightmode) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }

        SDL_Color textColor = {0, 0, 0, 255};

        errors.clear();
        int y = -scrollOffset;


        SDL_SetRenderDrawColor(renderer, 0, 0, 150, 150);
        SDL_RenderFillRect(renderer, &savebutton);
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

        SDL_SetRenderDrawColor(renderer, 150, 0, 0, 150);
        SDL_RenderFillRect(renderer, &RUNbutton);
        SDL_RenderCopy(renderer, textTextureRUN, nullptr, &textRectRUN);

        SDL_SetRenderDrawColor(renderer, 150, 0, 150, 150);
        SDL_RenderFillRect(renderer, &DEBUGCOMPILEbutton);
        SDL_RenderCopy(renderer, textTextureDEBUGCOMPILE, nullptr, &textRectDEBUGCOMPILE);

        SDL_SetRenderDrawColor(renderer, 150, 0, 0, 150);
        SDL_RenderFillRect(renderer, &VIEWbutton);
        SDL_RenderCopy(renderer, textTextureVIEW, nullptr, &textRectVIEW);

        SDL_SetRenderDrawColor(renderer, 0, 0, 150, 150);
        SDL_RenderFillRect(renderer, &EDITbutton);
        SDL_RenderCopy(renderer, textTextureEDIT, nullptr, &textRectEDIT);

        SDL_SetRenderDrawColor(renderer, 150, 0, 150, 150);
        SDL_RenderFillRect(renderer, &FILEbutton);
        SDL_RenderCopy(renderer, textTextureFILE, nullptr, &textRectFILE);





        if (showModeBox) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderFillRect(renderer, &modeBox);

            SDL_Surface* modeTextSurface = TTF_RenderText_Solid(font, "Mode", textColor);
            SDL_Texture* modeTextTexture = SDL_CreateTextureFromSurface(renderer, modeTextSurface);
            SDL_Rect modeTextRect = {modeBox.x + 5, modeBox.y + 5, modeTextSurface->w, modeTextSurface->h};
            SDL_FreeSurface(modeTextSurface);
            SDL_RenderCopy(renderer, modeTextTexture, NULL, &modeTextRect);
            SDL_DestroyTexture(modeTextTexture);
        }






        if (showEditBox) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);


            SDL_RenderFillRect(renderer, &undoBox);
            SDL_Surface* undoTextSurface = TTF_RenderText_Solid(font, "Undo", textColor);
            SDL_Texture* undoTextTexture = SDL_CreateTextureFromSurface(renderer, undoTextSurface);
            SDL_Rect undoTextRect = {undoBox.x + 5, undoBox.y + 5, undoTextSurface->w, undoTextSurface->h};
            SDL_FreeSurface(undoTextSurface);
            SDL_RenderCopy(renderer, undoTextTexture, NULL, &undoTextRect);
            SDL_DestroyTexture(undoTextTexture);


            SDL_RenderFillRect(renderer, &redoBox);
            SDL_Surface* redoTextSurface = TTF_RenderText_Solid(font, "Redo", textColor);
            SDL_Texture* redoTextTexture = SDL_CreateTextureFromSurface(renderer, redoTextSurface);
            SDL_Rect redoTextRect = {redoBox.x + 10, redoBox.y + 5, redoTextSurface->w, redoTextSurface->h};
            SDL_FreeSurface(redoTextSurface);
            SDL_RenderCopy(renderer, redoTextTexture, NULL, &redoTextRect);
            SDL_DestroyTexture(redoTextTexture);
        }


        if (showFileMenu) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

            SDL_RenderFillRect(renderer, &pNewBox);
            SDL_Surface* pNewTextSurface = TTF_RenderText_Solid(font, "P New", textColor);
            SDL_Texture* pNewTextTexture = SDL_CreateTextureFromSurface(renderer, pNewTextSurface);
            SDL_Rect pNewTextRect = {pNewBox.x + 5, pNewBox.y + 5, pNewTextSurface->w, pNewTextSurface->h};
            SDL_FreeSurface(pNewTextSurface);
            SDL_RenderCopy(renderer, pNewTextTexture, NULL, &pNewTextRect);
            SDL_DestroyTexture(pNewTextTexture);

            SDL_RenderFillRect(renderer, &pSaveBox);
            SDL_Surface* pSaveTextSurface = TTF_RenderText_Solid(font, "P Save", textColor);
            SDL_Texture* pSaveTextTexture = SDL_CreateTextureFromSurface(renderer, pSaveTextSurface);
            SDL_Rect pSaveTextRect = {pSaveBox.x + 5, pSaveBox.y + 5, pSaveTextSurface->w, pSaveTextSurface->h};
            SDL_FreeSurface(pSaveTextSurface);
            SDL_RenderCopy(renderer, pSaveTextTexture, NULL, &pSaveTextRect);
            SDL_DestroyTexture(pSaveTextTexture);

            SDL_RenderFillRect(renderer, &exitBox);
            SDL_Surface* exitTextSurface = TTF_RenderText_Solid(font, "Exit", textColor);
            SDL_Texture* exitTextTexture = SDL_CreateTextureFromSurface(renderer, exitTextSurface);
            SDL_Rect exitTextRect = {exitBox.x + 5, exitBox.y + 5, exitTextSurface->w, exitTextSurface->h};
            SDL_FreeSurface(exitTextSurface);
            SDL_RenderCopy(renderer, exitTextTexture, NULL, &exitTextRect);
            SDL_DestroyTexture(exitTextTexture);
        }

        for (size_t i = 0; i < lines.size(); ++i) {
            if (y + LINE_HEIGHT > 0 && y < SCREEN_HEIGHT) {
                if (lines[i].empty()) {
                    lines[i] = " ";
                }

                renderTextWithSyntaxHighlighting(renderer,font, lines[i],10,y);


                if(lines[i].find("#include <bits/stdc++.h>")!=-1) {
                    useiostream = true;
                    usecmath = true;
                    usealgorithm = true;
                    usemap = true;
                    useset = true;
                    usestring = true;
                    usevector = true;
                }
                if(lines[i].find("#include <iostream>")!=-1) {
                    useiostream = true;
                }
                if(lines[i].find("#include <cmath>")!=-1) {
                    usecmath = true;
                }




                if(((lines[i].find("cin")!=-1 || lines[i].find("cout")!=-1 || lines[i].find("endl")!=-1 || lines[i].find("getline")!=-1)) && !useiostream ) {
                    int t = count(errors.begin(), errors.end(),"not included iostream" );
                    if(t == 0) {
                        errors.push_back("not included iostream");
                    }
                }
                if(((lines[i].find("sqrt(")!=-1 || lines[i].find("pow(")!=-1 || lines[i].find("sin(")!=-1 || lines[i].find("cos(")!=-1 || lines[i].find("tan(")!=-1 || lines[i].find("abs(")!= -1 || lines[i].find("exp(") != -1 || lines[i].find("log(")!=-1 || lines[i].find("log10(")!=-1 || lines[i].find("floor(") != -1 || lines[i].find("ceil")!=-1)) && !usecmath) {
                    int s = std::count(errors.begin(), errors.end(),"not included cmath");
                    if(s == 0) {
                        errors.push_back("not included cmath");
                    }
                }
                if(lines[i].find("vector")!=-1 && !usevector ) {
                    int k = std::count(errors.begin(), errors.end(),"not included vector");
                    if(k == 0) {
                        errors.push_back("not included vector");
                    }
                }
                if(lines[i].find("map")!=-1 && !usemap ) {
                    int j = std::count(errors.begin(), errors.end(),"not included map");
                    if(j == 0) {
                        errors.push_back("not included map");
                    }
                }
                if(lines[i].find("set")!=-1 && !useset ) {
                    int h = std::count(errors.begin(), errors.end(),"not included set");
                    if(h == 0) {
                        errors.push_back("not included set");
                    }
                }
                if(lines[i].find("string")!=-1 && !usestring ) {
                    int d = std::count(errors.begin(), errors.end(),"not included string");
                    if(d == 0) {
                        errors.push_back("not included string");
                    }
                }





                if (i == currentLine) {
                    int cursorX = 0;
                    if (cursorPos > 0) {
                        TTF_SizeText(font, lines[i].substr(0, cursorPos).c_str(), &cursorX, nullptr);
                    }
                    cursorX += 10;
                    if(lightmode) {
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        SDL_RenderDrawLine(renderer, cursorX, y, cursorX, y + LINE_HEIGHT);
                    }
                    else {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_RenderDrawLine(renderer, cursorX, y, cursorX, y + LINE_HEIGHT);
                    }

                }
            }
            y += LINE_HEIGHT;
        }


        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &filePanel);

        SDL_Color fileTextColor = {255, 255, 255};
        int fileY = 35;

        if (savedFiles.empty()) {
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, "No file yet", fileTextColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {SCREEN_WIDTH - 190, fileY, textSurface->w, textSurface->h};
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        } else {
            for (const auto& fileName : savedFiles) {
                SDL_Surface* textSurface = TTF_RenderText_Solid(font, fileName.c_str(), fileTextColor);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect textRect = {SCREEN_WIDTH - 190, fileY, textSurface->w, textSurface->h};
                SDL_FreeSurface(textSurface);
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
                fileY += 30;
            }
        }


        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;


}


void ensureLastLineVisible(int currentLine, int &scrollOffset, int SCREEN_HEIGHT, int LINE_HEIGHT, int totalLines) {
    int cursorY = currentLine * LINE_HEIGHT - scrollOffset;
    if (cursorY < 0) {

        scrollOffset = currentLine * LINE_HEIGHT;
    } else if (cursorY + LINE_HEIGHT > SCREEN_HEIGHT) {

        scrollOffset = (currentLine + 1) * LINE_HEIGHT - SCREEN_HEIGHT;
    }


    int contentHeight = totalLines * LINE_HEIGHT;
    if (contentHeight > SCREEN_HEIGHT) {
        scrollOffset = std::min(scrollOffset, contentHeight - SCREEN_HEIGHT);
    } else {
        scrollOffset = 0;
    }
}