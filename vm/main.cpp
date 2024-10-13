#include <fstream>
#include <iostream>

#include "../include/argpp.hpp"
using namespace std;
using namespace argpp;

#include "raisin.hpp"

int main(int argc, char** argv) {
    Flag help("h", "help", "Lists the flags and usage");
    Flag version("v", "version", "Shows the current Raisin version");
    auto flags = vector<Flag>();
    flags.push_back(help);
    flags.push_back(version);

    Parser parser(argc, argv, flags, "raisin");

    auto parsed = parser.parse();

    if (parsed.first.at(version.mkey()).exists) {
        cout << parser.help() << "\n";
        return 0;
    }
    if (parsed.first.at(version.mkey()).exists) {
        cout << "Raisin version 1.0\n";
        return 0;
    }

    if (parsed.second.size() != 2) {
        cout << "expected 'raisin <path> " << parser.flag_usage() << "'\n";
        return 1;
    }

    ifstream file(parsed.second[1]);
    if (!file.is_open()) {
        cout << "error: could not open file '" << parsed.second[1] << "'\n";
        return 1;
    }

    vector<int> content;

    while (!file.eof()) {
        content.push_back(file.get());
        if (file.eof()) {
            break;
        }
    }
    file.close();

    int fJump = 0;
    int bJump = 0;
    for (int b : content) {
        if (b == 4) {
            fJump++;
        } else if (b == 5) {
            bJump++;
        }
    }

    if (fJump != bJump) {
        cout << "invalid file format, please recompile or use a different "
                "file\n";
        return 1;
    }

    string err = raisin::RunRaisinBytecode(content);
    if (!err.empty()) {
        cout << "Raisin VM Error: " << err << "\n";
        return 1;
    }

    return 0;
}
