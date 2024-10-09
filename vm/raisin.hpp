#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>
using namespace std;

namespace raisin {
class LimitedInt {
   private:
    int value;
    int max;
    int min;

   public:
    LimitedInt(int min, int max) {
        this->min = min;
        this->max = max;
        this->value = this->min;
    }

    LimitedInt operator+(int i) {
        int res = this->value + i;

        if (res < this->min) {
            this->value = this->min;
            this->operator+(i + this->min);
        } else if (res > this->max) {
            this->value = this->max;
            this->operator+(i - this->max);
        } else {
            this->value = res;
        }

        return *this;
    }

    LimitedInt operator-(int i) {
        int res = this->value - i;

        if (res < this->min) {
            this->value = this->min;
            this->operator-(i - this->min);
        } else if (res > this->max) {
            this->value = this->max;
            this->operator-(i + this->max);
        } else {
            this->value = res;
        }

        return *this;
    }

    void operator+=(int i) { this->value = this->operator+(i).value; }

    void operator-=(int i) { this->value = this->operator-(i).value; }

    void operator+=(LimitedInt i) { this->operator+=(i.value); }

    void operator-=(LimitedInt i) { this->operator-=(i.value); }

    bool operator==(int i) { return this->value == i; }

    bool operator==(LimitedInt i) { return this->value == i.value; }

    bool operator!=(int i) { return !this->operator==(i); }

    bool operator!=(LimitedInt i) { return !this->operator==(i); }

    LimitedInt operator+(LimitedInt i) { return this->operator+(i.value); }

    LimitedInt operator-(LimitedInt i) { return this->operator-(i.value); }

    void operator++(int _) {
        if (this->value > this->max) {
            this->value = this->min;
        } else {
            this->value++;
        }
    }

    void operator--(int _) {
        if (this->value < this->min) {
            this->value = this->max;
        } else {
            this->value--;
        }
    }

    void operator=(int i) {
        if (i < this->min) {
            this->value = this->min;
            this->operator=(i + this->max);
        } else if (i > this->max) {
            this->value = this->max;
            this->operator=(i - this->max);
        } else {
            this->value = i;
        }
    }

    void operator=(LimitedInt i) { this->operator=(i.value); }

    void set(int i) { this->operator=(i); }

    void set(LimitedInt i) { this->operator=(i); }

    const int& v() const { return this->value; }
};

class RaisinByte : public LimitedInt {
   public:
    RaisinByte() : LimitedInt(0, 255) {}
};

string RunRaisinBytecode(vector<int> bytes) {
    RaisinByte tape[30000] = {};
    LimitedInt ptr = *(new LimitedInt(0, 29999));

    ptr = 0;

    map<int, int> jumps = *(new map<int, int>());
    stack<int> jumpStack = *(new stack<int>());
    for (int i = 0; i < bytes.size(); i++) {
        if (bytes[i] == 4) {
            jumpStack.push(i);
        } else if (bytes[i] == 5) {
            int* p = (int*)malloc(sizeof(int));
            *p = jumpStack.top();
            jumpStack.pop();

            jumps[i] = *p;
            jumps[*p] = i;
        }
    }

    for (int bi = 0; bi < bytes.size(); bi++) {
        switch (bytes[bi]) {
            case 0:  // +
                tape[ptr.v()]++;
                break;
            case 1:  // -
                tape[ptr.v()]--;
                break;
            case 2:  // >
                ptr++;
                break;
            case 3:  // <
                ptr--;
                break;
            case 4:  // [
                if (tape[ptr.v()] == 0) {
                    bi = jumps[bi];
                }
                break;
            case 5:  // ]
                if (tape[ptr.v()] != 0) {
                    bi = jumps[bi];
                }
                break;
            case 6: {  // .
                char* ch = (char*)malloc(1);
                *ch = (char)tape[ptr.v()].v();
                if (*ch == 0) {
                    cout << to_string(tape[ptr.v()].v()) << "('')";
                } else {
                    cout << to_string(tape[ptr.v()].v()) << "('" << *ch << "')";
                }
            } break;
            case 7: {  // ,
                cout << "type a character: ";

                string* inp = new string();
                if (!getline(cin, *inp)) {
                    return "user input error";
                }

                if (inp->empty()) {
                    tape[ptr.v()].set(0);
                } else {
                    tape[ptr.v()].set(inp->c_str()[0]);
                }
            } break;

            case 8:  // _
                return "";

            case 9:  // '
                ptr += tape[ptr.v()];
                break;

            case 10:  // `
                ptr -= tape[ptr.v()];
                break;

            default: {
                string* err = new string();
                *err = "unknown opcode " + to_string(bytes[bi]);
                return (char*)(err->c_str());
            }
        }
    }

    return "";
}
}  // namespace raisin