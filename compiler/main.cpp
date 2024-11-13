#include <fstream>
#include <iostream>

#include "../include/argpp.hpp"
using namespace std;
using namespace argpp;

#include "raisinc.hpp"

int main(int argc, char **argv) {
    Flag help("h", "help", "Lists the flags and usage");
    Flag version("v", "version", "Shows the current Raisic version");
    Flag output(
        "o", "output",
        "changes the default output name('out.rac') to the given one('*.rac')");
    auto flags = vector<Flag>();
    flags.push_back(help);
    flags.push_back(version);
    flags.push_back(output);

    Parser parser(argc, argv, flags, "raisinc");

    auto parsed = parser.parse();

    if (parsed.first.at(version.mkey()).exists) {
        cout << parser.help() << "\n";
        return 0;
    } else if (parsed.first.at(version.mkey()).exists) {
        cout << "Raisinc version 1.0\n";
        return 0;
    }

    string outputName = "out.rac";
    auto outputCmd = parsed.first.at(output.mkey());
    if (outputCmd.exists) {
        if (outputCmd.value.empty() || outputCmd.value == ".cmd") {
            cout << "output name cannot be empty\n";
            return 1;
        }
        outputName = outputCmd.value + ".rac";
    }

    if (parsed.second.size() != 2) {
        cout << "expected 'raisinc <path> " << parser.flag_usage() << "'\n";
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cout << "error: could not open file '" << argv[1] << "'\n";
        return 1;
    }

    string content = "";

    while (!file.eof()) {
        content += " ";
        content[content.size() - 1] = (char)file.get();
        if (file.eof()) break;
    }
    file.close();

    if (content.empty()) {
        cout << "file is empty";
        return 1;
    }

    pair<vector<char>, string> result = raisinc::Compile(content, false, true);
    if (!result.second.empty()) {
        cout << result.second << "\n";
        return 1;
    }

    return 0;
}
