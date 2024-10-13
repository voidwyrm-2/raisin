// v1.2
#pragma once

#include <map>
#include <ostream>
#include <string>
#include <vector>
using namespace std;

namespace argpp {

/*
A command-line flag.
*/
struct Flag {
   private:
    string shorthand, sh, longhand, lh, description;
    bool _required, _storeTrue;

    bool valid_flag_char(char ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
               (ch >= '0' && ch <= '9') || ch == '-';
    }

    void validate_flag(string flag) {
        if (flag.empty() || flag == " ") return;

        int hyphens = 0;
        for (int i = 0; i < flag.size(); i++) {
            if (!this->valid_flag_char(flag[i])) {
                cout << "flags can only contain alphanumeric characters or "
                        "hyphens('-')\n";
                throw new exception();
            } else if (flag[i] == '-') {
                hyphens++;
            }
        }

        if (hyphens == flag.size()) {
            cout << "flags cannot only contain hyphens\n";
            throw new exception();
        } else if (!flag.empty()) {
            if (flag[0] == '-') {
                cout << "flags cannot begin with a hyphen\n";
                throw new exception();
            } else if (flag[flag.size() - 1] == '-') {
                cout << "flags cannot end with a hyphen\n";
                throw new exception();
            }
        }
    }

   public:
    Flag(string shorthand, string longhand = "", string description = "",
         bool required = false, bool storeTrue = true) {
        this->validate_flag(shorthand);
        this->sh = shorthand;
        this->shorthand = "-" + shorthand;
        this->validate_flag(longhand);
        this->lh = longhand;
        this->longhand = "--" + longhand;
        this->description = description;
        this->_required = required;
        this->_storeTrue = storeTrue;

        if (shorthand == "" && longhand == "") {
            cout << "the flag shorthand and longhand cannot both be empty\n";
            throw new exception();
        }
    }

    pair<string, string> help() {
        string flag = this->shorthand == "-" ? "" : this->shorthand;
        if (flag != "") flag += "  ";
        flag += this->longhand;

        return pair<string, string>(flag, this->description);
    }

    string usage() {
        string flag = this->shorthand == "-" ? "" : this->shorthand;
        if (flag != "") flag += "|";
        flag += this->longhand;
        if (!this->_storeTrue) flag += " <value>";
        return this->_required ? flag : "[" + flag + "]";
    }

    string mkey() { return this->sh + this->lh; }

    string rstr() {
        string flag = this->shorthand == "-" ? "" : this->shorthand;
        if (flag != "") flag += "/";
        flag += this->longhand;
        return flag;
    }

    bool required() { return this->_required; }
    bool storeTrue() { return this->_storeTrue; }

    bool operator==(string s) {
        return (bool)(this->shorthand == s || this->longhand == s);
    }
    bool operator!=(string s) { return !this->operator==(s); }
};

/*
Holds if the flag was found and the value of the flag if it has one.
*/
struct ParseResult {
   public:
    const string value;
    const bool exists;

    ParseResult(bool exists = false, string value = "")
        : exists(exists), value(value) {}

    bool operator==(bool b) { return (bool)(this->exists == b); }
    bool operator!=(bool b) { return !this->operator==(b); }

    bool operator==(string s) { return (bool)(this->value == s); }
    bool operator!=(string s) { return !this->operator==(s); }
};

class Parser {
   private:
    string name;
    string description;

   protected:
    vector<string> args;
    vector<Flag> flags;

    bool is_flag(string f) {
        if (!f.empty()) {
            if (f[0] == '-' && f.size() > 1) {
                return true;
            }
        }
        return false;
    }

    pair<bool, Flag>* is_valid_flag(string f) {
        for (Flag fl : this->flags) {
            if (fl == f) {
                return new pair<bool, Flag>(true, fl);
            }
        }
        return new pair<bool, Flag>(false, *(new Flag(" ")));
    }

    void swap(Flag* a, Flag* b) {
        Flag temp = *a;
        *a = *b;
        *b = temp;
    }

    void sort_flags() {
        for (int i = 0; i < this->flags.size() - 1; i++) {
            bool swapped = false;
            for (int j = 0; j < (this->args.size() - 1) - i; j++) {
                if (!this->flags[j].required() &&
                    this->flags[j + 1].required()) {
                    this->swap(&this->flags[j], &this->flags[j + 1]);
                    swapped = true;
                }
            }

            if (!swapped) break;
        }
    }

   public:
    Parser(int argc, char** argv, vector<Flag> flags, string name,
           string description = "") {
        this->args = *(new vector<string>());
        for (int i = 0; i < argc; i++) {
            this->args.push_back(argv[i]);
        }
        this->flags = flags;
        this->sort_flags();
        this->name = name;
        this->description = description;
    }

    Parser(vector<string> args, vector<Flag> flags, string name,
           string description = "") {
        this->args = args;
        this->flags = flags;
        this->sort_flags();
        this->name = name;
        this->description = description;
    }

    /*
    Generates the help message that usually shows when `-h` or `--help` is given
    as a flag.
    */
    string help() {
        string out = this->usage() + "\n";

        if (!this->description.empty()) {
            out += "\n";
            string s = "usage: " + this->name + " ";
            for (int i = 0; i < s.size(); i++) out += " ";
            out += this->description;
        }

        if (!this->description.empty()) out += "\n";
        out += "\nArguments:";

        vector<pair<string, string> > flagHelp =
            vector<pair<string, string> >();
        int min = 1000000;
        int max = 0;

        for (Flag f : this->flags) {
            auto h = f.help();
            if (h.first.size() > max) {
                max = h.first.size();
                // cout << "'" + h.first + "'(" + to_string(max) +
                //             ") is now largest\n";
            }
            if (h.first.size() < min) {
                min = h.first.size();
                // cout << "'" + h.first + "'(" + to_string(min) +
                //             ") is now smallest\n";
            }
            flagHelp.push_back(h);
        }

        string spacing = "  ";
        // for (int i = 0; i < max - min; i++) spacing += " ";

        for (pair<string, string> fh : flagHelp) {
            out += "\n  " + fh.first + spacing + fh.second;
        }

        while (out[out.size() - 1] == '\n') out.pop_back();

        return out;
    }

    /*
    Generates the usage, e.g. `usage: app [-h|--help] [-s|--say <value>]`.
    */
    string usage() {
        string use = this->name;
        string fu = this->flag_usage();
        if (!fu.empty()) use += " " + fu;
        return use;
    }

    /*
    Generates the flag part of the usage e.g. `[-h|--help] [-s|--say <value>]`.
    */
    string flag_usage() {
        string use = "";
        int c = 0;
        for (Flag f : this->flags) {
            use += use[use.size() - 1] == '\n' ? f.usage() : " " + f.usage();
            c++;
            if (c == 3) {
                c = 0;
                use += "\n";
            }
        }

        use = use.substr(1);

        if (use[use.size() - 1] == '\n') use.pop_back();

        return use;
    }

    /*
    Returns a pair containing the flags and the leftover arguments that aren't
    flags and aren't matched to flags.
    */
    pair<map<string, ParseResult>, vector<string> > parse() {
        map<string, ParseResult>* parsed = new map<string, ParseResult>();
        vector<string>* leftovers = new vector<string>();

        for (int i = 0; i < this->args.size(); i++) {
            if (this->is_flag(this->args[i])) {
                pair<bool, Flag>* valid = this->is_valid_flag(this->args[i]);
                if (valid->first) {
                    if (valid->second.storeTrue()) {
                        parsed->insert(pair<string, ParseResult>(
                            valid->second.mkey(), ParseResult(true)));
                    } else {
                        if (i + 1 >= this->args.size()) {
                            cout << "flag " << valid->second.rstr()
                                 << " requires an arguement\n";
                            throw new exception();
                        }

                        parsed->insert(pair<string, ParseResult>(
                            valid->second.mkey(),
                            ParseResult(true, this->args[i + 1])));
                        i++;
                    }
                } else {
                    cout << "unknown flag '" + this->args[i] + "'\n";
                    throw new exception();
                }
            } else {
                leftovers->push_back(this->args[i]);
            }
        }

        for (Flag f : this->flags) {
            if (!parsed->count(f.mkey())) {
                if (f.required()) {
                    cout << "required flag " + f.rstr() + " was not given\n";
                    throw new exception();
                }
                parsed->insert(
                    pair<string, ParseResult>(f.mkey(), ParseResult()));
            }
        }

        return pair<map<string, ParseResult>, vector<string> >(*parsed,
                                                               *leftovers);
    }
};
}  // namespace argpp