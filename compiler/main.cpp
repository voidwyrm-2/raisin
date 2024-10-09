#include <fstream>
#include <iostream>
using namespace std;

#include "raisic.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "expected 'raisic <path>'\n";
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cout << "error: could not open file '" << argv[1] << "'\n";
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

    raisic::CompResult* result = raisic::Compile(content);
    if (!result->err.empty()) {
        std::cout << result->err << "\n";
        return 1;
    }

    return 0;
}
