#include "parser.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

// ---------------- utils ----------------

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");

    if (first == string::npos)
        return "";

    size_t last = str.find_last_not_of(" \t\r\n");

    return str.substr(first, last - first + 1);
}

vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;

    stringstream ss(str);
    string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

// ---------------- RANGES ----------------

vector<Variable> parseRangesFile(const string& filename) {

    string filePath =  "../../datasets/ranges/" + filename;

    ifstream file(filePath);

    if (!file.is_open()) {
        throw runtime_error("Could not open ranges file: " + filePath);
    }

    vector<Variable> variables;
    string line;

    while (getline(file, line)) {

        line = trim(line);

        if (line.empty() || line[0] == '#')
            continue;

        size_t colonPos = line.find(':');

        if (colonPos == string::npos)
            throw runtime_error("Invalid line: " + line);

        string varName = trim(line.substr(0, colonPos));
        string rangePart = trim(line.substr(colonPos + 1));

        vector<string> tokens = split(rangePart, ',');

        LiveRange range;

        for (const string& token : tokens) {

            if (token.empty())
                continue;

            ProgramPoint p;
            p.isStart = false;
            p.isEnd = false;

            string num = token;

            if (token.back() == '+') {
                p.isStart = true;
                num = token.substr(0, token.size() - 1);
            }
            else if (token.back() == '-') {
                p.isEnd = true;
                num = token.substr(0, token.size() - 1);
            }

            p.line = stoi(num);
            range.points.push_back(p);
        }

        bool found = false;

        for (auto& v : variables) {
            if (v.name == varName) {
                v.ranges.push_back(range);
                found = true;
                break;
            }
        }

        if (!found) {
            Variable v;
            v.name = varName;
            v.ranges.push_back(range);
            variables.push_back(v);
        }
    }

    return variables;
}

// ---------------- REGISTERS ----------------

Registers parseRegistersFile(const string& filename) {

    string filePath =  "../../datasets/registers/" + filename;

    ifstream file(filePath);

    if (!file.is_open()) {
        throw runtime_error("Could not open registers file: " + filePath);
    }

    Registers r;

    string line;

    while (getline(file, line)) {

        line = trim(line);

        if (line.empty() || line[0] == '#')
            continue;

        size_t colonPos = line.find(':');

        if (colonPos == string::npos)
            throw runtime_error("Invalid line: " + line);

        string key = trim(line.substr(0, colonPos));
        string value = trim(line.substr(colonPos + 1));

        if (key == "registers") {
            r.registers = stoi(value);
        }
        else if (key == "algorithm") {

            vector<string> parts = split(value, ',');

            r.algorithm = parts[0];

            if (parts.size() > 1)
                r.parameter = stoi(parts[1]);
        }
    }

    return r;
}