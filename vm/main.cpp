#include <fstream>
#include <iostream>
using namespace std;

#include "raisin.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "expected 'raisin <path>'\n";
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cout << "error: could not open file '" << argv[1] << "'\n";
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
