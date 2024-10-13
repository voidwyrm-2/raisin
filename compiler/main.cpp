#include <fstream>
#include <iostream>

#include "../include/argpp.hpp"
using namespace std;
using namespace argpp;

#include "raisic.hpp"

int main(int argc, char** argv) {
    Flag help("h", "help", "Lists the flags and usage");
    Flag version("v", "version", "Shows the current Raisic version");
    auto flags = vector<Flag>();
    flags.push_back(help);
    flags.push_back(version);

    Parser parser(argc, argv, flags, "raisic");

    auto parsed = parser.parse();

    if (parsed.first.at(version.mkey()).exists) {
        cout << parser.help() << "\n";
        return 0;
    }
    if (parsed.first.at(version.mkey()).exists) {
        cout << "Raisic version 1.0\n";
        return 0;
    }

    if (parsed.second.size() != 2) {
        cout << "expected 'raisic <path> " << parser.flag_usage() << "'\n";
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cout << "error: could not open file '" << argv[1] << "'\n";
        return 1;
    }

    string content;

    while (!file.eof()) {
        char* ch = (char*)malloc(1);
        ch[0] = file.get();
        content.append(ch);
        free(ch);
        if (file.eof()) {
            break;
        }
    }
    file.close();

    // std::cout << "file content: '" << content << "'\n";

    if (content.empty()) {
        cout << "file is empty";
        return 1;
    }

    pair<vector<char>, string> result = raisic::Compile(content, false, true);
    if (!result.second.empty()) {
        cout << result.second << "\n";
        return 1;
    }

    return 0;
}
